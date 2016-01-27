/*
 * msapll.cpp
 *
 *  Created on: Jun 9, 2015
 *      Author: diego
 */

#include "msapll.h"
#include "utils.h"

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

    for (pll_partition_t *pll_part : pll_partitions)
        if (pll_part)
            pll_partition_destroy(pll_part);
  }

  const char * MsaPll::get_header (mt_index_t index) const
  {
    assert(index < n_taxa);
    return tipnames[index];
  }

  const char * MsaPll::get_sequence (mt_index_t index) const
  {
    assert(index < n_taxa);
    return sequences[index];
  }

  bool MsaPll::test(string const& msa_filename,
               mt_size_t *n_tips,
               mt_size_t *n_sites)
  {
      mt_index_t cur_seq;
      char *hdr = NULL;
      char *seq = NULL;
      long n_sites_read;
      long hdr_len;
      long seq_idx;

      /* reset error */
      errno = 0;

      pll_fasta_t * fp = pll_fasta_open (msa_filename.c_str (), pll_map_fasta);

      if (!fp)
      {
          errno = pll_errno;
          return false;
      }

      /* read FASTA sequences for finding the number of tips and seq len */
      /* make sure they are all of the same length */
      mt_size_t sites = MT_SIZE_UNDEF;
      for (cur_seq = 0;
           pll_fasta_getnext (fp, &hdr, &hdr_len, &seq, &n_sites_read, &seq_idx); ++cur_seq)
      {
          free (seq);
          free (hdr);

          assert(n_sites_read < MT_SIZE_UNDEF);

          if (n_sites_read < 0)
              return false;

          /* if parsing fail, we continue for avoid memory leaks */
          if (sites != MT_SIZE_UNDEF && (long)sites != n_sites_read)
          {
              errno = MT_ERROR_ALIGNMENT;
              break;
          }
          else if (sites == MT_SIZE_UNDEF)
              sites = (mt_size_t) n_sites_read;
      }

      if (sites == MT_SIZE_UNDEF)
      {
          errno = pll_errno;
      }

      pll_fasta_close (fp);

      if (errno)
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

      mt_index_t cur = 0;
      for (partition_t & partition : scheme)
      {
          partition_region_t new_region;
          new_region.start = cur+1;
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
          new_region.end = cur;
          new_region.stride = 1;
          partition.regions.clear();
          partition.regions.push_back(new_region);
      }

    return true;
  }

  bool MsaPll::compute_empirical_frequencies(partition_t & partition,
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

      mt_size_t cum_sites = 0;
      for (partition_region_t region : partition.regions)
          cum_sites += region.end - region.start;

      uint32_t existing_states = 0;
      const unsigned int * states_map = (partition.datatype == dt_dna)?pll_map_nt:pll_map_aa;
      for (mt_index_t i=0; i<n_taxa; ++i)
      {
          for (partition_region_t region : partition.regions)
          {
              for (mt_index_t j = region.start; j < region.end; j++)
              {
                  double sum_site = 0.0;
                  mt_size_t ind = states_map[(int)sequences[i][j]];
                  for (unsigned int k=0; k<states; ++k)
                  {
                      sum_site += (ind & (1<<k)) > 0;
                  }
                  for (unsigned int k=0; k<states; ++k)
                      partition.empirical_freqs[k] += 1.0 * ((ind & (1<<k))>0) / sum_site;
                  if (sum_site == 1)
                      existing_states |= ind;
              }
          }
      }

      for (mt_index_t i=0; i<states; i++)
      {
          partition.empirical_freqs[i] /= n_taxa * cum_sites;
      }

      /* validate */
      double checksum = 0.0;
      for (mt_index_t i=0; i<states; i++)
      {
          checksum += partition.empirical_freqs[i];
      }
      assert( fabs(1-checksum) < 1e-10 );

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
              std::cerr << "There are "<< missing_states << " missing states." << std::endl;
              return false;
          }
      }

      return true;
  }

  bool MsaPll::compute_empirical_pinv(partition_t &partition)
  {
      mt_size_t n_inv = 0;
      mt_size_t n_sites = 0;
      for (partition_region_t region : partition.regions)
      {
          n_sites += region.end - region.start;
          n_inv   += region.end - region.start;
          for (mt_index_t j = region.start; j < region.end; j++)
          {
              char state = sequences[0][j];
              for (mt_index_t i=0; i<n_taxa; ++i)
              {
                  if (sequences[i][j] != state)
                  {
                      n_inv--;
                      break;
                  }
              }
          }
      }
      double empirical_pinv = (double)1.0*n_inv/n_sites;
      partition.empirical_pinv = empirical_pinv;
      return true;
  }

  void MsaPll::print() const
  {
      for (mt_index_t i=0; i<n_taxa; i++)
          cout << setw(20) << tipnames[i] << " " << sequences[i] << endl;

  }

} /* namespace modeltest */
