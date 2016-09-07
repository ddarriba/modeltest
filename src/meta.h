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

#ifndef META_H
#define META_H

#include "global_defs.h"

#include <iostream>
#include <iomanip>

class Meta
{
public:
    static bool parse_arguments(int argc, char *argv[], mt_options_t & exec_opt, mt_size_t *n_procs);

    static void print_ascii_logo(std::ostream  &out = std::cout);

    /**
     * @brief Prints the header
     * @param[in,out] out  The output stream
     */
    static void print_header(std::ostream &out = std::cout);

    /**
     * @brief Prints the version
     * @param[in,out] out  The output stream
     */
    static void print_version(std::ostream &out = std::cout);

    static void print_system_info(std::ostream &out = std::cout);

    /**
     * @brief Prints the execution options
     * @param[in] opts      The execution options
     * @param[in,out] out  The output stream
     */
    static void print_options(mt_options_t & opts, std::ostream &out = std::cout);

    static void print_usage(std::ostream& out = std::cout);
    static void print_help(std::ostream& out = std::cout);
};

#endif // META_H
