/*
 * msapll.cpp
 *
 *  Created on: Jun 9, 2015
 *      Author: diego
 */

#include "msapll.h"
#include "utils.h"

#include <pll_msa.h>
#include <cerrno>
#include <cassert>
#include <vector>
#include <iomanip>

using namespace std;

namespace modeltest
{

  Msa::~Msa (){}

  MsaPll::MsaPll (string msa_filename)
      : Msa(msa_filename)
  {
    char *hdr = NULL;
    char *seq = NULL;
    long n_sites_read;
    long hdr_len;
    long seq_idx;

    /* MSA should be always checked beforehand */
    /* Therefore it should never fail here */
    assert(MsaPll::test(msa_filename, &n_taxa, &n_sites));

    pll_fasta_t * fp = pll_fasta_open (msa_filename.c_str (), pll_map_fasta);

    tipnames  = (char **)Utils::c_allocate(n_taxa, sizeof(char *));
    sequences = (char **)Utils::c_allocate(n_taxa, sizeof(char *));

    for (size_t cur_seq = 0; pll_fasta_getnext(fp,&hdr,&hdr_len,&seq,&n_sites_read,&seq_idx); ++cur_seq)
    {
        assert (n_sites_read == n_sites);
        for (size_t i=(strlen(hdr)-1); i>0 && hdr[i] == ' '; i--)
           hdr[i] = '\0';
        tipnames[cur_seq]  = hdr;
        sequences[cur_seq] = seq;
    }

    pll_fasta_close(fp);
    weights = 0;

    /* reset pll errno */
    pll_errno = 0;
  }

  MsaPll::~MsaPll ()
  {
    for (mt_index_t i = 0; i < n_taxa; i++)
      free (sequences[i]);
    free (sequences);

    for (mt_index_t i = 0; i < n_taxa; i++)
      free (tipnames[i]);
    free (tipnames);

    if (weights)
        free(weights);

    for (pll_partition_t *pll_part : pll_partitions)
        if (pll_part)
            pll_partition_destroy(pll_part);
  }

  const char * MsaPll::get_header (mt_index_t index) const
  {
    assert(index < n_taxa);
    return tipnames[index];
  }

  const char * const* MsaPll::get_headers() const
  {
      return tipnames;
  }

  const char * MsaPll::get_sequence (mt_index_t index) const
  {
    assert(index < n_taxa);
    return sequences[index];
  }

  const unsigned int * MsaPll::get_weights( void ) const
  {
      return weights;
  }

  const char aa_unique_chars[14] = {
    'e','f','i','l','p','q','z',
    'E','F','I','L','P','Q','Z'
  };

  bool MsaPll::test(string const& msa_filename,
               mt_size_t *n_tips,
               mt_size_t *n_sites,
               data_type_t *datatype)
  {
      mt_index_t cur_seq;
      char *hdr = NULL;
      char *seq = NULL;
      long n_sites_read;
      long hdr_len;
      long seq_idx;
      bool dt_unknown = true;

      /* reset error */
      mt_errno = 0;

      if (!modeltest::Utils::file_exists(msa_filename))
      {
          mt_errno = MT_ERROR_IO;
          snprintf(mt_errmsg, ERR_MSG_SIZE, "input file does not exist");
          return false;
      }

      pll_fasta_t * fp = pll_fasta_open (msa_filename.c_str (), pll_map_fasta);

      if (!fp)
      {
          mt_errno = pll_errno;
          strncpy(mt_errmsg, pll_errmsg, ERR_MSG_SIZE);
          return false;
      }

      if (datatype)
          *datatype = dt_dna;

      /* read FASTA sequences for finding the number of tips and seq len */
      /* make sure they are all of the same length */
      mt_size_t sites = MT_SIZE_UNDEF;
      for (cur_seq = 0;
           pll_fasta_getnext (fp, &hdr, &hdr_len, &seq, &n_sites_read, &seq_idx); ++cur_seq)
      {
          if (datatype && dt_unknown)
          {
              for (char c : aa_unique_chars)
              {
                  if (strchr(seq, c))
                  {
                      dt_unknown = false;
                      *datatype = dt_protein;
                  }
              }
          }

          assert(n_sites_read < MT_SIZE_UNDEF);

          if (n_sites_read < 0)
          {
              mt_errno = MT_ERROR_ALIGNMENT;
              snprintf(mt_errmsg, ERR_MSG_SIZE, "Wrong sites read");
              break;
          }

          /* if parsing fail, we continue for avoiding memory leaks */
          if (sites != MT_SIZE_UNDEF && (long)sites != n_sites_read)
          {
              mt_errno = MT_ERROR_ALIGNMENT;
              snprintf(mt_errmsg, ERR_MSG_SIZE, "Wrong sites read [Seq %d] %d vs %ld", cur_seq, sites, n_sites_read);
              break;
          }
          else if (sites == MT_SIZE_UNDEF)
              sites = (mt_size_t) n_sites_read;
          free (seq);
          free (hdr);
      }

      if (sites == MT_SIZE_UNDEF)
      {
          switch (pll_errno)
          {
          case PLL_ERROR_FASTA_ILLEGALCHAR:
              mt_errno = MT_ERROR_ALIGNMENT_ILLEGAL;
              break;
          case PLL_ERROR_FASTA_INVALIDHEADER:
              mt_errno = MT_ERROR_ALIGNMENT_HEADER;
              break;
          case PLL_ERROR_FASTA_UNPRINTABLECHAR:
              mt_errno = MT_ERROR_ALIGNMENT_UNPRINTABLECHAR;
              break;
          default:
              assert(0);
          }

          mt_errno = pll_errno;
          strncpy(mt_errmsg, pll_errmsg, ERR_MSG_SIZE);
      }

      pll_fasta_close (fp);

      if (mt_errno)
      {
          *n_tips = 0;
          *n_sites = 0;
          return false;
      }
      else
      {
          *n_tips = cur_seq;
          *n_sites = sites;
      }

      return true;
  }

  static void swap_sites(char **s,
                         mt_index_t n_taxa,
                         vector<mt_index_t> &r,
                         mt_index_t p1,
                         mt_index_t p2)
  {
      for (mt_index_t i=0; i<n_taxa; i++)
      {
        char s_aux = s[i][p1];
        s[i][p1] = s[i][p2];
        s[i][p2] = s_aux;
      }
      mt_index_t p = r[p1];
      r[p1] = r[p2];
      r[p2] = p;
  }


  bool MsaPll::reorder_sites(partitioning_scheme_t & scheme)
  {
      /* sort partitioning scheme */
      Utils::sort_partitioning_scheme(scheme);

      /* initialize reindex vector */
      vector<mt_index_t> reindex(n_sites);
      for (mt_index_t i=0; i<n_sites; i++)
          reindex[i] = i;

      vector<unsigned int *> partition_weights;
      char ** partition_sequences = (char **) malloc(n_taxa * sizeof(char *));
      partition_weights.reserve(scheme.size());
      mt_index_t cur = 0;
      for (partition_descriptor_t & partition : scheme)
      {
          partition_region_t new_region;
          new_region.start = cur+1;

          /* merge all regions into a single one */
          for (partition_region_t & region : partition.regions)
          {
              for (mt_index_t j=(region.start-1); j<region.end; j+=region.stride)
              {
                  if (reindex[cur] != j)
                  {
                      mt_index_t k = j;
                      while(reindex[k] != j) k = reindex[k];

                      /* swap */
                      swap_sites(sequences, n_taxa, reindex, cur, k);
                  }
                  cur++;
              }
          }

          /* compress patterns */
          for (mt_index_t i=0; i<n_taxa; ++i)
            partition_sequences[i] = sequences[i] + new_region.start - 1;

//          for (mt_index_t i=0; i<n_taxa; ++i)
//          {
//              cout << "t" << i << " : ";
//              for (mt_index_t j=0; j<cur; ++j)
//                  cout << partition_sequences[i][j];
//              cout << endl;
//          }

          int compressed_length = cur;
          unsigned int * pw = pll_compress_site_patterns(partition_sequences, n_taxa, &compressed_length);
          cur = new_region.start + compressed_length - 1;
          partition_weights.push_back(pw);
          cout << "SITES = " << cur << ", PATTERNS = " << compressed_length << endl;


          new_region.end = cur;
          new_region.stride = 1;
          partition.regions.clear();
          partition.regions.push_back(new_region);
      }

      assert (!weights);
      weights = (unsigned int *) malloc (cur * sizeof(unsigned int));
      unsigned int * weights_ptr = weights;
      for (mt_index_t i=0; i<scheme.size(); ++i)
      {
          memcpy(weights_ptr, partition_weights[i], sizeof(unsigned int) * (scheme[i].regions[0].end - scheme[i].regions[0].start + 1));
          free(partition_weights[i]);
      }
      free(partition_sequences);
    return true;
  }

  bool MsaPll::compute_empirical_frequencies(partition_descriptor_t & partition,
                                             bool smooth,
                                             bool force_recompute)
  {
      mt_size_t states = partition.states;
      assert (states);

      if (partition.empirical_freqs.size())
          if(!force_recompute || partition.empirical_freqs.size() != states)
              return false;

      partition.empirical_freqs.resize(states);
      for (mt_index_t i=0; i<states; i++)
          partition.empirical_freqs[i] = 0;

      mt_size_t cum_weights = 0;
      mt_size_t cum_abs_freq = 0;
      for (partition_region_t region : partition.regions)
          for (mt_index_t j = region.start - 1; j < region.end; j++)
              cum_weights += weights[j];

      uint32_t existing_states = 0;
      const unsigned int * states_map = (partition.datatype == dt_dna)?pll_map_nt:pll_map_aa;
      for (mt_index_t i=0; i<n_taxa; ++i)
      {
          for (partition_region_t region : partition.regions)
          {
              for (mt_index_t j = region.start - 1; j < region.end; j++)
              {
                  mt_size_t sum_site = 0;
                  mt_size_t ind = states_map[(int)sequences[i][j]];
                  if (!ind)
                  {
                      mt_errno = MT_ERROR_FREQUENCIES;
                      snprintf(mt_errmsg, ERR_MSG_SIZE, "MSA does not match the data type [%s]", partition.partition_name.c_str());
                      return false;
                  }
                  for (unsigned int k=0; k<states; ++k)
                  {
                      sum_site += ((ind & (1<<k)) > 0);
                  }
                  for (unsigned int k=0; k<states; ++k)
                      partition.empirical_freqs[k] += 1.0 * weights[j] * ((ind & (1<<k))>0) / sum_site;
                  if (sum_site == 1)
                      existing_states |= ind;
              }
          }
      }

      for (mt_index_t i=0; i<states; i++)
      {
          cum_abs_freq += partition.empirical_freqs[i];
          partition.empirical_freqs[i] /= n_taxa * cum_weights;
      }

      /* validate */
      double checksum = 0.0;
      for (mt_index_t i=0; i<states; i++)
      {
          checksum += partition.empirical_freqs[i];
      }

      if( (checksum != checksum) || (fabs(1-checksum) > 1e-10 ))
      {
          mt_errno = MT_ERROR_FREQUENCIES;
          snprintf(mt_errmsg, ERR_MSG_SIZE, "Empirical frequencies sum to %.4f instead of 1 [%s]", checksum, partition.partition_name.c_str());
          return false;
      }

      /* check missing states */
      mt_size_t missing_states = states - Utils::count_bits(existing_states);
      if (missing_states)
      {
          if (smooth)
          {
              std::cerr << "WARNING: Forced freq. smoothing" << std::endl;
              for (mt_index_t i=0; i<states; i++)
                  partition.empirical_freqs[i] /= checksum + MT_MIN_SMOOTH_FREQ * missing_states;
          }
          else
          {
              mt_errno = MT_ERROR_FREQUENCIES;
              snprintf(mt_errmsg, ERR_MSG_SIZE, "There are [%d] missing states [%s]", missing_states, partition.partition_name.c_str());
              return false;
          }
      }

      mt_errno = 0;
      return true;
  }

  bool MsaPll::compute_empirical_subst_rates(partition_descriptor_t &partition,
                                             bool force_recompute)
  {
      unsigned int i, j, k, n;
      mt_size_t states = partition.states;
      assert (states);

      if (partition.empirical_subst_rates.size())
          if(!force_recompute || partition.empirical_subst_rates.size() != states)
              return false;

        unsigned int n_subst_rates  = (states * (states - 1) / 2);
        const unsigned int * states_map = (partition.datatype == dt_dna)?pll_map_nt:pll_map_aa;

        partition.empirical_subst_rates.resize(n_subst_rates);
        for (mt_index_t i=0; i<states; i++)
            partition.empirical_subst_rates[i] = 0;

        unsigned *pair_rates = (unsigned *) calloc(
            states * states, sizeof(unsigned));
        unsigned *state_freq = (unsigned *) malloc(states * sizeof(unsigned));

        if (!(pair_rates && state_freq))
        {
          pll_errno = PLL_ERROR_MEM_ALLOC;
          snprintf (pll_errmsg, 200,
                    "Cannot allocate memory for empirical subst rates");
          if (pair_rates)
            free (pair_rates);
          if (state_freq)
            free (state_freq);
          return NULL;
        }

        mt_size_t undef_state = (unsigned int) (1<<states) - 1;
          for (n = 0; n < n_sites; ++n)
          {
            memset (state_freq, 0, sizeof(unsigned) * (states));
            for (i = 0; i < n_taxa; ++i)
            {
              mt_index_t state = states_map[(int)sequences[i][n]];
              if (state == undef_state)
                continue;
              for (k = 0; k < states; ++k)
              {
                if (state & 1)
                  state_freq[k]++;
                state >>= 1;
              }
            }

            for (i = 0; i < states; i++)
            {
              if (state_freq[i] == 0)
                continue;
              for (j = i + 1; j < states; j++)
              {
                pair_rates[i * states + j] += state_freq[i] * state_freq[j]
                    * weights[n];
              }
            }
          }

        k = 0;
        double last_rate = pair_rates[(states - 2) * states + states - 1];
        if (last_rate < 1e-7)
          last_rate = 1;
        for (i = 0; i < states - 1; i++)
        {
          for (j = i + 1; j < states; j++)
          {
            partition.empirical_subst_rates[k++] = pair_rates[i * states + j] / last_rate;
            if (partition.empirical_subst_rates[k - 1] < 0.01)
              partition.empirical_subst_rates[k - 1] = 0.01;
            if (partition.empirical_subst_rates[k - 1] > 50.0)
              partition.empirical_subst_rates[k - 1] = 50.0;
          }
        }

        partition.empirical_subst_rates[k - 1] = 1.0;

        free(pair_rates);
        free(state_freq);

    return true;
  }

  bool MsaPll::compute_empirical_pinv(partition_descriptor_t &partition)
  {
      mt_size_t n_inv = 0;
      double sum_wgt = 0.0;
      for (partition_region_t region : partition.regions)
      {
          for (mt_index_t j = region.start - 1; j < region.end; j++)
          {
              sum_wgt += weights[j];
              n_inv   += weights[j];
              char state = sequences[0][j];
              for (mt_index_t i=1; i<n_taxa; ++i)
              {
                  if (sequences[i][j] != state)
                  {
                      n_inv -= weights[j];
                      break;
                  }
              }
          }
      }
      double empirical_pinv = (double)1.0*n_inv/sum_wgt;
      partition.empirical_pinv = empirical_pinv;

      return true;
  }

  void MsaPll::print() const
  {
      for (mt_index_t i=0; i<n_taxa; i++)
          cout << setw(20) << tipnames[i] << " " << sequences[i] << endl;

  }

} /* namespace modeltest */
