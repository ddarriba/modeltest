#include "utils.h"

#include <algorithm>
#include <cerrno>
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <iomanip>

using namespace std;

namespace modeltest {

static vector<partition_t> * parse_partition (int * inp);
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
/* XYZ[ */    SYM_CHAR,    SYM_CHAR,    SYM_CHAR,   SYM_UNKNOWN,
/* \]^_ */ SYM_SLASH,   SYM_UNKNOWN, SYM_UNKNOWN,      SYM_CHAR,
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
    size_t mem = (n_nodes * n_sites * n_states * n_categories +
            n_states * n_states * n_categories * n_nodes) * sizeof(double) +
            ((n_nodes + 1)* n_sites) * sizeof(int);

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
#ifdef HAVE_MPI
    MPI_Finalize();
#endif
    exit(EXIT_FAILURE);
}

partitioning_scheme_t *Utils::parse_partitions_file(string filename)
{
  long n;
  char * rawdata;
  int input;
  vector<partition_t> * partitions;

  rawdata = read_file (filename, &n);
  if (!rawdata)
   {
     mt_errno = MT_ERROR_IO;
     snprintf(mt_errmsg, 200, "Cannot read file %s", filename.c_str());
     return 0;
   }

  n = (long) strlen (rawdata);

  init_lexan (rawdata, n);
  input = get_next_symbol();

  partitions = parse_partition (&input);
  if (partitions)
      Utils::sort_partitioning_scheme(*partitions);
  else
  {
      mt_errno = MT_ERROR_IO_FORMAT;
      snprintf(mt_errmsg, 200, "Wrong format in %s", filename.c_str());
  }

  free (rawdata);
  return partitions;
}

static bool sort_partitions(partition_t p1, partition_t p2)
{
    return p1.regions[0].start < p2.regions[0].start;
}

static bool sort_regions(partition_region_t r1, partition_region_t r2)
{
    return r1.start < r2.start;
}

void Utils::sort_partitioning_scheme(partitioning_scheme_t & scheme)
{
    for (partition_t & partition : scheme)
    {
        sort(partition.regions.begin(), partition.regions.end(), sort_regions);
    }
    sort(scheme.begin(), scheme.end(), sort_partitions);
}

void Utils::print_options(mt_options & opts, ostream  &out)
{
    out << setw(80) << setfill('-') << ""  << setfill(' ') << endl;
    out << left << setw(15) << "MSA:" << opts.msa_filename << endl;
    out << left << setw(15) << "#taxa:" << opts.n_taxa << endl;
    out << left << setw(15) << "#sites:" << opts.n_sites << endl;
    if (opts.tree_filename.compare(""))
        out << left << setw(15) << "Tree:" << opts.tree_filename << endl;
    if (opts.partitions_desc)
    {
        out << left << setw(15) << "Partitions:" << opts.partitions_filename << endl;
        out << left << setw(15) << "" << opts.partitions_desc->size() << " partitions" << endl;
        mt_size_t n_prot_parts = 0;
        mt_size_t n_dna_parts = 0;
        if (opts.verbose > VERBOSITY_DEFAULT)
        {
            out << setw(15) << " " << setw(10) << setfill('-') << ""
                << setfill(' ') << endl;
            for (mt_index_t i=0; i<opts.partitions_desc->size(); i++)
            {
                out << left << setw(15) << " "
                    << setw(4) << right << i+1 << " ";
                switch (opts.partitions_desc->at(i).datatype)
                {
                case dt_dna:
                    out << "[NT] ";
                    n_dna_parts++;
                    break;
                case dt_protein:
                    out << "[AA] ";
                    n_prot_parts++;
                    break;
                }
                out << opts.partitions_desc->at(i).partition_name << " : ";
                for (partition_region_t & region : opts.partitions_desc->at(i).regions)
                {
                    out << region.start << "-" << region.end;
                    if (region.stride != 1)
                        out << "/" << region.stride;
                    out << " ";
                }
                out << endl;
            }
            out << setw(15) << " " << setw(10) << setfill('-') << ""
                << setfill(' ') << endl;
        }
        else
        {
            for (mt_index_t i=0; i<opts.partitions_desc->size(); i++)
            {
                switch (opts.partitions_desc->at(i).datatype)
                {
                case dt_dna:
                    n_dna_parts++;
                    break;
                case dt_protein:
                    n_prot_parts++;
                    break;
                }
            }
        }
        if (n_dna_parts)
            out << left << setw(15) << " "
                << setw(4) << right << n_dna_parts
                << " DNA partitions" << endl;
        if (n_prot_parts)
            out << left << setw(15) << " "
                << setw(4) << right << n_prot_parts
                << " protein partitions" << endl;
    }
    out << setw(80) << setfill('-') << "" << setfill(' ') << endl;
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

static vector<partition_t> * parse_partition (int * inp)
{
    int input;
    lexToken token;
    int lines = 0;
    partition_region_t region;
    char * tmpchar;

    input  = *inp;

    NEXT_TOKEN

    vector<partition_t> * partitions = new vector<partition_t>();
    while (token.tokenType != TOKEN_EOF)
    {
        ++ lines;
        partition_t pi;

        CONSUME (TOKEN_WHITESPACE | TOKEN_NEWLINE)


        /* read partition type */
        if (token.tokenType != TOKEN_STRING)
        {
            cerr << "Invalid datatype in partition "<< lines << endl;
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
        }
        else if (!strcasecmp(tmpchar,"PROT") || !strcasecmp(tmpchar,"AA"))
        {
            /* and  protein data */
            pi.datatype  = dt_protein;
        }
        else
        {
            cerr << "Invalid datatype in partition " << lines << ": " << tmpchar << endl;
            delete partitions;
            free (tmpchar);
            return 0;
        }
        free (tmpchar);

        NEXT_TOKEN
                CONSUME(TOKEN_WHITESPACE)

                if (token.tokenType != TOKEN_COMMA)
        {
            cerr << "Expecting ',' after datatype in partition " << lines << endl;
            delete partitions;
            return 0;
        }
        NEXT_TOKEN
                CONSUME(TOKEN_WHITESPACE)

                /* read partition name */
                if (token.tokenType != TOKEN_STRING)
        {
            cerr << "Expecting partition name in partition "<< lines << endl;
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
            cerr << "Expecting '=' in partition " << lines << endl;
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
                cerr << "Invalid numerical character (region start) in partition " << lines << endl;
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
                    cerr << "Invalid numerical character (region end) in partition " << lines << endl;
                    delete partitions;
                    return 0;
                }
                region.end = (mt_index_t) atoi (token.lexeme);
                if (region.end < region.start)
                {
                    cerr << "End is smaller than Start in partition " << lines << endl;
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
                        cerr << "Invalid stride in partition " << lines << endl;
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

        partitions->push_back(pi);
    }

    return (partitions);
}

void init_lexan (const char * text, long n)
{
  rawtext      = text;
  rawtext_size = n;
  pos          = 0;
}

} /* namespace */
