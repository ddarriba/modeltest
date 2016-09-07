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
 * @brief Implementation of string utility functions.
 *
 * @file
 * @ingroup utils
 */

#include "genesis/std.h"
#include "genesis/string.h"

#include <algorithm>
#include <cctype>
#include <iomanip>
#include <sstream>
#include <stdexcept>

namespace genesis {
namespace utils {

// =================================================================================================
//     Compare
// =================================================================================================

/**
 * @brief Compare two strings case insensitive.
 */
bool equals_ci( std::string const& lhs, std::string const& rhs)
{
    const size_t sz = lhs.size();
    if( rhs.size() != sz ) {
        return false;
    }
    for( size_t i = 0; i < sz; ++i ) {
        if( tolower( lhs[i] ) != tolower( rhs[i] ) ) {
            return false;
        }
    }
    return true;
}

/**
 * @brief Return whether a string starts with another string.
 */
bool starts_with( std::string const & text, std::string const & start )
{
    if (start.size() > text.size()) {
        return false;
    }
    return std::equal( start.begin(), start.end(), text.begin() );
}

/**
 * @brief Return whether a string ends with another string.
 */
bool ends_with( std::string const & text, std::string const & ending )
{
    if (ending.size() > text.size()) {
        return false;
    }
    return std::equal( ending.rbegin(), ending.rend(), text.rbegin() );
}

// =================================================================================================
//     Substrings
// =================================================================================================

/**
 * @brief Return the first lines of the text.
 */
std::string head( std::string const& text, size_t lines )
{
    // Not totally efficient, but works for now.
    auto vec = split( text, "\n", false );
    size_t remove = vec.size() > lines ? vec.size() - lines : 0;
    vec.erase( vec.end() - remove, vec.end() );
    return join( vec, "\n" );
}

/**
 * @brief Return the last lines of the text.
 */
std::string tail( std::string const& text, size_t lines )
{
    // Not totally efficient, but works for now.
    auto vec = split( text, "\n", false );
    size_t remove = vec.size() > lines ? vec.size() - lines : 0;
    vec.erase( vec.begin(), vec.begin() + remove );
    return join( vec, "\n" );
}

// =================================================================================================
//     Find and Count
// =================================================================================================

/**
 * @brief Return the number of non-overlapping occurrences of a substring in a string.
 */
size_t count_substring_occurrences( std::string const& str, std::string const& sub )
{
    if (sub.length() == 0) {
        return 0;
    }

    size_t count = 0;
    for(
        size_t offset = str.find(sub);
        offset != std::string::npos;
        offset = str.find( sub, offset + sub.length() )
    ) {
        ++count;
    }

    return count;
}

/**
 * @brief Spilt a string int parts, given a set of delimiter chars.
 *
 * The string `str` is split using any of the chars in `delimiters` and returned as a vector
 * of strings. If `trim_empty` is set, empty strings resulting from adjacent delimiter chars are
 * excluded from the output.
 */
std::vector<std::string> split (
    std::string const& str,
    std::string const& delimiters,
    const bool trim_empty
) {
    size_t pos;
    size_t last_pos = 0;

    std::vector<std::string> result;

    while (true) {
        pos = str.find_first_of(delimiters, last_pos);

        if (pos == std::string::npos) {
           pos = str.length();

           if (pos != last_pos || !trim_empty) {
              result.push_back(std::string(str.data() + last_pos, pos - last_pos));
           }

           break;
        } else {
           if(pos != last_pos || !trim_empty) {
              result.push_back(std::string(str.data() + last_pos, pos - last_pos));
           }
        }

        last_pos = pos + 1;
    }

    return result;
}

// =================================================================================================
//     Manipulate
// =================================================================================================

/**
 * @brief Indent each line of `text` with `indentation` and return the result.
 *
 * By default, four spaces are used to indent. Whether the text ends with a new line or not is not
 * changed. Any trailing indentation chars are trimmed, in order to not have trailing whitespaces
 * in the result (except for the new line, if the text ends in one).
 */
std::string indent(
    std::string const& text,
    std::string const& indentation
) {
    auto ret = indentation + replace_all( text, "\n", "\n" + indentation );
    return trim_right( ret, indentation );
}

/**
 * @brief Return a copy of a string, where all occurrences of a search string
 * are replaced by a replace string.
 */
std::string replace_all (
    std::string const& text, std::string const& search, std::string const& replace
) {
    std::string tmp = text;
    for (size_t pos = 0; ; pos += replace.length()) {
        pos = tmp.find(search, pos);

        if (pos == std::string::npos)
            break;

        tmp.erase(pos, search.length());
        tmp.insert(pos, replace);
    }
    return tmp;
}

// inline version
/*
void replace_all(
    std::string &s, const std::string &search, const std::string &replace
) {
    for (size_t pos = 0; ; pos += replace.length() ) {
        pos = s.find(search, pos);

        if (pos == string::npos)
            break;

        s.erase(pos, search.length());
        s.insert(pos, replace);
    }
}
*/

/**
 * @brief Replace all occurrences of the `search_chars` in `text` by the `replace` char.
 */
std::string replace_all_chars (
    std::string const& text,
    std::string const& search_chars,
    char               replace
) {
    auto result = text;
    for( auto& c : result ) {
        if( search_chars.find( c ) != std::string::npos ) {
            c = replace;
        }
    }
    return result;
}

/**
 * @brief Return a copy of the input string, with left trimmed white spaces.
 */
std::string trim_right (
    std::string const& s,
    std::string const& delimiters
) {
    auto const pos = s.find_last_not_of(delimiters);
    if( std::string::npos == pos ) {
        return "";
    } else {
        return s.substr( 0, pos + 1 );
    }
}

/**
 * @brief Return a copy of the input string, with right trimmed white spaces.
 */
std::string trim_left (
    std::string const& s,
    std::string const& delimiters
) {
    auto const pos = s.find_first_not_of(delimiters);
    if( std::string::npos == pos ) {
        return "";
    } else {
        return s.substr(pos);
    }
}

/**
 * @brief Return a copy of the input string, with trimmed white spaces.
 */
std::string trim (
    std::string const& s,
    std::string const& delimiters
) {
    return trim_left(trim_right(s, delimiters), delimiters);
}

// =================================================================================================
//     Normalize
// =================================================================================================

/**
 * @brief Return an all-lowercase copy of the given string, locale-aware.
 */
std::string to_lower( std::string const& str )
{
    auto res = str;
    for( auto& c : res ){
        c = tolower(c);
    }
    return res;
}

/**
 * @brief Turn the given string to all-lowercase, locale-aware.
 */
void to_lower_inplace( std::string& str )
{
    for( auto& c : str ){
        c = tolower(c);
    }
}

/**
 * @brief Return an all-uppercase copy of the given string, locale-aware.
 */
std::string to_upper( std::string const& str )
{
    auto res = str;
    for( auto& c : res ){
        c = toupper(c);
    }
    return res;
}

/**
 * @brief Turn the given string to all-uppercase, locale-aware.
 */
void to_upper_inplace( std::string& str )
{
    for( auto& c : str ){
        c = toupper(c);
    }
}

/**
 * @brief Return the lower case of a given char, ascii-only.
 */
char to_lower_ascii( char c )
{
    return ( 'A' <= c && c <= 'Z' ) ? c + 0x20 : c;
}

/**
 * @brief Return the upper case of a given char, ascii-only.
 */
char to_upper_ascii( char c )
{
    return ( 'a' <= c && c <= 'z' ) ? c - 0x20 : c;
}

/**
 * @brief Return an all-lowercase copy of the given string, ascii-only.
 */
std::string to_lower_ascii( std::string const& str )
{
    auto res = str;
    for( auto& c : res ){
        c = to_lower_ascii(c);
    }
    return res;
}

/**
 * @brief Turn the given string to all-lowercase, ascii-only.
 */
void to_lower_ascii_inplace( std::string& str )
{
    for( auto& c : str ){
        c = to_lower_ascii(c);
    }
}

/**
 * @brief Return an all-uppercase copy of the given string, ascii-only.
 */
std::string to_upper_ascii( std::string const& str )
{
    auto res = str;
    for( auto& c : res ){
        c = to_upper_ascii(c);
    }
    return res;
}

/**
 * @brief Turn the given string to all-uppercase, ascii-only.
 */
void to_upper_ascii_inplace( std::string& str )
{
    for( auto& c : str ){
        c = to_upper_ascii(c);
    }
}

/**
 * @brief Return a string where special chars are replaces by their escape sequence.
 *
 * All new lines are transformed into either \\r or \\n, tabs into \\t.
 * Double quotation marks are preceeded by a backslash, also the backslash itself will be escaped,
 * so that `"` becomes `\"` and `\` becomes `\\`.
 */
std::string escape( std::string const& text )
{
    // This is slow-ish, because the string is iterated multiple times. Could be done faster.
    std::string tmp;
    tmp = replace_all( text, "\r", "\\r"  );
    tmp = replace_all( tmp,  "\n", "\\n"  );
    tmp = replace_all( tmp,  "\t", "\\t"  );
    tmp = replace_all( tmp,  "\"", "\\\"" );
    tmp = replace_all( tmp,  "\\", "\\\\" );
    return tmp;
}

/**
 * @brief Return a string where backslash-escaped characters are transformed into
 * their respective string form.
 *
 * All occurrences of `backslash + char` in the string are de-escaped. That is, all `\n`, `\t` and
 * `\r` are turned into their respective control sequences, while all other chars folloing a
 * backslash are translated into the char itself (so that e.g., quotation marks or backslashes
 * themself can be escaped).
 *
 * Also see deescape( char c ).
 */
std::string deescape( std::string const& text )
{
    // Prepare a string that might be a bit too big, but saves reallocation.
    std::string tmp;
    tmp.reserve( text.size() );

    // Copy from text to tmp string, while deescaping.
    for( size_t i = 0; i < text.size(); ++i ) {
        if( text[ i ] == '\\' ) {
            if( i + 1 >= text.size() ){
                break;
            }

            tmp += deescape( text[ i + 1 ] );
            ++i;
        } else {
            tmp += text[ i ];
        }
    }
    return tmp;
}

/**
 * @brief Return the de-escaped char for a backslash-escaped char.
 *
 * The function takes the char that follows a backslash in an escaped string and returns its
 * de-escaped char. That is, `n` is turned into a new line (`\n`), `t` is turned into a tab (`\t`)
 * and `r` is turned into a carrier return (`\r`). All other chars (e.g., quotation marks or
 * the backslash itself) are simply returned as-is.
 */
char deescape( char c )
{
    switch( c ) {
        case 'r' :
            return '\r';

        case 'n' :
            return '\n';

        case 't' :
            return '\t';

        default :
            return c;
    }
}

// =================================================================================================
//     Output
// =================================================================================================

/**
 * @brief Take a string and repeat it a given number of times.
 */
std::string repeat( std::string const& word, size_t times )
{
    // Init and avoid repeated reallocation.
    std::string result;
    result.reserve( times * word.length() );

    // Concat repeats.
    for( size_t i = 0; i < times; ++i ) {
        result += word ;
    }
    return result;
}

/**
 * @brief Return a precise string representation of the input value, using the provided precision
 * value (determining its decimal places).
 *
 * This function rounds the value to the given precision, and then returns its string representation
 * with possible trailing zeros. Thus, it uses fixed precision. This is useful for e.g., output
 * in a table format.
 *
 * For a version of this function that truncates trailing zeros, see to_string_rounded().
 * Also, use to_string() if you do not want to round the value at all.
 */
std::string to_string_precise( double value, const int precision )
{
    std::ostringstream out;
    out << std::fixed << std::setprecision(precision) << value;
    return out.str();
}

/**
 * @brief Return a string representation of the input value, using the provided precision value
 * (determining its decimal places) to round, and truncate trailing zeros.
 *
 * This function rounds the value to the given precision, and then returns its string representation
 * without trailing zeros. This is useful for output that keeps a certain amount of significant
 * decimal digits, while making the output as short as possible.
 *
 * If you want to round, but also keep trailing zeros, see to_string_precise().
 * Also, use to_string() if you do not want to round the value at all.
 */
std::string to_string_rounded( double value, const int precision )
{
    std::ostringstream s;
    s << utils::round_to( value, precision);
    return s.str();
}

} // namespace utils
} // namespace genesis
