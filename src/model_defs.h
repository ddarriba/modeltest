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

#ifndef MODEL_DEFS_H
#define MODEL_DEFS_H

#include "plldefs.h"
#include "global_defs.h"

#include <string>

#define N_DNA_STATES                 4
#define N_DNA_SUBST_RATES            6
#define N_DNA_MODEL_MATRICES        11
#define N_DNA_ALLMATRIX_COUNT      203
#define N_PROT_STATES               20
#define N_PROT_SUBST_RATES         190
#define N_PROT_MODEL_MATRICES       21
#define N_PROT_MODEL_ALL_MATRICES   22 //! all protein matrices

#define N_MIXTURE_CATS  4
#define LG4M_INDEX     19
#define LG4X_INDEX     20
#define PROT_GTR_INDEX 21

#define N_DNA_RAXML_MATRICES     3
#define N_DNA_MRBAYES_MATRICES   3
#define N_DNA_PHYML_MATRICES    11
#define N_DNA_PAUP_MATRICES     11
#define N_DNA_IQTREE_MATRICES   11
#define N_PROT_RAXML_MATRICES   19
#define N_PROT_PHYML_MATRICES   14
#define N_PROT_MRBAYES_MATRICES 8
#define N_PROT_PAUP_MATRICES    19

#define DNA_JC_INDEX             0
#define DNA_HKY_INDEX           18
#define DNA_GTR_INDEX          202
#define MATRIX_INDEX_UNDEF     999

#define MOD_PARAM_FIXED_FREQ      (1<<0)
#define MOD_PARAM_EMPIRICAL_FREQ  (1<<1)
#define MOD_PARAM_ESTIMATED_FREQ  (1<<2)
#define MOD_PARAM_NO_RATE_VAR     (1<<3)
#define MOD_PARAM_INV             (1<<4)
#define MOD_PARAM_GAMMA           (1<<5)
#define MOD_PARAM_INV_GAMMA       (1<<6)
#define MOD_PARAM_MIXTURE         (1<<7)
#define MOD_PARAM_FREE_RATES      (1<<8)

// b(branches) f(freqs) g(gamma) i(pinv)
// r(ratecats) s(substrates)
#define N_PARAMETERS                  6

#define MOD_PARAM_MIN_RPARAM MOD_PARAM_NO_RATE_VAR
#define MOD_PARAM_MAX_RPARAM MOD_PARAM_INV_GAMMA

#define MOD_MASK_FREQ_PARAMS    7
#define MOD_MASK_RATE_PARAMS  120
#define MOD_MASK_MIXT_PARAMS  384

typedef enum {
    freqs_equal,
    freqs_empirical,
    freqs_predef
} freqs_type_t;

const mt_index_t raxml_matrices_indices[3] = {
    DNA_JC_INDEX,
    DNA_HKY_INDEX,
    DNA_GTR_INDEX
};

const std::string dna_model_names[22] = {
    "JC",     "F81",
    "K80",    "HKY",
    "TrNef",  "TrN",
    "TPM1",   "TPM1uf",
    "TPM2",   "TPM2uf",
    "TPM3",   "TPM3uf",
    "TIM1ef", "TIM1",
    "TIM2ef", "TIM2",
    "TIM3ef", "TIM3",
    "TVMef",  "TVM",
    "SYM",    "GTR"
};

/* matrices included in default search */
const mt_index_t dna_model_matrices_indices[N_DNA_MODEL_MATRICES] = {
    0,    /*     JC/F81    */
    18,   /*    K80/HKY    */
    59,   /*  TrNef/TrN    */
    116,  /*   TPM1/TPM1uf */
    72,   /*   TPM2/TPM2uf */
    100,  /*   TPM3/TPM3uf */
    167,  /* TIM1ef/TIM1   */
    139,  /* TIM2ef/TIM2   */
    155,  /* TIM3ef/TIM3   */
    194,  /*  TVMef/TVM    */
    202   /*    SYM/GTR    */
};

const dna_subst_schemes_t dna_raxml_schemes = ss_3;
const mt_mask_t dna_raxml_parameters =
        MOD_PARAM_ESTIMATED_FREQ |
        MOD_PARAM_NO_RATE_VAR |
        MOD_PARAM_GAMMA |
        MOD_PARAM_INV |
        MOD_PARAM_INV_GAMMA;
const dna_subst_schemes_t dna_mrbayes_schemes = ss_3;
const mt_mask_t dna_mrbayes_parameters =
        MOD_PARAM_FIXED_FREQ |
        MOD_PARAM_ESTIMATED_FREQ |
        MOD_PARAM_NO_RATE_VAR |
        MOD_PARAM_GAMMA |
        MOD_PARAM_INV |
        MOD_PARAM_INV_GAMMA;
const dna_subst_schemes_t dna_phyml_schemes = ss_11;
const mt_mask_t dna_phyml_parameters =
        MOD_PARAM_FIXED_FREQ |
        MOD_PARAM_ESTIMATED_FREQ |
        MOD_PARAM_NO_RATE_VAR |
        MOD_PARAM_GAMMA |
        MOD_PARAM_INV |
        MOD_PARAM_INV_GAMMA;
const dna_subst_schemes_t dna_paup_schemes = ss_11;
const mt_mask_t dna_paup_parameters =
        MOD_PARAM_FIXED_FREQ |
        MOD_PARAM_ESTIMATED_FREQ |
        MOD_PARAM_NO_RATE_VAR |
        MOD_PARAM_GAMMA |
        MOD_PARAM_INV |
        MOD_PARAM_INV_GAMMA;

/* extended DNA map. '-' is considered as a 5th state, and '?' as missing
   data. Only '-' itself and '?' will fill the CLV for this 5th steate. */
const pll_state_t extended_dna_map[256] =
         {
           0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
           0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
           0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 16,  0,  0,
           0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 31,
           0,  1, 14,  2, 13,  0,  0,  4, 11,  0,  0, 12,  0,  3, 31, 15,
           0,  0,  5,  6,  8,  8,  7,  9, 15, 10,  0,  0,  0,  0,  0,  0,
           0,  1, 14,  2, 13,  0,  0,  4, 11,  0,  0, 12,  0,  3, 31, 15,
           0,  0,  5,  6,  8,  8,  7,  9, 15, 10,  0,  0,  0,  0,  0,  0,
           0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
           0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
           0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
           0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
           0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
           0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
           0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
           0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
         };

const std::string dna_model_matrices[N_DNA_ALLMATRIX_COUNT] = {
    "000000",                                                             // 0

    "000001", "000010", "000011", "000100", "000101", "000110", "000111", // 1
    "001000", "001001", "001010", "001011", "001100", "001101", "001110",
    "001111", "010000", "010001", "010010", "010011", "010100", "010101",
    "010110", "010111", "011000", "011001", "011010", "011011", "011100",
    "011101", "011110", "011111",

    "000012", "000102", "000112", "000120", "000121", "000122", "001002", // 32
    "001012", "001020", "001021", "001022", "001102", "001112", "001120",
    "001121", "001122", "001200", "001201", "001202", "001210", "001211",
    "001212", "001220", "001221", "001222", "010002", "010012", "010020",
    "010021", "010022", "010102", "010112", "010120", "010121", "010122",
    "010200", "010201", "010202", "010210", "010211", "010212", "010220",
    "010221", "010222", "011002", "011012", "011020", "011021", "011022",
    "011102", "011112", "011120", "011121", "011122", "011200", "011201",
    "011202", "011210", "011211", "011212", "011220", "011221", "011222",
    "012000", "012001", "012002", "012010", "012011", "012012", "012020",
    "012021", "012022", "012100", "012101", "012102", "012110", "012111",
    "012112", "012120", "012121", "012122", "012200", "012201", "012202",
    "012210", "012211", "012212", "012220", "012221", "012222",

    "000123", "001023", "001123", "001203", "001213", "001223", "001230", //122
    "001231", "001232", "001233", "010023", "010123", "010203", "010213",
    "010223", "010230", "010231", "010232", "010233", "011023", "011123",
    "011203", "011213", "011223", "011230", "011231", "011232", "011233",
    "012003", "012013", "012023", "012030", "012031", "012032", "012033",
    "012103", "012113", "012123", "012130", "012131", "012132", "012133",
    "012203", "012213", "012223", "012230", "012231", "012232", "012233",
    "012300", "012301", "012302", "012303", "012310", "012311", "012312",
    "012313", "012320", "012321", "012322", "012323", "012330", "012331",
    "012332", "012333",

    "001234", "010234", "011234", "012034", "012134", "012234", "012304", //187
    "012314", "012324", "012334", "012340", "012341", "012342", "012343",
    "012344",

    "012345"                                                              //202
};

const std::string prot_model_names[N_PROT_MODEL_ALL_MATRICES] = {
    "DAYHOFF",   //  0
    "LG",        //  1
    "DCMUT",     //  2
    "JTT",       //  3
    "MTREV",     //  4
    "WAG",       //  5
    "RTREV",     //  6
    "CPREV",     //  7
    "VT",        //  8
    "BLOSUM62",  //  9
    "MTMAM",     // 10
    "MTART",     // 11
    "MTZOA",     // 12
    "PMB",       // 13
    "HIVB",      // 14
    "HIVW",      // 15
    "JTT-DCMUT", // 16
    "FLU",       // 17
    "STMTREV",   // 18
    /* additional mixture matrices */
    "LG4M",      // 19
    "LG4X",      // 20
    /* matrices not included by default*/
    "GTR"        // 21
};

/* matrices included in default search */
const mt_index_t prot_model_matrices_indices[N_PROT_MODEL_MATRICES] = {
    0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18
};

const mt_index_t prot_raxml_matrices_indices[N_PROT_RAXML_MATRICES] = {
    0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18
};
const mt_mask_t prot_raxml_parameters =
        MOD_PARAM_FIXED_FREQ |
        MOD_PARAM_ESTIMATED_FREQ |
        MOD_PARAM_GAMMA |
        MOD_PARAM_INV |
        MOD_PARAM_INV_GAMMA;
const mt_index_t prot_mrbayes_matrices_indices[N_PROT_MRBAYES_MATRICES] = {
    0,4,10,5,6,7,8,9
};
const mt_mask_t prot_mrbayes_parameters =
        MOD_PARAM_FIXED_FREQ |
        MOD_PARAM_ESTIMATED_FREQ |
        MOD_PARAM_NO_RATE_VAR |
        MOD_PARAM_GAMMA |
        MOD_PARAM_INV |
        MOD_PARAM_INV_GAMMA;
const mt_index_t prot_phyml_matrices_indices[N_PROT_PHYML_MATRICES] = {
    1,5,3,4,0,2,6,7,8,9,10,11,15,14
};
const mt_mask_t prot_phyml_parameters =
        MOD_PARAM_FIXED_FREQ |
        MOD_PARAM_ESTIMATED_FREQ |
        MOD_PARAM_NO_RATE_VAR |
        MOD_PARAM_GAMMA |
        MOD_PARAM_INV |
        MOD_PARAM_INV_GAMMA;
const mt_index_t prot_paup_matrices_indices[N_PROT_PAUP_MATRICES] = {
    0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18
};
const mt_mask_t prot_paup_parameters =
        MOD_PARAM_FIXED_FREQ |
        MOD_PARAM_ESTIMATED_FREQ |
        MOD_PARAM_NO_RATE_VAR |
        MOD_PARAM_GAMMA |
        MOD_PARAM_INV |
        MOD_PARAM_INV_GAMMA;

extern const double * prot_model_rates[N_PROT_MODEL_MATRICES];
extern const double * prot_model_freqs[N_PROT_MODEL_MATRICES];

#endif // MODEL_DEFS_H
