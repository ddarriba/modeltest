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
#include "model_defs.h"
#include "service/modeltestservice.h"
#include "thread/parallel_context.h"

#include <cerrno>
#include <cstdio>
#include <sstream>
#include <cassert>
#include <cstdarg>
#include <cstring>
#include <iomanip>
#include <unistd.h>
#include <algorithm>

using namespace std;

namespace modeltest {

static vector<partition_descriptor_t> * parse_partition (int * inp);
static char * read_file (string filename, long * filesize);
void init_lexan (const char * text, long n);

static lexToken get_token (int * input);
static int get_next_byte (void);
static int get_next_symbol (void);

static const char * rawtext;
static long rawtext_size;
static long pos = 0;

static int lex_table[ASCII_SIZE] = {
/*      */ SYM_UNKNOWN, SYM_UNKNOWN, SYM_UNKNOWN,   SYM_UNKNOWN,
/*      */ SYM_UNKNOWN, SYM_UNKNOWN, SYM_UNKNOWN,   SYM_UNKNOWN,
/*      */ SYM_UNKNOWN,     SYM_TAB,      SYM_CR,   SYM_UNKNOWN,
/*      */ SYM_UNKNOWN,      SYM_LF, SYM_UNKNOWN,   SYM_UNKNOWN,
/*      */ SYM_UNKNOWN, SYM_UNKNOWN, SYM_UNKNOWN,   SYM_UNKNOWN,
/*      */ SYM_UNKNOWN, SYM_UNKNOWN, SYM_UNKNOWN,   SYM_UNKNOWN,
/*      */ SYM_UNKNOWN, SYM_UNKNOWN, SYM_UNKNOWN,   SYM_UNKNOWN,
/*      */ SYM_UNKNOWN, SYM_UNKNOWN, SYM_UNKNOWN,   SYM_UNKNOWN,
/*  !"# */   SYM_SPACE, SYM_UNKNOWN, SYM_UNKNOWN,   SYM_UNKNOWN,
/* $%&' */ SYM_UNKNOWN, SYM_UNKNOWN, SYM_UNKNOWN,   SYM_UNKNOWN,
/* ()*+ */  SYM_OPAREN,  SYM_CPAREN, SYM_UNKNOWN,      SYM_PLUS,
/* ,-./ */   SYM_COMMA,    SYM_DASH,     SYM_DOT,     SYM_SLASH,
/* 0123 */   SYM_DIGIT,   SYM_DIGIT,   SYM_DIGIT,     SYM_DIGIT,
/* 4567 */   SYM_DIGIT,   SYM_DIGIT,   SYM_DIGIT,     SYM_DIGIT,
/* 89:; */   SYM_DIGIT,   SYM_DIGIT,   SYM_COLON, SYM_SEMICOLON,
/* <=>? */ SYM_UNKNOWN,   SYM_EQUAL, SYM_UNKNOWN,      SYM_CHAR,
/* @ABC */ SYM_UNKNOWN,    SYM_CHAR,    SYM_CHAR,      SYM_CHAR,
/* DEFG */    SYM_CHAR,    SYM_CHAR,    SYM_CHAR,      SYM_CHAR,
/* HIJK */    SYM_CHAR,    SYM_CHAR,    SYM_CHAR,      SYM_CHAR,
/* LMNO */    SYM_CHAR,    SYM_CHAR,    SYM_CHAR,      SYM_CHAR,
/* PQRS */    SYM_CHAR,    SYM_CHAR,    SYM_CHAR,      SYM_CHAR,
/* TUVW */    SYM_CHAR,    SYM_CHAR,    SYM_CHAR,      SYM_CHAR,
/* XYZ[ */    SYM_CHAR,    SYM_CHAR,    SYM_CHAR,  SYM_OBRACKET,
/* \]^_ */   SYM_SLASH, SYM_CBRACKET, SYM_UNKNOWN,     SYM_CHAR,
/* `abc */ SYM_UNKNOWN,    SYM_CHAR,    SYM_CHAR,      SYM_CHAR,
/* defg */    SYM_CHAR,    SYM_CHAR,    SYM_CHAR,      SYM_CHAR,
/* hijk */    SYM_CHAR,    SYM_CHAR,    SYM_CHAR,      SYM_CHAR,
/* lmno */    SYM_CHAR,    SYM_CHAR,    SYM_CHAR,      SYM_CHAR,
/* pqrs */    SYM_CHAR,    SYM_CHAR,    SYM_CHAR,      SYM_CHAR,
/* tuvw */    SYM_CHAR,    SYM_CHAR,    SYM_CHAR,      SYM_CHAR,
/* xyz{ */    SYM_CHAR,    SYM_CHAR,    SYM_CHAR,   SYM_UNKNOWN,
/* |}~  */    SYM_CHAR, SYM_UNKNOWN, SYM_UNKNOWN,   SYM_UNKNOWN
 };

Utils::Utils()
{
}

mt_mask_t Utils::get_parameters_from_template(template_models_t tool,
                                              data_type_t datatype)
{
    switch(tool)
    {
    case template_raxml:
        if (datatype == dt_dna)
            return dna_raxml_parameters;
        else if (datatype == dt_protein)
            return prot_raxml_parameters;
        break;
    case template_mrbayes:
        if (datatype == dt_dna)
            return dna_mrbayes_parameters;
        else if (datatype == dt_protein)
            return prot_mrbayes_parameters;
        break;
    case template_phyml:
        if (datatype == dt_dna)
            return dna_phyml_parameters;
        else if (datatype == dt_protein)
            return prot_phyml_parameters;
        break;
    case template_paup:
        if (datatype == dt_dna)
            return dna_paup_parameters;
        else if (datatype == dt_protein)
            return prot_paup_parameters;
        break;
    case template_none:
        return 0;
    }
    return 0;
}

const mt_index_t* Utils::get_prot_matrices_from_template(template_models_t tool,
                                                         mt_size_t *n_matrices)
{
    switch(tool)
    {
    case template_raxml:
        *n_matrices = N_PROT_RAXML_MATRICES;
        return prot_raxml_matrices_indices;
        break;
    case template_mrbayes:
        *n_matrices = N_PROT_MRBAYES_MATRICES;
        return prot_mrbayes_matrices_indices;
        break;
    case template_phyml:
        *n_matrices = N_PROT_PHYML_MATRICES;
        return prot_phyml_matrices_indices;
        break;
    case template_paup:
        *n_matrices = N_PROT_PAUP_MATRICES;
        return prot_paup_matrices_indices;
        break;
    case template_none:
        *n_matrices = 0;
        return 0;
    }
    return 0;
}

mt_size_t Utils::number_of_models(mt_size_t n_matrices, mt_mask_t model_params)
{
    mt_size_t n_models = n_matrices;
    n_models *= count_bits(model_params & MOD_MASK_FREQ_PARAMS);
    n_models *= count_bits(model_params & MOD_MASK_RATE_PARAMS);
    return n_models;
}

dna_subst_schemes_t Utils::get_dna_matrices_from_template(template_models_t tool)
{
    switch(tool)
    {
    case template_raxml:
        return dna_raxml_schemes;
        break;
    case template_mrbayes:
        return dna_mrbayes_schemes;
        break;
    case template_phyml:
        return dna_phyml_schemes;
        break;
    case template_paup:
        return dna_paup_schemes;
        break;
    default:
        return ss_undef;
    }
    return ss_undef;
}

string Utils::getBaseName(string const& filename)
{
  return string(
    find_if( filename.rbegin(), filename.rend(),
    MatchPathSeparator() ).base(),
    filename.end() );
}

size_t Utils::mem_size(unsigned int n_taxa,
                       unsigned int n_sites,
                       unsigned int n_categories,
                       unsigned int n_states)
{
    mt_size_t n_nodes = n_taxa * 2 - 2;
    mt_size_t n_branches = n_taxa * 2 - 3;
    size_t n_clvs = n_sites * n_states * n_categories * n_nodes;
    size_t n_scal = ((n_nodes + 1)* n_sites);
    size_t n_pmat = n_branches * n_sites * n_states * n_categories;

    size_t mem =  (n_clvs + n_pmat) * sizeof(double) + n_scal * sizeof(unsigned int) ;
//    size_t mem = (n_nodes * n_sites * n_states * n_categories +
//            n_states * n_states * n_categories * n_nodes) * sizeof(double) +
//            ((n_nodes + 1)* n_sites) * sizeof(int);

    return mem;
}

void * Utils::allocate(mt_size_t n, mt_size_t size) {
    void * ptr;
    ptr = malloc(n * size);
    if (!ptr) {
        exit_with_error("Error allocating memory");
    }
    return ptr;
}

void * Utils::c_allocate(mt_size_t n, mt_size_t size) {
    void * ptr;
    ptr = calloc(n, size);
    if (!ptr) {
        exit_with_error("Error allocating memory");
    }
    return ptr;
}

void Utils::exit_with_error(const char * message, ...) {
    va_list arg;
    if (ROOT) {
        va_start(arg, message);

        fprintf(stderr, "Error: ");
        vfprintf(stderr, message, arg);
        fprintf(stderr, "\n");

        va_end(arg);
    }

    ModelTestService::instance()->destroy_instance();

    ParallelContext::finalize(true);

    exit(EXIT_FAILURE);
}

partitioning_scheme_t *Utils::parse_partitions_file(string filename)
{
  long n;
  char * rawdata;
  int input;
  vector<partition_descriptor_t> * partitions;

  rawdata = read_file (filename, &n);
  if (!rawdata)
   {
     mt_errno = MT_ERROR_IO;
     snprintf(mt_errmsg, ERR_MSG_SIZE, "Cannot read file %s", filename.c_str());
     return 0;
   }

  n = (long) strlen (rawdata);

  init_lexan (rawdata, n);
  input = get_next_symbol();

  partitions = parse_partition (&input);
  if (partitions)
      Utils::sort_partitioning_scheme(*partitions);

  free (rawdata);
  return partitions;
}

static bool sort_partitions(partition_descriptor_t p1, partition_descriptor_t p2)
{
    return p1.regions[0].start < p2.regions[0].start;
}

static bool sort_regions(partition_region_t r1, partition_region_t r2)
{
    return r1.start < r2.start;
}

void Utils::sort_partitioning_scheme(partitioning_scheme_t & scheme)
{
    for (partition_descriptor_t & partition : scheme)
    {
        sort(partition.regions.begin(), partition.regions.end(), sort_regions);
    }
    sort(scheme.begin(), scheme.end(), sort_partitions);
}



static char * read_file (string filename, long * filesize)
{
  FILE * fp;
  char * rawdata;

  fp = fopen (filename.c_str(), "r");
  if (!fp) return (NULL);

  /* obtain file size */
  if (fseek (fp, 0, SEEK_END) == -1)
   {
     fclose (fp);
     return (NULL);
   }

  *filesize = ftell (fp);

  if (*filesize == -1)
   {
     fclose (fp);
     return (NULL);
   }
  rewind (fp);

  /* allocate buffer and read file contents */
  rawdata = (char *) Utils::allocate(((mt_size_t)(*filesize) + 10), sizeof (char));
  if (rawdata)
   {
     if (fread (rawdata, sizeof (char), (size_t)*filesize, fp) != (size_t) *filesize)
      {
        free (rawdata);
        rawdata = NULL;
      }
     else
      {
        rawdata[*filesize] = 0;
      }
   }

  fclose (fp);

  return rawdata;
}

static int get_next_byte (void)
{
  if (pos == rawtext_size)
   {
     ++pos;
     return (EOS);
   }

  return (rawtext[pos++]);
}

static int get_next_symbol (void)
{
  int ch, sym;

  ch = get_next_byte ();

  if (ch == EOS) return (SYM_EOF);
  if (ch >= ASCII_SIZE) return (SYM_UNKNOWN);

  sym = lex_table[ch];

  if (sym == SYM_LF)
   {
     if (get_next_byte() == '\n')
      {
        sym = SYM_LFCR;
      }
     else
      {
        --pos;
      }
   }

  return sym;
}

static lexToken get_token (int * input)
{
  lexToken token;
  long
    start_pos,
    isFloating = 0;

  token.lexeme = rawtext + pos - 1;
  start_pos    = pos;

  switch (*input)
   {
     case SYM_SLASH:
       token.tokenType = TOKEN_SLASH;
       *input = get_next_symbol();
       break;

     case SYM_DASH:
       token.tokenType = TOKEN_DASH;
       *input = get_next_symbol();
       break;

     case SYM_EQUAL:
       token.tokenType = TOKEN_EQUAL;
       *input = get_next_symbol();
       break;

     case SYM_SEMICOLON:
       token.tokenType = TOKEN_SEMICOLON;
       *input = get_next_symbol();
       break;

     case SYM_COMMA:
       token.tokenType = TOKEN_COMMA;
       *input = get_next_symbol();
       break;

     case SYM_COLON:
       token.tokenType = TOKEN_COLON;
       *input = get_next_symbol();
       break;

     case SYM_OPAREN:
       token.tokenType = TOKEN_OPAREN;
       *input = get_next_symbol();
       break;

     case SYM_CPAREN:
       token.tokenType = TOKEN_CPAREN;
       *input = get_next_symbol();
       break;

     case SYM_OBRACKET:
       token.tokenType = TOKEN_OBRACKET;
       *input = get_next_symbol();
       break;

     case SYM_CBRACKET:
       token.tokenType = TOKEN_CBRACKET;
       *input = get_next_symbol();
       break;

     case SYM_SPACE:
     case SYM_TAB:
       do
        {
          *input = get_next_symbol();
        } while (*input == SYM_SPACE || *input == SYM_TAB);
       token.len   = pos - start_pos;
       token.tokenType = TOKEN_WHITESPACE;
       if (*input == SYM_LFCR) --token.len;
       break;

     case SYM_DIGIT:
       do
        {
          *input = get_next_symbol();
        } while (*input == SYM_DIGIT);

       if (*input == SYM_DOT)
        {
          isFloating = 1;
          do
           {
             *input = get_next_symbol ();
           } while (*input == SYM_DIGIT);
        }

       if (*input != SYM_CHAR)
        {
          token.len   = pos - start_pos;
          if (!isFloating)
            token.tokenType = TOKEN_NUMBER;
          else
            token.tokenType = TOKEN_FLOAT;
        }
       else
        {
          /* check for E notation */
          if (rawtext[pos - 1] == 'E' || rawtext[pos - 1] == 'e')
           {
             *input = get_next_symbol ();

             if (*input == SYM_PLUS || *input == SYM_DASH || *input == SYM_DIGIT)
              {
                do
                 {
                   *input = get_next_symbol ();
                 } while (*input == SYM_DIGIT);

                if (*input != SYM_CHAR)
                 {
                   token.len = pos - start_pos;
                   token.tokenType = TOKEN_FLOAT;
                 }
              }
             else
              {
                token.len = pos - start_pos;
                token.tokenType = TOKEN_STRING;
              }
           }

          if (*input == SYM_CHAR)
           {
             do {
               *input = get_next_symbol();
             } while (*input == SYM_CHAR || *input == SYM_DIGIT || *input == SYM_DOT);
             token.len   = pos - start_pos;
             token.tokenType = TOKEN_STRING;
           }
        }

       if (*input == SYM_LFCR) --token.len;
       break;

     case SYM_CHAR:
       do
        {
          *input = get_next_symbol();
        }
       while (*input == SYM_CHAR  ||
              *input == SYM_DIGIT ||
              *input == SYM_DASH  ||
              *input == SYM_DOT);
       token.len   = pos - start_pos;
       token.tokenType = TOKEN_STRING;
       if (*input == SYM_LFCR) --token.len;
       break;

     case SYM_EOF:
       token.tokenType = TOKEN_EOF;
       break;

     case SYM_CR:
     case SYM_LF:
     case SYM_LFCR:
       do
        {
          *input = get_next_symbol();
        } while (*input == SYM_CR || *input == SYM_LFCR || *input == SYM_LF);
       token.tokenType = TOKEN_NEWLINE;
       break;
     case SYM_UNKNOWN:
     default:
       token.tokenType = TOKEN_UNKNOWN;
       break;
   }

  return (token);
}

static vector<partition_descriptor_t> * parse_partition (int * inp)
{
    int input;
    lexToken token;
    int lines = 0;
    int partition_id = 0;
    partition_region_t region;
    char * tmpchar;

    input  = *inp;

    NEXT_TOKEN

    vector<partition_descriptor_t> * partitions = new vector<partition_descriptor_t>();
    while (token.tokenType != TOKEN_EOF)
    {
        ++ lines;
        partition_descriptor_t pi;

        CONSUME (TOKEN_WHITESPACE | TOKEN_NEWLINE)

        /* read partition type */
        if (token.tokenType != TOKEN_STRING)
        {
            mt_errno = MT_ERROR_IO_FORMAT;
            snprintf(mt_errmsg, ERR_MSG_SIZE, "Invalid datatype in partition %d", lines);
            delete partitions;
            return 0;
        }

        tmpchar = (char *) Utils::c_allocate((mt_size_t)token.len+10, sizeof(char));
        strncpy (tmpchar, token.lexeme, (size_t)token.len);
        tmpchar[token.len] = '\0';

        /* check first for DNA */
        if (!strcasecmp(tmpchar,"DNA") || !strcasecmp(tmpchar,"NT"))
        {
            pi.datatype   = dt_dna;
            pi.states     = N_DNA_STATES;
        }
        else if (!strcasecmp(tmpchar,"PROT") || !strcasecmp(tmpchar,"AA"))
        {
            /* and  protein data */
            pi.datatype  = dt_protein;
            pi.states    = N_PROT_STATES;
        }
        else
        {
            mt_errno = MT_ERROR_IO_FORMAT;
            snprintf(mt_errmsg, ERR_MSG_SIZE, "Invalid datatype in partition %d: %s", lines, tmpchar);
            delete partitions;
            free (tmpchar);
            return 0;
        }
        free (tmpchar);

        NEXT_TOKEN
        CONSUME(TOKEN_WHITESPACE)

        if (token.tokenType == TOKEN_OBRACKET)
        {
          printf("Bracket Found\n");
          NEXT_TOKEN
          /* parse composition */
          while (token.tokenType != TOKEN_CBRACKET)
          {
            /* consume weight */

            tmpchar = (char *) Utils::c_allocate((mt_size_t)token.len+10, sizeof(char));
            strncpy (tmpchar, token.lexeme, (size_t)token.len);
            tmpchar[token.len] = '\0';

            mt_size_t w = Utils::parse_size(tmpchar);
            printf("WEIGHT = %d %s\n", w, tmpchar);

            free(tmpchar);
            CONSUME(TOKEN_WHITESPACE)
            CONSUME(TOKEN_COMMA)
            NEXT_TOKEN
          }
          NEXT_TOKEN
        }

        if (token.tokenType != TOKEN_COMMA)
        {
            mt_errno = MT_ERROR_IO_FORMAT;
            snprintf(mt_errmsg, ERR_MSG_SIZE, "Expecting ',' after datatype in partition %d", lines);
            delete partitions;
            return 0;
        }
        NEXT_TOKEN
                CONSUME(TOKEN_WHITESPACE)

                /* read partition name */
                if (token.tokenType != TOKEN_STRING)
        {
            mt_errno = MT_ERROR_IO_FORMAT;
            snprintf(mt_errmsg, ERR_MSG_SIZE, "Expecting partition name in partition %d", lines);
            delete partitions;
            return 0;
        }

        tmpchar = (char *) Utils::c_allocate((mt_size_t)token.len+10, sizeof(char));
        strncpy (tmpchar, token.lexeme, (size_t)token.len);
        tmpchar[token.len] = '\0';
        pi.partition_name = tmpchar;
        free (tmpchar);

        NEXT_TOKEN
                CONSUME(TOKEN_WHITESPACE)

                /* read equal sign */
                if (token.tokenType != TOKEN_EQUAL)
        {
            mt_errno = MT_ERROR_IO_FORMAT;
            snprintf(mt_errmsg, ERR_MSG_SIZE, "Expecting '=' in partition %d", lines);
            delete partitions;
            return 0;
        }
        NEXT_TOKEN
                CONSUME(TOKEN_WHITESPACE)

        /* read rhs */
        while (1)
        {
            if (token.tokenType != TOKEN_NUMBER)
            {
                mt_errno = MT_ERROR_IO_FORMAT;
                snprintf(mt_errmsg, ERR_MSG_SIZE, "Invalid numerical character (region start) in partition %d", lines);
                delete partitions;
                return 0;
            }
            region.start  = region.end = (mt_index_t) atoi (token.lexeme);
            region.stride = 1;
            NEXT_TOKEN
                    CONSUME(TOKEN_WHITESPACE)

                    if  (token.tokenType == TOKEN_DASH)
            {
                NEXT_TOKEN
                        CONSUME(TOKEN_WHITESPACE)
                        if (token.tokenType != TOKEN_NUMBER)
                {
                    mt_errno = MT_ERROR_IO_FORMAT;
                    snprintf(mt_errmsg, ERR_MSG_SIZE, "Invalid numerical character (region end) in partition %d", lines);
                    delete partitions;
                    return 0;
                }
                region.end = (mt_index_t) atoi (token.lexeme);
                if (region.end < region.start)
                {
                    mt_errno = MT_ERROR_IO_FORMAT;
                    snprintf(mt_errmsg, ERR_MSG_SIZE, "End is smaller than Start in partition %d", lines);
                    delete partitions;
                    return 0;
                }
                NEXT_TOKEN
                        CONSUME(TOKEN_WHITESPACE)
                        if (token.tokenType == TOKEN_SLASH)
                {
                    NEXT_TOKEN
                            CONSUME(TOKEN_WHITESPACE)
                            if (token.tokenType != TOKEN_NUMBER)
                    {
                        mt_errno = MT_ERROR_IO_FORMAT;
                        snprintf(mt_errmsg, ERR_MSG_SIZE, "Invalid stride in partition %d", lines);
                        delete partitions;
                        return 0;
                    }
                    region.stride = (mt_index_t) atoi (token.lexeme);
                    NEXT_TOKEN
                }
                CONSUME(TOKEN_WHITESPACE)
            }
            pi.regions.push_back(region);

            if (token.tokenType != TOKEN_COMMA) break;
            NEXT_TOKEN
                    CONSUME(TOKEN_WHITESPACE)
        }
        CONSUME(TOKEN_WHITESPACE | TOKEN_NEWLINE)

        pi.gap_aware = false;
        pi.unique_id = ++partition_id;

        partitions->push_back(pi);
    }

    mt_errno = 0;
    return (partitions);
}

void init_lexan (const char * text, long n)
{
  rawtext      = text;
  rawtext_size = n;
  pos          = 0;
}

mt_size_t Utils::count_bits( uint32_t i)
{
     i = i - ((i >> 1) & 0x55555555);
     i = (i & 0x33333333) + ((i >> 2) & 0x33333333);
     return (((i + (i >> 4)) & 0x0F0F0F0F) * 0x01010101) >> 24;
}

static bool is_integer(const char *str)
{
  int len = strlen(str);
  for (int i=0; i<len; ++i)
    if (str[i]<'0' || str[i]>'9')
      return false;
  return true;
}

mt_size_t Utils::parse_size(const char *str)
{
  mt_errno = 0;
  if (!is_integer(str))
  {
    mt_errno = MT_ERROR_NUMBER_INT;
    return UINT_MAX;
  }
  return (mt_size_t) atol(str);
}

mt_index_t Utils::parse_index(const char *str)
{
  mt_errno = 0;
  if (!is_integer(str))
  {
    mt_errno = MT_ERROR_NUMBER_INT;
    return UINT_MAX;
  }
  return (mt_index_t) atol(str);
}

string Utils::format_time(time_t seconds)
{
    time_t exec_time_h, exec_time_m, exec_time_s;
    exec_time_s = seconds;
    exec_time_h = exec_time_s / 3600;
    exec_time_s -= exec_time_h * 3600;
    exec_time_m = exec_time_s / 60;
    exec_time_s -= exec_time_m * 60;

    stringstream ss;
    ss << setfill('0') << exec_time_h <<"h:" << setw(2) << exec_time_m << ":" << setw(2) << exec_time_s << setfill(' ');
    return string(ss.str().c_str());
}

string Utils::int_array_to_string(const int array[], int length)
{
  string returnstring = "";
  for (int i = 0; i < length; i++)
    returnstring += '0' + array[i];
  return returnstring;
}

bool Utils::file_exists(const string &filename)
{
  FILE *fp;
  int res;
  fp = fopen(filename.c_str(),"rb");

  if(fp)
    {
      res = true;
      fclose(fp);
    }
  else
    res = false;

  return res;
}

bool Utils::file_writable(const string & filename)
{
  FILE *fp;
  int res;
  fp = fopen(filename.c_str(),"w");

  if(fp)
    {
      res = true;
      fclose(fp);
    }
  else
    res = false;

  return res;
}

ofstream * Utils::open_file_for_writing(const string &filename)
{
    ofstream * outfile = new ofstream();
    outfile->open(filename, ios_base::out);
    return outfile;
}

bool Utils::append_to_file(const string &filename,
                           const string &text)
{
    ofstream outfile;

    outfile.open(filename, ios_base::app);
    if (!outfile.is_open())
        return false;
    outfile << text;
    outfile.close();

    return true;
}

bool Utils::append_to_file(ofstream & outfile,
                           const string &text)
{
    assert(outfile.is_open());
    outfile << text;
    return true;
}




/* System utils */

mt_size_t Utils::count_logical_cores( void ) {
    unsigned logicalcpucount;
#if defined(_WIN32) || defined(WIN32)
    SYSTEM_INFO systeminfo;
    GetSystemInfo( &systeminfo );
    logicalcpucount = (unsigned) systeminfo.dwNumberOfProcessors;
#else
    logicalcpucount = (unsigned) sysconf( _SC_NPROCESSORS_ONLN );
#endif
    return logicalcpucount;
}

mt_size_t Utils::count_physical_cores( void ) {
    uint32_t registers[4];
    unsigned logicalcpucount;
    unsigned physicalcpucount;

    logicalcpucount = count_logical_cores();

    if ((logicalcpucount % 2) != 0)
        return logicalcpucount;
    __asm__ __volatile__ ("cpuid " :
                          "=a" (registers[0]),
            "=b" (registers[1]),
            "=c" (registers[2]),
            "=d" (registers[3])
        : "a" (1), "c" (0));

    unsigned CPUFeatureSet = registers[3];
    bool hyperthreading = CPUFeatureSet & (1 << 28);
    if (hyperthreading){
        physicalcpucount = logicalcpucount / 2;
    } else {
        physicalcpucount = logicalcpucount;
    }
    return physicalcpucount;
}

/* Copyright (C) 2014-2015 Tomas Flouri, Torbjorn Rognes, Jeff Epler */
unsigned long Utils::get_memtotal()
{
#if defined(_SC_PHYS_PAGES) && defined(_SC_PAGESIZE)

  long phys_pages = sysconf(_SC_PHYS_PAGES);
  long pagesize = sysconf(_SC_PAGESIZE);

  if ((phys_pages == -1) || (pagesize == -1))
  {
    mt_errno = MT_ERROR_SYSTEM;
    snprintf(mt_errmsg, ERR_MSG_SIZE, "Cannot determine amount of RAM");
    return 0L;
  }

  // sysconf(3) notes that pagesize * phys_pages can overflow, such as
  // when long is 32-bits and there's more than 4GB RAM.  Since vsearch
  // apparently targets LP64 systems like x86_64 linux, this will not
  // arise in practice on the intended platform.

  if (pagesize > LONG_MAX / phys_pages)
    return LONG_MAX;
  else
    return (unsigned long)pagesize * (unsigned long)phys_pages;

#elif defined(__APPLE__)
#if defined(CTL_HW) && (defined(HW_MEMSIZE)

  int mib [] = { CTL_HW, HW_MEMSIZE };
  int64_t ram = 0;
  size_t length = sizeof(ram);
  if(-1 == sysctl(mib, 2, &ram, &length, NULL, 0))
  {
    mt_errno = MT_ERROR_SYSTEM;
    snprintf(mt_errmsg, ERR_MSG_SIZE, "Cannot determine amount of RAM for OS X");
    return 0L;
  }
  return ram;
#else
  mt_errno = MT_ERROR_SYSTEM;
  snprintf(mt_errmsg, ERR_MSG_SIZE, "Cannot determine amount of RAM for OS X");
  return 0L;
#endif
#else

  struct sysinfo si;
  if (sysinfo(&si))
  {
    mt_errno = MT_ERROR_SYSTEM;
    snprintf(mt_errmsg, ERR_MSG_SIZE, "Cannot determine amount of RAM");
    return 0L;
  }
  return si.totalram * si.mem_unit;

#endif
}

} /* namespace */
