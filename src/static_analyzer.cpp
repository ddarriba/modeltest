#include <iostream>

#include "msapll.h"
#include "utils.h"
#include "plldefs.h"
#include "static_analyzer.h"
#include "model_defs.h"

using namespace std;

namespace modeltest
{

void StaticAnalyzer::print_info(const string msa_filename, msa_format_t format)
{
  long n_sites_read;
  mt_size_t n_taxa, n_sites;
  int  n_patterns;
  long hdr_len;
  long seq_idx;
  char ** sequences;
  mt_size_t invar_count   = 0;
  mt_size_t missing_count = 0;
  mt_size_t indel_count   = 0;
  long site_taxa_count    = 0;

  if (format == mf_fasta)
  {
    bool test_msa = MsaPll::test(msa_filename, &n_taxa, &n_sites);
    if (!test_msa)
    {
      cerr << mt_errno << ": " << mt_errmsg << endl;
    }
    assert(test_msa);

    sequences = (char **)Utils::c_allocate(n_taxa, sizeof(char *));

    pll_fasta_t * fp = pll_fasta_open (msa_filename.c_str(), pll_map_fasta);

    char * hdr;
    char * seq;
    for (size_t cur_seq = 0; pll_fasta_getnext(fp,&hdr,&hdr_len,&seq,&n_sites_read,&seq_idx); ++cur_seq)
    {
        assert (n_sites_read == n_sites);
        for (size_t i=(strlen(hdr)-1); i>0 && hdr[i] == ' '; i--)
           hdr[i] = '\0';
        sequences[cur_seq] = seq;
    }

    pll_fasta_close(fp);
  }
  else
  {
    cout << ". . . parsing PHYLIP file" << endl;
    pll_msa_t * msa_data = pll_phylip_parse_msa(msa_filename.c_str(),
                                                &n_taxa);
    assert(msa_data);
    n_sites = msa_data->length;
    cout << ". . . size: " << n_taxa << " x " << n_sites << endl;
    sequences = (char **)Utils::c_allocate(n_taxa, sizeof(char *));

    cout << ". . . placing sequences" << endl;
    for (size_t cur_seq = 0; cur_seq < n_taxa; ++cur_seq)
    {
      free(msa_data->label[cur_seq]);
      sequences[cur_seq] = msa_data->sequence[cur_seq];
    }
    cout << ". . . done initialization" << endl;
    free(msa_data->label);
    free(msa_data->sequence);
    free(msa_data);
  }

  n_patterns = n_sites;
  unsigned int * w_vector = pll_compress_site_patterns(sequences,
                                                       extended_dna_map,
                                                       n_taxa,
                                                       &n_patterns);
  char cur_state = '\0';
  for (mt_index_t i = 0; i<(mt_size_t)n_patterns; ++i)
  {
    if (sequences[0][i] != cur_state)
    {
      cur_state = sequences[0][i];
    }

    /* check if invariant */
    bool invariant = true;
    bool missing = false;
    for (mt_index_t j = 0; j<n_taxa; ++j)
    {
      char test_state = sequences[j][i];
      missing = (test_state == '?' || test_state == 'n');
      if (missing) {
        missing_count += w_vector[i];
      }
      else
        site_taxa_count += w_vector[i];
      if (test_state == '-') indel_count += w_vector[i];
      if ((test_state != cur_state) && !missing)
      {
        invariant = false;
      }
    }

    if (invariant)
    {
      invar_count += w_vector[i];
    }
  }

  for (size_t cur_seq = 0; cur_seq < n_taxa; ++cur_seq)
    free(sequences[cur_seq]);
  free(sequences);
  free(w_vector);

  cout << "N_TAXA     = " << n_taxa << endl;
  cout << "N_SITES    = " << n_sites << endl;
  cout << "N_PATTERNS = " << n_patterns << endl;
  cout << "INVAR SITES = " << invar_count << endl;
  cout << "INVAR PROP = " << 1.0 * invar_count / n_sites << endl;

  cout << "MISSING TO DATA RATIO = " << 1.0 * missing_count / site_taxa_count << endl;
  cout << "MISSING RATIO = " << 1.0 * missing_count / (n_sites * n_taxa) << endl;
  cout << "INDEL RATIO   = " << 1.0 * indel_count / site_taxa_count << endl;
}

}
