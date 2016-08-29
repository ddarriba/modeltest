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

#ifndef LOGGABLE_H
#define LOGGABLE_H

#include <iostream>

/**
 * @brief Loggable classes interface
 *
 * A loggable class must implement output_log and input_log.
 * The output format is arbitrary as long as the class knows how to load itself
 * from the same string. All attributes must be logged.
 */
class Loggable
{
public:
    virtual ~Loggable() {}

    /**
     * @brief Prints out the log
     * @param[in] out the output stream to print log to
     */
    virtual void output_log(std::ostream  &out) = 0;

    /**
     * @brief Loads a logfile from an input stream
     * @param[in] in the input stream to read log from
     */
    virtual void input_log(std::istream  &in) = 0;
};

#endif // LOGGABLE_H
