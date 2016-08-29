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

#ifndef Q_DEBUGSTREAM_H_
#define Q_DEBUGSTREAM_H_

#include <iostream>
#include <streambuf>
#include <string>

#ifdef QT_WIDGETS_LIB
#include <QtWidgets>
#else
#include <Qt/qtextbrowser.h>
#endif

class Q_DebugStream : public std::basic_streambuf<char>
{
public:
    Q_DebugStream(std::ostream &stream, QTextBrowser* text_edit) : m_stream(stream)
    {
        log_window = text_edit;
        m_old_buf = stream.rdbuf();
        stream.rdbuf(this);
    }
    ~Q_DebugStream()
    {
        // output anything that is left
        if (!m_string.empty())
            log_window->append(m_string.c_str());

        m_stream.rdbuf(m_old_buf);
    }

protected:
    virtual int_type overflow(int_type v)
    {
        if (v == '\n')
        {
            log_window->append(m_string.c_str());
            m_string.erase(m_string.begin(), m_string.end());
        }
        else
            m_string += v;
        return v;
    }

    virtual std::streamsize xsputn(const char *p, std::streamsize n)
    {
        m_string.append(p, p + n);

        size_t pos = 0;
        while (pos != std::string::npos)
        {
            pos = m_string.find('\n');
            if (pos != std::string::npos)
            {
                std::string tmp(m_string.begin(), m_string.begin() + pos);
                log_window->append(tmp.c_str());
                m_string.erase(m_string.begin(), m_string.begin() + pos + 1);
            }
        }
        return n;
    }

private:
    std::ostream &m_stream;
    std::streambuf *m_old_buf;
    std::string m_string;
    QTextBrowser* log_window;
};



#endif
