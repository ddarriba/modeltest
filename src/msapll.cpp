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
    long seqlen;
    long hdrlen;
    long seqno;

    /* MSA should be always checked beforehand */
    /* Therefore it should never fail here */
    assert(MsaPll::test(msa_filename, &n_sequences, &n_sites));

    pll_fasta_t * fp = pll_fasta_open (msa_filename.c_str (), pll_map_fasta);

    tipnames  = (char **)Utils::c_allocate(n_sequences, sizeof(char *));
    sequences = (char **)Utils::c_allocate(n_sequences, sizeof(char *));

    for (size_t cur_seq = 0; pll_fasta_getnext(fp,&hdr,&hdrlen,&seq,&seqlen,&seqno); ++cur_seq)
    {
        for (size_t i=(strlen(hdr)-1); i>0 && hdr[i] == ' '; i--)
           hdr[i] = '\0';
        tipnames[cur_seq]  = hdr;
        sequences[cur_seq] = seq;
    }

    pll_fasta_close(fp);
  }

  MsaPll::~MsaPll ()
  {
    for (mt_index_t i = 0; i < n_sequences; i++)
      free (sequences[i]);
    free (sequences);

    for (mt_index_t i = 0; i < n_sequences; i++)
      free (tipnames[i]);
    free (tipnames);
  }

  const char * MsaPll::get_header (mt_index_t index) const
  {
    assert(index < n_sequences);
    return tipnames[index];
  }

  const char * MsaPll::get_sequence (mt_index_t index) const
  {
    assert(index < n_sequences);
    return sequences[index];
  }

  bool MsaPll::test(string const& msa_filename,
               mt_size_t *n_tips,
               mt_size_t *n_sites)
  {
      mt_index_t cur_seq;
      char *hdr = NULL;
      char *seq = NULL;
      long seqlen;
      long hdrlen;
      long seqno;

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
           pll_fasta_getnext (fp, &hdr, &hdrlen, &seq, &seqlen, &seqno); ++cur_seq)
      {
          free (seq);
          free (hdr);

          assert(seqlen < MT_SIZE_UNDEF);

          if (seqlen < 0)
              return false;

          /* if parsing fail, we continue for avoid memory leaks */
          if (sites != MT_SIZE_UNDEF && (long)sites != seqlen)
          {
              errno = MT_ERROR_ALIGNMENT;
              break;
          }
          else if (sites == MT_SIZE_UNDEF)
              sites = (mt_size_t) seqlen;
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
                         mt_index_t n_sequences,
                         vector<mt_index_t> &r,
                         mt_index_t p1,
                         mt_index_t p2)
  {
      for (mt_index_t i=0; i<n_sequences; i++)
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
                      swap_sites(sequences, n_sequences, reindex, cur, k);
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

  void MsaPll::print() const
  {
      for (mt_index_t i=0; i<n_sequences; i++)
          cout << setw(20) << tipnames[i] << " " << sequences[i] << endl;

  }
} /* namespace modeltest */
