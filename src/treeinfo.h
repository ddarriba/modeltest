/*
    Copyright (C) 2016 Alexey Kozlov

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

    Contact: Alexey Kozlov <Alexey.Kozlov@h-its.org>,
    Heidelberg Institute for Theoretical Studies,
    Schloss-Wolfsbrunnenweg 35, D-69118 Heidelberg, Germany
*/

#ifndef TREEINFO_H_
#define TREEINFO_H_

#include <cstdlib>
//#include <unistd.h>

#include "global_defs.h"
#include "plldefs.h"

/* TODO: move to libpll-modules */

#define PLLMOD_TREEINFO_PARTITION_ALL -1

typedef struct treeinfo
{
  // dimensions
  size_t tip_count;
  size_t partition_count;

  /* 0 = unlinked/per-partion branch lengths, 1 = linked/shared */
  int linked_branches;

  pll_utree_t * root;

  // partitions & partition-specific stuff
  pll_partition_t ** partitions;
  double * alphas;
  unsigned int ** param_indices;
  int ** subst_matrix_symmetries;
  double ** branch_lengths;

  // invalidation flags
  char ** clv_valid;
  char ** pmatrix_valid;

  // buffers
  pll_utree_t ** travbuffer;
  unsigned int * matrix_indices;
  pll_operation_t * operations;

  // partition on which all operations should be performed
  int active_partition;

  // general-purpose counter
  size_t counter;
} pllmod_treeinfo_t;

/* treeinfo management */
unsigned int pllmod_util_subst_rate_count(unsigned int states);

pllmod_treeinfo_t * pllmod_treeinfo_create(pll_utree_t * root,
                                           unsigned int tips, unsigned int partitions,
                                           int linked_branch_lengths);

int pllmod_treeinfo_init_partition(pllmod_treeinfo_t * treeinfo, unsigned int partition_index,
                                  pll_partition_t * partition, double alpha,
                                  const unsigned int * param_indices,
                                  const int * subst_matrix_symmetries);

int pllmod_treeinfo_set_active_partition(pllmod_treeinfo_t * treeinfo, int partition_index);

void pllmod_treeinfo_set_root(pllmod_treeinfo_t * treeinfo, pll_utree_t * root);

void pllmod_treeinfo_set_branch_length(pllmod_treeinfo_t * treeinfo, pll_utree_t * edge,
                                       double length);

void pllmod_treeinfo_destroy(pllmod_treeinfo_t * treeinfo);

int pllmod_treeinfo_update_prob_matrices(pllmod_treeinfo_t * treeinfo, int update_all);

void pllmod_treeinfo_invalidate_all(pllmod_treeinfo_t * treeinfo);

int pllmod_treeinfo_validate_clvs(pllmod_treeinfo_t * treeinfo, pll_utree_t ** travbuffer,
                                  unsigned int travbuffer_size);

void pllmod_treeinfo_invalidate_pmatrix(pllmod_treeinfo_t * treeinfo, const pll_utree_t * edge);

void pllmod_treeinfo_invalidate_clv(pllmod_treeinfo_t * treeinfo, const pll_utree_t * edge);

double pllmod_treeinfo_compute_loglh(pllmod_treeinfo_t * treeinfo, bool incremental);

double spr_round(pllmod_treeinfo_t * treeinfo,
                 int radius_min,
                 int radius_max,
                 int n_topologies,
                 int thorough,
                 double bl_min,
                 double bl_max,
                 int smoothings,
                 double epsilon);

void cmd_search(pllmod_treeinfo_t * treeinfo,
                int params_to_optimize,
                double bl_min,
                double bl_max,
                int smoothings,
                double lh_epsilon);

#endif /* TREEINFO_H_ */
