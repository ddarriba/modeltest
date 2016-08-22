/*
    Copyright (C) 2016 Alexandros Stamatakis, Tomas Flouri, Diego Darriba, Alexey Kozlov

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as
    published by the Free Software Foundation, either version 3 of the
    License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    Contact: Tomas Flouri <Tomas.Flouri@h-its.org>,
    Heidelberg Institute for Theoretical Studies,
    Schloss-Wolfsbrunnenweg 35, D-69118 Heidelberg, Germany
*/
#include "treeinfo.h"
#include "plldefs.h"

using namespace modeltest;

/* if not defined, branch length optimization will use
 * the same starting set of branch lengths for every topology */
#define PLLMOD_SEARCH_GREEDY_BLO

#ifdef DEBUG
    #define DBG(fmt, ...) do { //print_info(fmt, ##__VA_ARGS__); } while(0)
#else
    #define DBG(fmt, ...)
#endif

//const bool USE_FAST_EVAL = false;
const bool USE_FAST_EVAL = true;

typedef struct spr_params
{
  int thorough;
  int radius_min;
  int radius_max;
  int ntopol_keep;
} pllmod_search_params_t;

typedef struct rollback_list
{
  pll_tree_rollback_t * list;
  int current;
  mt_size_t size;
} pllmod_rollback_list_t;

typedef struct node_entry {
  pll_utree_t * p_node;
  pll_utree_t * r_node;
  double b1, b2, b3;
  double lh;
  mt_index_t rollback_slot;
} node_entry_t;

typedef struct bestnode_list
{
  node_entry_t * list;
  int current;
  mt_size_t size;
} pllmod_bestnode_list_t;


int node_counter(pll_utree_t * node, void * counter)
{
  if (pllmod_utree_is_tip(node))
    {
      *((int*) counter) += 1;
    }
  return PLL_SUCCESS;
}

int get_utree_tip_count(pll_utree_t * root)
{
  int counter = 0;

  if (pllmod_utree_is_tip(root))
    root = root->back;

  pllmod_utree_traverse_apply(root,
                           &node_counter,
                           NULL,
                           (void *) &counter);
  return counter;
}

int get_utree_edge_count(pll_utree_t * root)
{
  int tips = get_utree_tip_count(root);
  return 2 * tips - 3;
}

void query_allnodes_recursive(pll_utree_t * node, pll_utree_t ** buffer, int * index)
{
  if (node->next)
    {
      query_allnodes_recursive(node->next->back, buffer, index);
      query_allnodes_recursive(node->next->next->back, buffer, index);

      buffer[(*index)++] = node->next->next;
      buffer[(*index)++] = node->next;
      buffer[(*index)++] = node;
    }
}

int query_allnodes(pll_utree_t * root, pll_utree_t ** buffer)
{
  assert(root && buffer);

  int index = 0;

  query_allnodes_recursive(root->back, buffer, &index);
  query_allnodes_recursive(root, buffer, &index);

  return index;
}

pllmod_rollback_list_t * rollback_list_create(mt_size_t slots)
{
  pllmod_rollback_list_t * rollback_list = (pllmod_rollback_list_t *) calloc(1, sizeof(pllmod_rollback_list_t));
  rollback_list->list = (pll_tree_rollback_t *) calloc(slots, sizeof(pll_tree_rollback_t));
  rollback_list->size = slots;
  rollback_list->current = 0;
  return rollback_list;
}

void rollback_list_destroy(pllmod_rollback_list_t * rollback_list)
{
  free(rollback_list->list);
  free(rollback_list);
}

pll_tree_rollback_t * rollback_list_prev(pllmod_rollback_list_t * rollback_list)
{
  if (rollback_list->current > 0)
    rollback_list->current--;
  else
    rollback_list->current = rollback_list->size - 1;

   return rollback_list->list + rollback_list->current;
}

pll_tree_rollback_t * rollback_list_next(pllmod_rollback_list_t * rollback_list)
{
  if (rollback_list->current < (int) rollback_list->size - 1)
    rollback_list->current++;
  else
    rollback_list->current = 0;

   return rollback_list->list + rollback_list->current;
}

pllmod_bestnode_list_t * bestnode_list_create(mt_size_t slots)
{
  pllmod_bestnode_list_t * bestnode_list = (pllmod_bestnode_list_t *) calloc(1, sizeof(pllmod_bestnode_list_t));
  bestnode_list->list = (node_entry_t *) calloc(slots, sizeof(node_entry_t));
  bestnode_list->size = slots;
  bestnode_list->current = 0;
  return bestnode_list;
}

void bestnode_list_destroy(pllmod_bestnode_list_t * bestnode_list)
{
  free(bestnode_list->list);
  free(bestnode_list);
}

void bestnode_list_copy_entry(node_entry_t * dst, const node_entry_t * src)
{
  memcpy(dst, src, sizeof(node_entry_t));
}

void bestnode_list_save(pllmod_bestnode_list_t * best_node_list,
                        const node_entry_t * entry)
{
  node_entry_t * list = best_node_list->list;
  const mt_size_t list_size = best_node_list->size;

  mt_size_t idx = 0;
  while ((list[idx].p_node) && (entry->lh < list[idx].lh))
    {
      ++idx;
      if (idx == list_size)
        return;
    }

  mt_size_t j = list_size - 1;
  while (j > idx)
    {
      bestnode_list_copy_entry(&list[j], &list[j-1]);
      --j;
    }

  bestnode_list_copy_entry(&list[idx], entry);
}

int bestnode_list_next_index(pllmod_bestnode_list_t * best_node_list,
                          mt_index_t rollback_slot, int curr_index)
{
  assert(curr_index >= -1);

  node_entry_t * list = best_node_list->list;
  const int list_size = best_node_list->size;

  do
    {
      curr_index++;
      if (curr_index >= (int) list_size || !list[curr_index].p_node)
        return -1;
    }
  while (list[curr_index].rollback_slot != rollback_slot);

  return curr_index;
}

double optimize_bl_triplet(pll_utree_t * node,
                           pllmod_treeinfo_t * treeinfo,
                           double bl_min,
                           double bl_max,
                           int smoothings,
                           double epsilon)
{
  assert(treeinfo->partition_count == 1);
  unsigned int p = 0;

  return -1 * pllmod_opt_optimize_branch_lengths_local(treeinfo->partitions[p],
				    node,
				    treeinfo->param_indices[p],           /* params_index */
                                    bl_min,
                                    bl_max,
                                    epsilon,                                 /* logLH epsilon */
                                    smoothings,
				    1,                                    /* radius */
				    true);                                /* keep_update */
}

double optimize_bl_iterative(pllmod_treeinfo_t * treeinfo,
                             double bl_min,
                             double bl_max,
                             int smoothings,
                             double epsilon)
{
  assert(treeinfo->partition_count == 1);
  unsigned int p = 0;

  pllmod_treeinfo_compute_loglh(treeinfo, false);

  double new_loglh = -1 * pllmod_opt_optimize_branch_lengths_iterative(treeinfo->partitions[p], treeinfo->root,
                                                     treeinfo->param_indices[p],
                                                     bl_min,
                                                     bl_max,
                                                     epsilon,
                                                     smoothings,
                                                     1);

  return new_loglh;
}

pll_utree_t * best_reinsert_edge(pllmod_treeinfo_t * treeinfo,
                                 node_entry_t * entry,
                                 const pllmod_search_params_t * params,
                                 double bl_min,
                                 double bl_max,
                                 int smoothings,
                                 double epsilon)
{
  assert(treeinfo && entry && params);

  pll_utree_t * p_edge = entry->p_node;
  entry->r_node = NULL;
  entry->lh = PLLMOD_OPT_LNL_UNLIKELY;

  pll_utree_t ** regraft_nodes = (pll_utree_t **) calloc ((mt_size_t) 2 * treeinfo->tip_count, sizeof(pll_utree_t *));
  int regraft_edges = 0;

  /* save original branch lengths at the pruning point */
  double z1, z2, z3;
  z1 = p_edge->length;
  z2 = p_edge->next->length;
  z3 = p_edge->next->next->length;

  pllmod_treeinfo_set_root(treeinfo, p_edge);

  /* recompute all CLVs and p-matrices before pruning */
  pllmod_treeinfo_compute_loglh(treeinfo, false);

  pll_utree_t * orig_prune_edge = pllmod_utree_prune(p_edge);
  if (!orig_prune_edge)
  {
    /* check that errno was set correctly */
    assert(pll_errno & PLLMOD_TREE_ERROR_SPR_MASK);
    return PLL_FAILURE;
  }

  pllmod_treeinfo_set_root(treeinfo, orig_prune_edge);

  /* invalidate CLVs & p-matrix at the pruned edge */
  pllmod_treeinfo_invalidate_clv(treeinfo, orig_prune_edge);
  pllmod_treeinfo_invalidate_clv(treeinfo, orig_prune_edge->back);
  pllmod_treeinfo_invalidate_pmatrix(treeinfo, orig_prune_edge);

  /* get list of candidate regrafting nodes in the given distance range */
  mt_size_t regraft_count;
  int retval = pllmod_utree_nodes_at_edge_dist(treeinfo->root, regraft_nodes, &regraft_count, params->radius_min, params->radius_max);
  assert(retval == PLL_SUCCESS);

  mt_index_t j;
  pll_utree_t * r_edge;
  for (j = 0; j < regraft_count; ++j)
    {
      r_edge = regraft_nodes[j];

      /* do not re-insert back into the pruning branch */
      if (r_edge == orig_prune_edge || r_edge == orig_prune_edge->back)
        continue;

      regraft_edges++;

      /* regraft p_edge on r_edge*/
      double regraft_length = r_edge->length;

      /* regraft into the candidate branch */
      int retval = pllmod_utree_regraft(p_edge, r_edge);
      assert(retval == PLL_SUCCESS);

//      /* update branch lengths to place pruned subtree in the middle of regraft branch */
//      pllmod_utree_set_length(p_edge->next, regraft_length / 2.);
//      pllmod_utree_set_length(p_edge->next->next, regraft_length / 2.);

      /* invalidate p-matrices */
      pllmod_treeinfo_invalidate_pmatrix(treeinfo, p_edge->next);
      pllmod_treeinfo_invalidate_pmatrix(treeinfo, p_edge->next->next);

      /* place root at the pruning branch and invalidate CLV at the new root */
      pllmod_treeinfo_set_root(treeinfo, p_edge);
      pllmod_treeinfo_invalidate_clv(treeinfo, p_edge);

      /* save branch lengths */
      double b1, b2, b3;
      b1 = p_edge->length;
      b2 = p_edge->next->length;
      b3 = p_edge->next->next->length;

      double loglh;
      if (params->thorough)
        {
//          //print_info("exp. triplet: %d - %d, %d - %d, %d - %d\n",
//                     p_edge->clv_index, p_edge->back->clv_index,
//                     p_edge->next->clv_index, p_edge->next->back->clv_index,
//                     p_edge->next->next->clv_index, p_edge->next->next->back->clv_index
//                     );

//          pllmod_utree_set_length(p_edge->next, RAXML_BRLEN_DEFAULT);
//          pllmod_utree_set_length(p_edge->next->next, RAXML_BRLEN_DEFAULT);

          /* re-compute invalid CLVs and p-matrices */
          pllmod_treeinfo_compute_loglh(treeinfo, true);

          /* optimize 3 adjacent branches and get tree logLH */
          loglh = optimize_bl_triplet(p_edge,
                                      treeinfo,
                                      bl_min,
                                      bl_max,
                                      smoothings,
                                      epsilon);
        }
      else
        {
          /* re-compute invalid CLVs and p-matrices, and get tree logLH */
          loglh = pllmod_treeinfo_compute_loglh(treeinfo, true);
        }

      if (loglh > entry->lh)
        {
          entry->lh = loglh;
          entry->r_node = r_edge;
          entry->b1 = p_edge->length;
          entry->b2 = p_edge->next->length;
          entry->b3 = p_edge->next->next->length;
        }

      // restore original branch lengths
      pllmod_utree_set_length(p_edge, b1);
      pllmod_utree_set_length(p_edge->next, b2);
      pllmod_utree_set_length(p_edge->next->next, b3);

      /* rollback the REGRAFT */
      pll_utree_t * pruned_tree = pllmod_utree_prune(p_edge);
      pllmod_utree_set_length(pruned_tree, regraft_length);
      pllmod_treeinfo_invalidate_pmatrix(treeinfo, pruned_tree);
    }

  /* done with regrafting; restore old root */
  pllmod_treeinfo_set_root(treeinfo, orig_prune_edge);

  /* re-insert into the original pruning branch */
  retval = pllmod_utree_regraft(p_edge, orig_prune_edge);
  assert(retval == PLL_SUCCESS || (pll_errno & PLLMOD_TREE_ERROR_SPR_MASK));

  /* restore original branch length */
  pllmod_utree_set_length(p_edge, z1);
  pllmod_utree_set_length(p_edge->next, z2);
  pllmod_utree_set_length(p_edge->next->next, z3);

  /* invalidate p-matrices */
  pllmod_treeinfo_invalidate_pmatrix(treeinfo, p_edge);
  pllmod_treeinfo_invalidate_pmatrix(treeinfo, p_edge->next);
  pllmod_treeinfo_invalidate_pmatrix(treeinfo, p_edge->next->next);

  free(regraft_nodes);

  return entry->r_node;
}

double reinsert_nodes(pllmod_treeinfo_t * treeinfo, pll_utree_t ** nodes, int node_count,
                      pllmod_rollback_list_t * rollback_list,
                      pllmod_bestnode_list_t * best_node_list,
                      const pllmod_search_params_t * params,
                      double bl_min,
                      double bl_max,
                      int smoothings,
                      double epsilon)
{
  int i;

  double loglh   = pllmod_treeinfo_compute_loglh(treeinfo, false);
  double best_lh = loglh;

  node_entry_t spr_entry;

  pll_tree_rollback_t * rollback = rollback_list->list + rollback_list->current;

  for (i = 0; i < node_count; ++i)
    {
      pll_utree_t * p_edge = nodes[i];

      /* if remaining pruned tree would only contain 2 taxa, skip this node */
      if (pllmod_utree_is_tip(p_edge->next->back) && pllmod_utree_is_tip(p_edge->next->next->back))
        continue;

      assert(!pllmod_utree_is_tip(p_edge));

      spr_entry.p_node = p_edge;

      pll_utree_t * best_r_edge = best_reinsert_edge(treeinfo,
                                                     &spr_entry,
                                                     params,
                                                     bl_min,
                                                     bl_max,
                                                     smoothings,
                                                     epsilon);

      /* original placement is the best for the current node -> move on to the next one */
      if (!best_r_edge || best_r_edge == p_edge || best_r_edge == p_edge->back || best_r_edge->back == p_edge)
        {
          continue;
        }

      /* LH improved -> re-apply the SPR move */
      if (spr_entry.lh - best_lh > 1e-6)
        {
          double regraft_length = best_r_edge->length;

          /* re-apply best SPR move for the node */
          int retval = pllmod_utree_spr(p_edge, best_r_edge, rollback);
          if (!retval)
            printf("ERROR: %d : %s\n", pll_errno, pll_errmsg);
          assert(retval == PLL_SUCCESS);

          /* increment rollback slot counter to save SPR history */
          rollback = rollback_list_next(rollback_list);

          if (params->thorough)
            {
              /* restore optimized branch length */
              pllmod_utree_set_length(p_edge, spr_entry.b1);
              pllmod_utree_set_length(p_edge->next, spr_entry.b2);
              pllmod_utree_set_length(p_edge->next->next, spr_entry.b3);
            }
          else
            {
              assert(p_edge->next->next->length == regraft_length / 2.);
              assert(p_edge->next->length == regraft_length / 2.);
            }

          assert(spr_entry.lh > best_lh);

          best_lh = spr_entry.lh;
        }
      else if (best_r_edge)
        {
          /* LH didn't improve but could be still high enough to be in top-20 */
          spr_entry.rollback_slot = rollback_list->current;
          bestnode_list_save(best_node_list, &spr_entry);
          loglh = spr_entry.lh;
        }

#ifdef _ULTRACHECK
      double tmp_logh = pllmod_treeinfo_compute_loglh(treeinfo, false);
      if(fabs(tmp_logh - best_lh) > 10e-6)
        {
          printf("%f %f\n", tmp_logh, best_lh);
          assert(0);
        }
#endif

      DBG("LogLikelihood after SPRs for node %lu (idx %d, clv %d): %f, best LH: %f\n",
                 i, p_edge->node_index, p_edge->clv_index, loglh, best_lh);
    }

  return loglh;
}

double spr_round(pllmod_treeinfo_t * treeinfo,
                 int radius_min,
                 int radius_max,
                 int n_topologies,
                 int thorough,
                 double bl_min,
                 double bl_max,
                 int smoothings,
                 double epsilon)
{
  /* process search params */
  pllmod_search_params_t params;
  params.thorough = thorough; //user_params ? user_params->thorough : false;
  params.ntopol_keep = n_topologies; //(user_params && user_params->ntopol_keep > 0) ? user_params->ntopol_keep : 20;
  params.radius_min = radius_min; // user_params ? user_params->radius_min : 0;
  params.radius_max = radius_max; //(user_params && user_params->radius_max > 0) ? user_params->radius_max : 10;

  /* allocate rollback_info slots */
  mt_size_t rollback_slots = params.ntopol_keep;
  pllmod_rollback_list_t * rollback_list = rollback_list_create(rollback_slots);

  /* allocate best node slots */
  mt_size_t toplist_slots = params.thorough ? params.ntopol_keep : params.ntopol_keep * 3;
  pllmod_bestnode_list_t * bestnode_list = bestnode_list_create(toplist_slots);

  mt_index_t i;

  double loglh   = pllmod_treeinfo_compute_loglh(treeinfo, false);
  double best_lh = loglh;

  int retval;

  /* query all nodes */
  int allnodes_count = (treeinfo->tip_count - 2) * 3;
  pll_utree_t ** allnodes = (pll_utree_t **) calloc ((mt_size_t) allnodes_count, sizeof(pll_utree_t *));
  assert(query_allnodes(treeinfo->root, allnodes) == allnodes_count);

  loglh = reinsert_nodes(treeinfo, allnodes, allnodes_count, rollback_list, bestnode_list, &params,
                         bl_min, bl_max, smoothings, epsilon);

  /* in FAST mode, we re-insert a subset of best-scoring subtrees with BLO (i.e., in SLOW mode)*/
  if (!params.thorough)
    {
      params.thorough = true;
      for (i = 0; bestnode_list->list[i].p_node != NULL; i++)
        {
          allnodes[i] = bestnode_list->list[i].p_node;
          bestnode_list->list[i].p_node = NULL;
        }

      DBG("\nThorough re-insertion of %d best-scoring nodes...\n", i);

      loglh = reinsert_nodes(treeinfo, allnodes, i, rollback_list, bestnode_list, &params,
                             bl_min, bl_max, smoothings, epsilon);
    }

  free(allnodes);

  best_lh = optimize_bl_iterative(treeinfo,
                                  bl_min,
                                  bl_max,
                                  smoothings,
                                  epsilon);

  DBG("Best tree LH after BLO: %f\n", best_lh);

  pll_utree_t * best_tree = pll_utree_clone(treeinfo->root);

  /* Restore best topologies and re-evaluate them after full BLO.
     NOTE: some SPRs were applied (if they improved LH) and others weren't.
     Therefore in order to restore the original topology, we need to either rollback SPR
     (if it was already applied), or re-do it again (if it wasn't applied).
     We perform it by simultaneously iterating over the history of applied SPRs (rollback_list) and
     over the list of not-applied SPRs which resulted in topologies with the highest LH (bestnode_list).
     */
  mt_size_t rollback_counter = 0;
  int toplist_index = -1;
  pll_tree_rollback_t * rollback2 = (pll_tree_rollback_t *) calloc(1, sizeof(pll_tree_rollback_t));
  bool undo_SPR = false;

#ifndef  PLLMOD_SEARCH_GREEDY_BLO
  pll_utree_t * tmp_tree = NULL;
#endif

  while (rollback_counter < rollback_list->size)
    {
      toplist_index = bestnode_list_next_index(bestnode_list, rollback_list->current, toplist_index);

      if (toplist_index == -1)
        {
          /* no more topologies for this rollback, so we go one slot back */
          pll_tree_rollback_t * rollback = rollback_list_prev(rollback_list);

          if (!rollback->SPR.prune_edge)
            {
              DBG("  Rollback slot %d is empty, exiting the loop...\n", rollback_list->current);
              break;
            }

          DBG("  Undoing SPR %d (slot %d)... ", rollback_counter, rollback_list->current);

          retval = pllmod_tree_rollback(rollback);
          assert(retval == PLL_SUCCESS);

          rollback_counter++;

          undo_SPR = false;
        }
      else
        {
          if (toplist_index > params.ntopol_keep)
            continue;

          node_entry_t * spr_entry = &bestnode_list->list[toplist_index];
          pll_utree_t * p_edge = spr_entry->p_node;
          pll_utree_t * r_edge = spr_entry->r_node;

          if (!p_edge)
            {
              DBG("    SPR slot %d is empty, exiting the loop...\n", toplist_index);
              break;
            }
          else
            DBG("    Evaluating topology %d (idx %d, clv %d -> idx %d, clv %d), old LH: %f... ",
                       toplist_index, p_edge->node_index, p_edge->clv_index,
                       r_edge->node_index, r_edge->clv_index, spr_entry->lh);

          /* re-apply best SPR move for the node */
          retval = pllmod_utree_spr(p_edge, r_edge, rollback2);
          if (!retval)
          {
            printf("MOVE %d-%d to %d-%d\n", p_edge->clv_index, p_edge->back->clv_index, r_edge->clv_index, r_edge->back->clv_index);
            printf("ERROR: %d : %s\n", pll_errno, pll_errmsg);
          }
          assert(retval == PLL_SUCCESS || pll_errno == PLL_ERROR_SPR_NOCHANGE);

#ifndef  PLLMOD_SEARCH_GREEDY_BLO
          /* clone the tree before BLO to save original branch length */
          // TODO: make it more efficient
          tmp_tree = treeinfo->root;
          treeinfo->root = pll_utree_clone(treeinfo->root);
#endif

          /* restore optimized branch lengths */
          pllmod_utree_set_length(p_edge, spr_entry->b1);
          pllmod_utree_set_length(p_edge->next, spr_entry->b2);
          pllmod_utree_set_length(p_edge->next->next, spr_entry->b3);

          undo_SPR = true;
        }

      /* now optimize all the branches */
      double loglh;
      loglh = optimize_bl_iterative(treeinfo, bl_min, bl_max, smoothings, epsilon);

      DBG("  new LH after BLO: %f\n", loglh);
      assert(loglh > -INFINITY);

      if (loglh - best_lh > 0.01)
	{
          DBG("Best tree LH: %f\n", loglh);

          if (best_tree)
            pll_utree_destroy(best_tree);

	  best_tree = pll_utree_clone(treeinfo->root);
	  best_lh = loglh;
	}

      if (undo_SPR)
        {
#ifndef  PLLMOD_SEARCH_GREEDY_BLO
          /* restore original brlens */
          pll_utree_destroy(treeinfo->root);
          treeinfo->root = tmp_tree;
#endif

          /* rollback the SPR */
          retval = pllmod_tree_rollback(rollback2);
          assert(retval == PLL_SUCCESS || pll_errno == PLL_ERROR_SPR_NOCHANGE);
        }
    }


  if (best_tree)
    {
      pll_utree_destroy(treeinfo->root);
      treeinfo->root = best_tree;
    }

  free(rollback2);

  bestnode_list_destroy(bestnode_list);
  rollback_list_destroy(rollback_list);

  return best_lh;
}

double optimize_branch_lengths(pllmod_treeinfo_t * treeinfo,
                               double bl_min,
                               double bl_max,
                               int smoothings,
                               double lh_epsilon)
{
  double loglh;
  loglh = -pllmod_opt_optimize_branch_lengths_iterative(treeinfo->partitions[0], treeinfo->root,
                                                        treeinfo->param_indices[0],
                                                        bl_min, bl_max,
                                                        lh_epsilon,
                                                        smoothings,
                                                        1);
  loglh = pllmod_treeinfo_compute_loglh(treeinfo, false);

  return loglh;
}

void cmd_search(pllmod_treeinfo_t * treeinfo,
                int params_to_optimize,
                double bl_min,
                double bl_max,
                int smoothings,
                double lh_epsilon)
{
  // const char result_fname[] = "result.tre";
  const double fast_modopt_eps = 10.;
  const double interim_modopt_eps = 3.;

  double loglh;

  /* Compute initial LH of the starting tree */
  loglh = pllmod_treeinfo_compute_loglh(treeinfo, false);

  //print_info("\nInitial LogLikelihood: %f\n", loglh);

  //!!!! loglh = optimize_tree(treeinfo, PLLMOD_OPT_PARAM_BRANCHES_ITERATIVE, fast_modopt_eps, 1);
  loglh = optimize_branch_lengths(treeinfo,
                                  bl_min,
                                  bl_max,
                                  smoothings,
                                  fast_modopt_eps);

  printf("LH after intial BLO: %f\n", loglh);

  //print_info("\nOptimize model parameters (fast), eps = %f\n", fast_modopt_eps);

  /* Initial fast model optimization */
  //!!!! loglh = optimize_tree(treeinfo, params_to_optimize, fast_modopt_eps, 1);

  //print_info("\nLogLikelihood after preliminary model optimization (eps = %.1f): %f\n", fast_modopt_eps, loglh);

  //!!!! loglh = optimize_tree(treeinfo, params_to_optimize, interim_modopt_eps, 1);

  //print_info("\nLogLikelihood after preliminary model optimization (eps = %.1f): %f\n", interim_modopt_eps, loglh);

  // do SPRs
  const int radius_limit = (treeinfo->tip_count>25)?22:(treeinfo->tip_count-3);
  const int radius_step = 5;

  treeinfo->counter = 0;

  /* initialize search params */
  pllmod_search_params_t spr_params;
  spr_params.thorough = false;
  spr_params.radius_min = 1;
  spr_params.radius_max = 10;
  spr_params.ntopol_keep = 20;

  double old_loglh, new_loglh = loglh;
  int round = 0;
  do
    {
      ++round;
      old_loglh = new_loglh;
      //print_info("\nStart %s round %d (radius: %d)\n\n",
      //           spr_params.thorough ? "SLOW" : "FAST", round, spr_params.radius_max);
      new_loglh = spr_round(treeinfo,
                            spr_params.radius_min,
                            spr_params.radius_max,
                            spr_params.ntopol_keep,
                            spr_params.thorough,
                            bl_min,
                            bl_max,
                            smoothings,
                            lh_epsilon);

      /* optimize ALL branches */
      new_loglh = optimize_branch_lengths(treeinfo,
                                          bl_min,
                                          bl_max,
                                          smoothings,
                                          lh_epsilon);

      printf("\nold/new LogLikelihood after %s round %d: %f %f\n\n",
                  spr_params.thorough ? "SLOW" : "FAST", round, old_loglh, new_loglh);

      // save_utree_to_file(treeinfo->root, result_fname);

      bool impr = (new_loglh - old_loglh > lh_epsilon);
      if (impr && spr_params.thorough)
        {
          spr_params.radius_min = 1;
          spr_params.radius_max = radius_step;
        }
      else if (!impr)
	{
          if (!spr_params.thorough)
	    {
              spr_params.thorough = true;
	      round = 0;
	      spr_params.radius_min = 1;
	      spr_params.radius_max = radius_step;

	      //print_info("\nOptimize model parameters (interm), eps = %f\n", 1.0);
	      //!!!! new_loglh = optimize_tree(treeinfo, params_to_optimize, 1.0, 1);
	      //print_info("LogLikelihood after FAST iterations: %f\n", new_loglh);
	    }
	  else
	    {
	      spr_params.radius_min += radius_step;
	      spr_params.radius_max += radius_step;
	    }
	}
    }
  while (spr_params.radius_min >= 0 && spr_params.radius_min < radius_limit);

  loglh = new_loglh;

  /* Final thorough model optimization */
  //print_info("\nOptimize model parameters (thorough), eps = %f\n", lh_epsilon);
  //!!!! loglh = optimize_tree(treeinfo, params_to_optimize, lh_epsilon, 1);

  //print_info("\nFinal LogLikelihood: %f\n", loglh);

  // print_model_params(treeinfo, useropt);

  // save_utree_to_file(treeinfo->root, result_fname);
  //print_info("\nFinal tree saved to: %s\n", result_fname);


  //print_info("\nTotal CLV updates: %d\n", treeinfo->counter);

  /* destroy treeinfo structure with all its guts */
  // pllmod_treeinfo_destroy(treeinfo);
}
