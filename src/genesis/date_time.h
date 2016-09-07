#ifndef GENESIS_UTILS_TOOLS_DATE_TIME_H_
#define GENESIS_UTILS_TOOLS_DATE_TIME_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2016 Lucas Czech

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    Contact:
    Lucas Czech <lucas.czech@h-its.org>
    Exelixis Lab, Heidelberg Institute for Theoretical Studies
    Schloss-Wolfsbrunnenweg 35, D-69118 Heidelberg, Germany
*/

/**
 * @brief Provides functions for date and time access.
 *
 * @file
 * @ingroup utils
 */

#include <string>

namespace genesis {
namespace utils {

/**
 * @brief Returns the current date as a string in the format "2014-12-31".
 */
std::string current_date();

/**
 * @brief Returns the current time as a string in the format "13:37:42".
 */
std::string current_time();

} // namespace utils
} // namespace genesis

#endif // include guard
