/*
  Copyright (C) 2016 Diego Darriba

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

  Contact: Diego Darriba <Diego.Darriba@h-its.org>,
  Heidelberg Institute for Theoretical Studies,
  Schloss-Wolfsbrunnenweg 35, D-69118 Heidelberg, Germany
*/

#include "utils.h"
#include "msapll.h"
#include "plldefs.h"

#include <cerrno>
#include <vector>
#include <cassert>
#include <iomanip>

using namespace std;

namespace modeltest
{

  Msa::~Msa (){}

  MsaPll::MsaPll (string msa_filename, pll_msa_t * msa_data)
      : Msa(msa_filename, mf_phylip)
  {
    assert(msa_data);

    /* MSA should be always checked beforehand */
    n_sites = msa_data->length;
    n_patterns = n_sites;

    tipnames  = (char **)Utils::c_allocate(n_taxa, sizeof(char *));
    sequences = (char **)Utils::c_allocate(n_taxa, sizeof(char *));

    for (size_t cur_seq = 0; cur_seq < n_taxa; ++cur_seq)
    {
      tipnames[cur_seq] = msa_data->label[cur_seq];
      sequences[cur_seq] = msa_data->sequence[cur_seq];
    }
    weights = 0;

    /* reset pll errno */
    pll_errno = 0;
  }

  MsaPll::MsaPll (string msa_filename, mt_size_t _n_taxa)
      : Msa(msa_filename, mf_phylip)
  {
    char *hdr = NULL;
    char *seq = NULL;
    long n_sites_read = -1;
    long hdr_len;
    long seq_idx;

    n_taxa = _n_taxa;

    /* MSA should be always checked beforehand */
    tipnames  = (char **)Utils::c_allocate(n_taxa, sizeof(char *));
    sequences = (char **)Utils::c_allocate(n_taxa, sizeof(char *));

    pll_fasta_t * fp = pll_fasta_open (msa_filename.c_str(), pll_map_fasta);

    for (size_t cur_seq = 0; pll_fasta_getnext(fp,&hdr,&hdr_len,&seq,&n_sites_read,&seq_idx); ++cur_seq)
    {
      if (!cur_seq)
        n_sites = n_sites_read;

      assert (n_sites_read == n_sites);
      for (size_t i=(strlen(hdr)-1); i>0 && hdr[i] == ' '; i--)
         hdr[i] = '\0';
      tipnames[cur_seq]  = hdr;
      sequences[cur_seq] = seq;
    }
    pll_fasta_close(fp);

    n_patterns = n_sites;
    weights = 0;

    /* reset pll errno */
    pll_errno = 0;
  }

  MsaPll::MsaPll (string msa_filename, msa_format_t msa_format)
      : Msa(msa_filename, msa_format)
  {
    char *hdr = NULL;
    char *seq = NULL;
    long n_sites_read;
    long hdr_len;
    long seq_idx;

    /* MSA should be always checked beforehand */
    switch (msa_format)
    {
      case mf_phylip:
        {
          pll_msa_t * msa_data = pll_phylip_parse_msa(msa_filename.c_str(),
                                                      &n_taxa);
          assert(msa_data);
          n_sites = msa_data->length;
          tipnames  = (char **)Utils::c_allocate(n_taxa, sizeof(char *));
          sequences = (char **)Utils::c_allocate(n_taxa, sizeof(char *));

          for (size_t cur_seq = 0; cur_seq < n_taxa; ++cur_seq)
          {
            tipnames[cur_seq] = msa_data->label[cur_seq];
            sequences[cur_seq] = msa_data->sequence[cur_seq];
          }
          free(msa_data->label);
          free(msa_data->sequence);
          free(msa_data);
        }
        break;
      case mf_fasta:
        {
          bool test_msa = MsaPll::test(msa_filename, &n_taxa, &n_sites);
          assert(test_msa);

          tipnames  = (char **)Utils::c_allocate(n_taxa, sizeof(char *));
          sequences = (char **)Utils::c_allocate(n_taxa, sizeof(char *));

          pll_fasta_t * fp = pll_fasta_open (msa_filename.c_str(), pll_map_fasta);

        for (size_t cur_seq = 0; pll_fasta_getnext(fp,&hdr,&hdr_len,&seq,&n_sites_read,&seq_idx); ++cur_seq)
        {
            assert (n_sites_read == n_sites);
            for (size_t i=(strlen(hdr)-1); i>0 && hdr[i] == ' '; i--)
               hdr[i] = '\0';
            tipnames[cur_seq]  = hdr;
            sequences[cur_seq] = seq;
        }

        pll_fasta_close(fp);
        }
        break;
      default:
        printf("Error: Cannot recognise MSA file format\n");
        assert(0);
    }
    n_patterns = n_sites;
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

  msa_format_t Msa::guess_msa_format(string const& msa_filename)
  {
      FILE * msa_file;
      char first_char = ' ';
      msa_file = fopen(msa_filename.c_str(), "r");
      while(first_char == ' ')
          if (fread(&first_char, 1, 1, msa_file) != 1) first_char='\0';
      fclose(msa_file);

      if (first_char == '>')
      {
          return mf_fasta;
      }
      if (first_char >= '0' && first_char <= '9')
      {
          return mf_phylip;
      }
      return mf_undefined;
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
               mt_size_t *n_patterns,
               msa_format_t *msa_format,
               data_type_t *datatype)
  {
      msa_format_t format;
      mt_size_t sites = MT_SIZE_UNDEF;
      mt_index_t cur_seq = 0;

      char *hdr = NULL;
      char *seq = NULL;
      vector<char *> seqs;
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

      format = modeltest::Msa::guess_msa_format(msa_filename);

      if (format == mf_phylip)
      {
        pll_msa_t * msa_data = pll_phylip_parse_msa(
                                         msa_filename.c_str(),
                                         &cur_seq);
        if (!msa_data || !cur_seq)
        {
          mt_errno = MT_ERROR_IO_FORMAT;
          strncpy(mt_errmsg, pll_errmsg, ERR_MSG_SIZE);
        }
        else
        {
          sites = msa_data->length;
        }
        pll_msa_destroy(msa_data);
      }
      else if (format == mf_fasta)
      {
          pll_fasta_t * fp = pll_fasta_open (msa_filename.c_str (), pll_map_fasta);

          if (!fp)
          {
              mt_errno = pll_errno;
              strncpy(mt_errmsg, pll_errmsg, ERR_MSG_SIZE);
              return false;
          }

          if (datatype)
              *datatype = dt_dna;

          /* resize sequences vector to hardcoded soft limit */
          if (n_patterns)
              seqs.reserve(1000);

          /* read FASTA sequences for finding the number of tips and seq len */
          /* make sure they are all of the same length */
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

              if (n_patterns)
                seqs.push_back(seq);
              else
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

          if (n_patterns)
          {
            int i_n_patterns;
            const unsigned int * char_map = *datatype == dt_dna ?
                                              pll_map_nt :
                                              pll_map_aa;

            assert(seqs.size() == cur_seq);
            unsigned int * pw = pll_compress_site_patterns(&(seqs[0]),
                                                           char_map,
                                                           cur_seq,
                                                           &i_n_patterns);

            for (char * seq : seqs)
              free(seq);

            if (!pw)
              return false;

            *n_patterns = i_n_patterns;

            free(pw);
          }

          pll_fasta_close (fp);
      }
      else
      {
        mt_errno = MT_ERROR_IO_FORMAT;
        snprintf(mt_errmsg, ERR_MSG_SIZE, "Cannot detect MSA format");
        return false;
      }

      if (mt_errno)
      {
          *n_tips     = 0;
          *n_sites    = 0;
          if (msa_format)
            *msa_format = mf_undefined;
          return false;
      }
      else
      {
          *n_tips     = cur_seq;
          *n_sites    = sites;
          if (msa_format)
            *msa_format = format;
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


  bool MsaPll::reorder_sites(partitioning_scheme_t & scheme,
                             bool compress_patterns)
  {
      mt_size_t compressed_sum = 0;
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
          {
            partition_sequences[i] = sequences[i] + new_region.start - 1;
          }

          int compressed_length = cur - new_region.start + 1;
          const unsigned int * char_map = partition.datatype == dt_dna ? pll_map_nt : pll_map_aa;
          unsigned int * pw;
          if (compress_patterns)
          {
            pw = pll_compress_site_patterns(partition_sequences, char_map, n_taxa, &compressed_length);
          }
          else
          {
             pw = (unsigned int *) malloc(compressed_length * sizeof(unsigned int));
             for (mt_index_t i=0; i<(mt_size_t)compressed_length;++i) pw[i]=1;
          }

          compressed_sum += compressed_length;

          cur = new_region.start + compressed_length - 1;
          partition_weights.push_back(pw);
          n_patterns = compressed_length;

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
          weights_ptr += scheme[i].regions[0].end - scheme[i].regions[0].start + 1;
          free(partition_weights[i]);
      }
      free(partition_sequences);

      for (mt_index_t i=0; i<n_taxa; ++i)
      {
        sequences[i] = (char *)realloc(sequences[i], compressed_sum+1);

      }

    return true;
  }

  void MsaPll::print() const
  {
      for (mt_index_t i=0; i<n_taxa; i++)
          cout << setw(20) << tipnames[i] << " " << sequences[i] << endl;
  }

} /* namespace modeltest */
