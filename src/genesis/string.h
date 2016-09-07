#ifndef GENESIS_UTILS_TEXT_STRING_H_
#define GENESIS_UTILS_TEXT_STRING_H_

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
 * @brief Provides some commonly used string utility functions.
 *
 * @file
 * @ingroup utils
 */

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace genesis {
namespace utils {

// =================================================================================================
//     Compare
// =================================================================================================

bool equals_ci( std::string const& lhs, std::string const& rhs );

bool starts_with( std::string const & text, std::string const & start );
bool ends_with(   std::string const & text, std::string const & ending );

// =================================================================================================
//     Substrings
// =================================================================================================

std::string head( std::string const& text, size_t lines = 10 );
std::string tail( std::string const& text, size_t lines = 10 );

// =================================================================================================
//     Find and Count
// =================================================================================================

size_t count_substring_occurrences( std::string const& str, std::string const& sub );

std::vector<std::string> split (
    std::string const& str,
    std::string const& delimiters = " ",
    const bool trim_empty = true
);

// =================================================================================================
//     Manipulate
// =================================================================================================

std::string indent(
    std::string const& text,
    std::string const& indentation = "    "
);

std::string replace_all (
    std::string const& text,
    std::string const& search,
    std::string const& replace
);

std::string replace_all_chars (
    std::string const& text,
    std::string const& search_chars,
    char               replace
);

std::string trim_right (
    std::string const& s,
    std::string const& delimiters = " \f\n\r\t\v"
);

std::string trim_left (
    std::string const& s,
    std::string const& delimiters = " \f\n\r\t\v"
);

std::string trim (
    std::string const& s,
    std::string const& delimiters = " \f\n\r\t\v"
);

// =================================================================================================
//     Normalize
// =================================================================================================

std::string to_lower(         std::string const& str );
void        to_lower_inplace( std::string&       str );
std::string to_upper(         std::string const& str );
void        to_upper_inplace( std::string&       str );

char        to_lower_ascii( char c );
char        to_upper_ascii( char c );

std::string to_lower_ascii(         std::string const& str );
void        to_lower_ascii_inplace( std::string&       str );
std::string to_upper_ascii(         std::string const& str );
void        to_upper_ascii_inplace( std::string&       str );

std::string escape  ( std::string const& text );
std::string deescape( std::string const& text );
char        deescape( char c );

// =================================================================================================
//     Output
// =================================================================================================

std::string repeat( std::string const& word, size_t times );

std::string to_string_precise( double value, const int precision = 6 );
std::string to_string_rounded( double value, const int precision = 6 );

/**
 * @brief Return a string representation of a given value.
 *
 * This function template is a drop-in replacement for std::to_string, with the difference that it
 * treats floating point numbers more nicely: Instead of printing a fixed amount of digits, it
 * only prints digits without trailing zeros.
 *
 * If you also want to round the value, see to_string_precise() and to_string_rounded().
 *
 * As it uses operator << on the given value, it is suitable for any class or value for which this
 * stream operator is available. Thus, this function can also be used for conveniently returning a
 * string where otherwise some stream operations would have been necessary.
 */
template <typename T>
std::string to_string( T const& v )
{
    std::ostringstream s;
    s << v;
    return s.str();
}

/**
 * @brief Return a string where the elements of a container `v` are joined using the string
 * `delimiter` in between them.
 *
 * The container is iterated via its range based for loop, thus it needs to have begin() and end()
 * functions.
 *
 * For turning the elements of the container into a string, their operator << is used.
 * Thus, this function can used with all types that support this operator.
 */
template <typename T>
std::string join( T const& v, std::string const& delimiter )
{
    std::ostringstream s;
    for( auto const& i : v ) {
        if( &i != &v[0] ) {
            s << delimiter;
        }
        s << i;
    }
    return s.str();
}

} // namespace utils
} // namespace genesis

#endif // include guard
