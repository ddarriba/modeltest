#ifndef UTILS_H
#define UTILS_H

#include "global_defs.h"
#include <string>
#include <vector>
#include <iostream>

namespace modeltest {

#ifdef WIN32
struct MatchPathSeparator
{
    bool operator()( char ch ) const
    {
        return ch == '\\' || ch == '/';
    }
};
#else
struct MatchPathSeparator
{
    bool operator()( char ch ) const
    {
        return ch == '/';
    }
};
#endif

#define  ASCII_SIZE                128
#define  EOS                       0x00000200

#define  SYM_CR                    1 << 0
#define  SYM_LF                    1 << 1
#define  SYM_LFCR                  1 << 2
#define  SYM_DIGIT                 1 << 3
#define  SYM_CHAR                  1 << 4
#define  SYM_SPACE                 1 << 5
#define  SYM_TAB                   1 << 6
#define  SYM_EOF                   1 << 7
#define  SYM_UNKNOWN               1 << 8
#define  SYM_DOT                   1 << 9
#define  SYM_COLON                 1 << 10
#define  SYM_OPAREN                1 << 11
#define  SYM_CPAREN                1 << 12
#define  SYM_COMMA                 1 << 13
#define  SYM_SEMICOLON             1 << 14
#define  SYM_EQUAL                 1 << 15
#define  SYM_DASH                  1 << 16
#define  SYM_SLASH                 1 << 17
#define  SYM_PLUS                  1 << 18

#define  TOKEN_NUMBER              1 << 0
#define  TOKEN_STRING              1 << 1
#define  TOKEN_EOF                 1 << 2
#define  TOKEN_WHITESPACE          1 << 3
#define  TOKEN_NEWLINE             1 << 4
#define  TOKEN_UNKNOWN             1 << 5
#define  TOKEN_COLON               1 << 6
#define  TOKEN_OPAREN              1 << 7
#define  TOKEN_CPAREN              1 << 8
#define  TOKEN_FLOAT               1 << 9
#define  TOKEN_COMMA               1 << 10
#define  TOKEN_SEMICOLON           1 << 11
#define  TOKEN_EQUAL               1 << 12
#define  TOKEN_DASH                1 << 13
#define  TOKEN_SLASH               1 << 14

#define CONSUME(x)         while (token.tokenType & (x)) token = get_token (&input);
#define NEXT_TOKEN         token = get_token (&input);

typedef struct
 {
   int 	        tokenType;
   const char * lexeme;
   long         len;
 } lexToken;

class Utils
{
public:
    Utils();
    static std::string getBaseName(std::string const& filename);
    static size_t mem_size(unsigned int n_taxa,
                           unsigned int n_sites,
                           unsigned int n_categories,
                           unsigned int n_states);

    static void * allocate(mt_size_t n, mt_size_t size);
    static void * c_allocate(mt_size_t n, mt_size_t size);
    static void exit_with_error(const char * message, ...) __attribute__ ((noreturn));

    static std::vector<partition_t> * parse_partitions_file (std::string filename);
    static void print_options(mt_options & opts, std::ostream  &out = std::cout);
};

}

#endif // UTILS_H
