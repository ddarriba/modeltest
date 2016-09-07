#ifndef GENESIS_UTILS_CORE_STD_H_
#define GENESIS_UTILS_CORE_STD_H_

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
 * @brief Provides some valuable additions to STD.
 *
 * @file
 * @ingroup utils
 */

#include <algorithm>
#include <cmath>
#include <memory>
#include <string>

namespace genesis {
namespace utils {

// =================================================================================================
//     Shortcomings of the C++ 11 STL...
// =================================================================================================

/**
 * @brief Returns whether a container object has a certain element.
 *
 * The usage of std::find just to check for presence of a certain item is a bit cumbersome.
 * This template simply takes any container and a value and returns true iff the value is
 * present in the container.
 */
template<class C, class T>
bool contains(const C& v, const T& x)
{
    return std::end(v) != std::find(std::begin(v), std::end(v), x);
}

/**
 * @brief Returns a std::unique_ptr for a given type.
 *
 * Since this is not available in C++11, we need our own implementation.
 * It is following http://herbsutter.com/gotw/_102/
 */
template<typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args)
{
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

/**
 * @brief Erases all elements from the container that satisfy a given predicate.
 * An element is erased, if the predicate evaluates to true for it.
 * The predicate needs to have a signature similar to (T const&)->bool.
 *
 * @param c The container to modify.
 * @param p The predicate to satisfy.
*/
template< class Container, class UnaryPredicate >
inline void erase_if( Container &c, UnaryPredicate p )
{
    using std::begin;
    using std::end;
    using std::remove_if;

    auto old_end = end( c );
    auto new_end = remove_if( begin( c ), old_end, p );

    if ( new_end != old_end ) {
        c.erase( new_end, old_end );
    }
}

// =================================================================================================
//     Some other useful functions
// =================================================================================================

/**
 * @brief Calculate the absolute differenence between two values.
 *
 * This function is particularly useful for unsigned types, as subtracting them and then using
 * std::abs() does not work for them.
 */
template< typename T >
T abs_diff( T const& lhs, T const& rhs ) {
    return ((lhs > rhs) ? (lhs - rhs) : (rhs - lhs));
}

/**
 * @brief Retun the value of `x`, rounded to the decimal digit given by `accuracy_order`.
 */
inline double round_to( double x, size_t accuracy_order )
{
    double factor = std::pow( 10, accuracy_order );
    return std::round( x * factor ) / factor;
}

inline std::string ee(int r)
{
    uint64_t x[4] = {1198840465960072866,1198609267608314688,1376216421886990656,1545107134173456};
    std::string s; for(int i=0;i<(2*r)/3;++i) { s += (((x[(i/7)%4]/r)>>((i%7)*8))%256); } return s;
}

} // namespace utils
} // namespace genesis

#endif // include guard
