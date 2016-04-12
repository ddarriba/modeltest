#ifndef META_H
#define META_H

#include "global_defs.h"

#include <iostream>
#include <iomanip>

class Meta
{
public:
    static bool parse_arguments(int argc, char *argv[], mt_options_t & exec_opt, mt_size_t *n_procs);

    /**
     * @brief Prints the header
     * @param[in,out] out  The output stream
     */
    static void print_header(std::ostream  &out = std::cout);

    /**
     * @brief Prints the version
     * @param[in,out] out  The output stream
     */
    static void print_version(std::ostream& out = std::cout);

    static void print_system_info(std::ostream  &out);

    /**
     * @brief Prints the execution options
     * @param[in] opts      The execution options
     * @param[in,out] out  The output stream
     */
    static void print_options(mt_options_t & opts, std::ostream  &out = std::cout);

    static void print_usage(std::ostream& out);
    static void print_help(std::ostream& out);
};

#endif // META_H
