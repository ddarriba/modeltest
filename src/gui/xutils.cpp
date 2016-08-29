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

#include "gui/xutils.h"

QString xutils::to_qstring(const char * msg, msg_level_id level)
{
    QString line = msg;
    QString endHtml = "</font>";

    switch(level)
    {
        case msg_lvl_error: line = "<font color=\"Red\">" + line + "</font>"; break;
        case msg_lvl_alert: line = "<font color=\"DeepPink\">" + line + "</font>"; break;
        case msg_lvl_notify: line = "<font color=\"DarkGreen\">" + line + "</font>"; break;
        case msg_lvl_info: line  = "<font color=\"Black\">" + line + "</font>"; break;
        default: break;
    }

    line = line + endHtml;
    return line;
}
