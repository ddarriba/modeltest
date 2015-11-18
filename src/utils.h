#ifndef UTILS_H
#define UTILS_H

#include "global_defs.h"
#include "msa.h"
#include "tree.h"

#include <string>
#include <vector>
#include <iostream>
#include <iomanip>

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

    /**
     * @brief Gets the base file of a path+filename
     * @param[in] filename The filename
     * @return The name of the file, excluding the path
     */
    static std::string getBaseName(std::string const& filename);

    /**
     * @brief Estimates the required memory
     * @param[in] n_taxa The number of taxa
     * @param[in] n_sites The sequence length
     * @param[in] n_categories The number of gamma rate categories
     * @param[in] n_states The number of states (e.g., 4 nt, 20 aa)
     * @return The estimated required memory, in bytes
     */
    static size_t mem_size(unsigned int n_taxa,
                           unsigned int n_sites,
                           unsigned int n_categories,
                           unsigned int n_states);

    /**
     * @brief Allocates an uninitialized chunk of memory and checks the return value
     * @param[in] n    The number of elements
     * @param[in] size The size of each element
     */
    static void * allocate(mt_size_t n, mt_size_t size);

    /**
     * @brief Allocates a chunk of memory initialized to zero and checks the return value
     * @param[in] n    The number of elements
     * @param[in] size The size of each element
     */
    static void * c_allocate(mt_size_t n, mt_size_t size);

    /**
     * @brief Exits modeltest and prints an error message
     * @param[in] message The error message
     * @param[in] ...     Formatting arguments
     */
    static void exit_with_error(const char * message, ...) __attribute__ ((noreturn));

    /**
     * @brief Parses a file containing a set of partitions
     * @param[in] filename The file to parse
     * @return The set of partitions sorted by starting site
     */
    static partitioning_scheme_t * parse_partitions_file (std::string filename);

    /**
     * @brief Sorts a partitioning scheme according to the starting sites
     * @param[in,out] scheme The scheme to sort
     */
    static void sort_partitioning_scheme(partitioning_scheme_t & scheme);

    /**
     * @brief Prints the execution options
     * @param[in] opts      The execution options
     * @param[in,out] out  The output stream
     */
    static void print_options(mt_options & opts, std::ostream  &out = std::cout);

    /**
     * @brief count the number of physical CPU cores
     * @return the number of physical CPU cores
     */
    static mt_size_t count_physical_cores( void );
};

}

#endif // UTILS_H
