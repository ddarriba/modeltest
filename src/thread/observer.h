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

#ifndef OBSERVER_H
#define OBSERVER_H

#include <vector>

class Observable;

class Observer
{
  public:
    Observer();
    virtual ~Observer();
    virtual void update(Observable * subject, void * data) = 0;
};

class Observable
{
    std::vector < Observer * > views;
  public:
    virtual ~Observable();
    void attach(Observer *obs);
    virtual void notify(void * data = 0);
};

#endif // OBSERVER_H
