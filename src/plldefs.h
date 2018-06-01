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

#ifndef PLLDEFS_H_
#define PLLDEFS_H_

extern "C" {
#include <libpll/pll_tree.h>
#include <libpll/pll_msa.h>
#include <libpll/pllmod_common.h>
#include <libpll/pllmod_util.h>
#include <libpll/pll_optimize.h>
#include <libpll/pll_binary.h>
#include <libpll/pllmod_algorithm.h>

typedef unsigned int pll_index_t;
}

#endif /* PLLDEFS_H_ */
