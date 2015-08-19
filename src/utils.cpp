#include "utils.h"

#include <algorithm>
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

static int lex_table[PLL_ASCII_SIZE] = {
/*      */ PLL_SYM_UNKNOWN, PLL_SYM_UNKNOWN, PLL_SYM_UNKNOWN,   PLL_SYM_UNKNOWN,
/*      */ PLL_SYM_UNKNOWN, PLL_SYM_UNKNOWN, PLL_SYM_UNKNOWN,   PLL_SYM_UNKNOWN,
/*      */ PLL_SYM_UNKNOWN,     PLL_SYM_TAB,      PLL_SYM_CR,   PLL_SYM_UNKNOWN,
/*      */ PLL_SYM_UNKNOWN,      PLL_SYM_LF, PLL_SYM_UNKNOWN,   PLL_SYM_UNKNOWN,
/*      */ PLL_SYM_UNKNOWN, PLL_SYM_UNKNOWN, PLL_SYM_UNKNOWN,   PLL_SYM_UNKNOWN,
/*      */ PLL_SYM_UNKNOWN, PLL_SYM_UNKNOWN, PLL_SYM_UNKNOWN,   PLL_SYM_UNKNOWN,
/*      */ PLL_SYM_UNKNOWN, PLL_SYM_UNKNOWN, PLL_SYM_UNKNOWN,   PLL_SYM_UNKNOWN,
/*      */ PLL_SYM_UNKNOWN, PLL_SYM_UNKNOWN, PLL_SYM_UNKNOWN,   PLL_SYM_UNKNOWN,
/*  !"# */   PLL_SYM_SPACE, PLL_SYM_UNKNOWN, PLL_SYM_UNKNOWN,   PLL_SYM_UNKNOWN,
/* $%&' */ PLL_SYM_UNKNOWN, PLL_SYM_UNKNOWN, PLL_SYM_UNKNOWN,   PLL_SYM_UNKNOWN,
/* ()*+ */  PLL_SYM_OPAREN,  PLL_SYM_CPAREN, PLL_SYM_UNKNOWN,      PLL_SYM_PLUS,
/* ,-./ */   PLL_SYM_COMMA,    PLL_SYM_DASH,     PLL_SYM_DOT,     PLL_SYM_SLASH,
/* 0123 */   PLL_SYM_DIGIT,   PLL_SYM_DIGIT,   PLL_SYM_DIGIT,     PLL_SYM_DIGIT,
/* 4567 */   PLL_SYM_DIGIT,   PLL_SYM_DIGIT,   PLL_SYM_DIGIT,     PLL_SYM_DIGIT,
/* 89:; */   PLL_SYM_DIGIT,   PLL_SYM_DIGIT,   PLL_SYM_COLON, PLL_SYM_SEMICOLON,
/* <=>? */ PLL_SYM_UNKNOWN,   PLL_SYM_EQUAL, PLL_SYM_UNKNOWN,      PLL_SYM_CHAR,
/* @ABC */ PLL_SYM_UNKNOWN,    PLL_SYM_CHAR,    PLL_SYM_CHAR,      PLL_SYM_CHAR,
/* DEFG */    PLL_SYM_CHAR,    PLL_SYM_CHAR,    PLL_SYM_CHAR,      PLL_SYM_CHAR,
/* HIJK */    PLL_SYM_CHAR,    PLL_SYM_CHAR,    PLL_SYM_CHAR,      PLL_SYM_CHAR,
/* LMNO */    PLL_SYM_CHAR,    PLL_SYM_CHAR,    PLL_SYM_CHAR,      PLL_SYM_CHAR,
/* PQRS */    PLL_SYM_CHAR,    PLL_SYM_CHAR,    PLL_SYM_CHAR,      PLL_SYM_CHAR,
/* TUVW */    PLL_SYM_CHAR,    PLL_SYM_CHAR,    PLL_SYM_CHAR,      PLL_SYM_CHAR,
/* XYZ[ */    PLL_SYM_CHAR,    PLL_SYM_CHAR,    PLL_SYM_CHAR,   PLL_SYM_UNKNOWN,
/* \]^_ */ PLL_SYM_SLASH,   PLL_SYM_UNKNOWN, PLL_SYM_UNKNOWN,      PLL_SYM_CHAR,
/* `abc */ PLL_SYM_UNKNOWN,    PLL_SYM_CHAR,    PLL_SYM_CHAR,      PLL_SYM_CHAR,
/* defg */    PLL_SYM_CHAR,    PLL_SYM_CHAR,    PLL_SYM_CHAR,      PLL_SYM_CHAR,
/* hijk */    PLL_SYM_CHAR,    PLL_SYM_CHAR,    PLL_SYM_CHAR,      PLL_SYM_CHAR,
/* lmno */    PLL_SYM_CHAR,    PLL_SYM_CHAR,    PLL_SYM_CHAR,      PLL_SYM_CHAR,
/* pqrs */    PLL_SYM_CHAR,    PLL_SYM_CHAR,    PLL_SYM_CHAR,      PLL_SYM_CHAR,
/* tuvw */    PLL_SYM_CHAR,    PLL_SYM_CHAR,    PLL_SYM_CHAR,      PLL_SYM_CHAR,
/* xyz{ */    PLL_SYM_CHAR,    PLL_SYM_CHAR,    PLL_SYM_CHAR,   PLL_SYM_UNKNOWN,
/* |}~  */    PLL_SYM_CHAR, PLL_SYM_UNKNOWN, PLL_SYM_UNKNOWN,   PLL_SYM_UNKNOWN
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

vector<partition_t> * Utils::parse_partitions_file (string filename)
{
  long n;
  char * rawdata;
  int input;
  vector<partition_t> * partitions;

  rawdata = read_file (filename, &n);
  if (!rawdata)
   {
     errno = MT_ERROR_IO;
     return 0;
   }

  n = (long) strlen (rawdata);

  init_lexan (rawdata, n);
  input = get_next_symbol();

  partitions = parse_partition (&input);
  if (!partitions)
      errno = MT_ERROR_IO_FORMAT;

  free (rawdata);
  return partitions;
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
        if (n_dna_parts)
            out << "  " << left << setw(15) << " "
                << n_dna_parts << " DNA partitions" << endl;
        if (n_prot_parts)
            out << "  " << left << setw(15) << " "
                << n_prot_parts << " protein partitions" << endl;
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
     return (PLL_EOS);
   }

  return (rawtext[pos++]);
}

static int get_next_symbol (void)
{
  int ch, sym;

  ch = get_next_byte ();

  if (ch == PLL_EOS) return (PLL_SYM_EOF);
  if (ch >= PLL_ASCII_SIZE) return (PLL_SYM_UNKNOWN);

  sym = lex_table[ch];

  if (sym == PLL_SYM_LF)
   {
     if (get_next_byte() == '\n')
      {
        sym = PLL_SYM_LFCR;
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
     case PLL_SYM_SLASH:
       token.tokenType = PLL_TOKEN_SLASH;
       *input = get_next_symbol();
       break;

     case PLL_SYM_DASH:
       token.tokenType = PLL_TOKEN_DASH;
       *input = get_next_symbol();
       break;

     case PLL_SYM_EQUAL:
       token.tokenType = PLL_TOKEN_EQUAL;
       *input = get_next_symbol();
       break;

     case PLL_SYM_SEMICOLON:
       token.tokenType = PLL_TOKEN_SEMICOLON;
       *input = get_next_symbol();
       break;

     case PLL_SYM_COMMA:
       token.tokenType = PLL_TOKEN_COMMA;
       *input = get_next_symbol();
       break;

     case PLL_SYM_COLON:
       token.tokenType = PLL_TOKEN_COLON;
       *input = get_next_symbol();
       break;

     case PLL_SYM_OPAREN:
       token.tokenType = PLL_TOKEN_OPAREN;
       *input = get_next_symbol();
       break;

     case PLL_SYM_CPAREN:
       token.tokenType = PLL_TOKEN_CPAREN;
       *input = get_next_symbol();
       break;

     case PLL_SYM_SPACE:
     case PLL_SYM_TAB:
       do
        {
          *input = get_next_symbol();
        } while (*input == PLL_SYM_SPACE || *input == PLL_SYM_TAB);
       token.len   = pos - start_pos;
       token.tokenType = PLL_TOKEN_WHITESPACE;
       if (*input == PLL_SYM_LFCR) --token.len;
       break;

     case PLL_SYM_DIGIT:
       do
        {
          *input = get_next_symbol();
        } while (*input == PLL_SYM_DIGIT);

       if (*input == PLL_SYM_DOT)
        {
          isFloating = 1;
          do
           {
             *input = get_next_symbol ();
           } while (*input == PLL_SYM_DIGIT);
        }

       if (*input != PLL_SYM_CHAR)
        {
          token.len   = pos - start_pos;
          if (!isFloating)
            token.tokenType = PLL_TOKEN_NUMBER;
          else
            token.tokenType = PLL_TOKEN_FLOAT;
        }
       else
        {
          /* check for E notation */
          if (rawtext[pos - 1] == 'E' || rawtext[pos - 1] == 'e')
           {
             *input = get_next_symbol ();

             if (*input == PLL_SYM_PLUS || *input == PLL_SYM_DASH || *input == PLL_SYM_DIGIT)
              {
                do
                 {
                   *input = get_next_symbol ();
                 } while (*input == PLL_SYM_DIGIT);

                if (*input != PLL_SYM_CHAR)
                 {
                   token.len = pos - start_pos;
                   token.tokenType = PLL_TOKEN_FLOAT;
                 }
              }
             else
              {
                token.len = pos - start_pos;
                token.tokenType = PLL_TOKEN_STRING;
              }
           }

          if (*input == PLL_SYM_CHAR)
           {
             do {
               *input = get_next_symbol();
             } while (*input == PLL_SYM_CHAR || *input == PLL_SYM_DIGIT || *input == PLL_SYM_DOT);
             token.len   = pos - start_pos;
             token.tokenType = PLL_TOKEN_STRING;
           }
        }

       if (*input == PLL_SYM_LFCR) --token.len;
       break;

     case PLL_SYM_CHAR:
       do
        {
          *input = get_next_symbol();
        }
       while (*input == PLL_SYM_CHAR  ||
              *input == PLL_SYM_DIGIT ||
              *input == PLL_SYM_DASH  ||
              *input == PLL_SYM_DOT);
       token.len   = pos - start_pos;
       token.tokenType = PLL_TOKEN_STRING;
       if (*input == PLL_SYM_LFCR) --token.len;
       break;

     case PLL_SYM_EOF:
       token.tokenType = PLL_TOKEN_EOF;
       break;

     case PLL_SYM_CR:
     case PLL_SYM_LF:
     case PLL_SYM_LFCR:
       do
        {
          *input = get_next_symbol();
        } while (*input == PLL_SYM_CR || *input == PLL_SYM_LFCR || *input == PLL_SYM_LF);
       token.tokenType = PLL_TOKEN_NEWLINE;
       break;
     case PLL_SYM_UNKNOWN:
     default:
       token.tokenType = PLL_TOKEN_UNKNOWN;
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
    while (token.tokenType != PLL_TOKEN_EOF)
    {
        ++ lines;
        partition_t pi;

        CONSUME (PLL_TOKEN_WHITESPACE | PLL_TOKEN_NEWLINE)


        /* read partition type */
        if (token.tokenType != PLL_TOKEN_STRING)
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
                CONSUME(PLL_TOKEN_WHITESPACE)

                if (token.tokenType != PLL_TOKEN_COMMA)
        {
            cerr << "Expecting ',' after datatype in partition " << lines << endl;
            delete partitions;
            return 0;
        }
        NEXT_TOKEN
                CONSUME(PLL_TOKEN_WHITESPACE)

                /* read partition name */
                if (token.tokenType != PLL_TOKEN_STRING)
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
                CONSUME(PLL_TOKEN_WHITESPACE)

                /* read equal sign */
                if (token.tokenType != PLL_TOKEN_EQUAL)
        {
            cerr << "Expecting '=' in partition " << lines << endl;
            delete partitions;
            return 0;
        }
        NEXT_TOKEN
                CONSUME(PLL_TOKEN_WHITESPACE)

                /* read rhs */
                while (1)
        {
            if (token.tokenType != PLL_TOKEN_NUMBER)
            {
                cerr << "Invalid numerical character (region start) in partition " << lines << endl;
                delete partitions;
                return 0;
            }
            region.start  = region.end = atoi (token.lexeme);
            region.stride = 1;
            NEXT_TOKEN
                    CONSUME(PLL_TOKEN_WHITESPACE)

                    if  (token.tokenType == PLL_TOKEN_DASH)
            {
                NEXT_TOKEN
                        CONSUME(PLL_TOKEN_WHITESPACE)
                        if (token.tokenType != PLL_TOKEN_NUMBER)
                {
                    cerr << "Invalid numerical character (region end) in partition " << lines << endl;
                    delete partitions;
                    return 0;
                }
                region.end = atoi (token.lexeme);
                if (region.end < region.start)
                {
                    cerr << "End is smaller than Start in partition " << lines << endl;
                    delete partitions;
                    return 0;
                }
                NEXT_TOKEN
                        CONSUME(PLL_TOKEN_WHITESPACE)
                        if (token.tokenType == PLL_TOKEN_SLASH)
                {
                    NEXT_TOKEN
                            CONSUME(PLL_TOKEN_WHITESPACE)
                            if (token.tokenType != PLL_TOKEN_NUMBER)
                    {
                        cerr << "Invalid stride in partition " << lines << endl;
                        delete partitions;
                        return 0;
                    }
                    region.stride = atoi (token.lexeme);
                    NEXT_TOKEN
                }
                CONSUME(PLL_TOKEN_WHITESPACE)
            }
            pi.regions.push_back(region);

            if (token.tokenType != PLL_TOKEN_COMMA) break;
            NEXT_TOKEN
                    CONSUME(PLL_TOKEN_WHITESPACE)
        }
        CONSUME(PLL_TOKEN_WHITESPACE | PLL_TOKEN_NEWLINE)

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
