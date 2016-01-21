#ifndef MYDEBUGSTREAM_H
#define MYDEBUGSTREAM_H

#include <iostream>
#include <streambuf>
#include <string>

#include <Qt/qtextbrowser.h>
#include <Qt/qstring.h>

class MyDebugStream : public QObject, public std::basic_streambuf<char>
{
Q_OBJECT
public:
    MyDebugStream(std::ostream &stream) : m_stream(stream)
    {
        m_old_buf = stream.rdbuf();
        stream.rdbuf(this);
    }
    ~MyDebugStream()
    {
        // output anything that is left
        if (!m_string.empty())
        {
            char * text = (char *) malloc(m_string.length() + 1);
            strcpy(text, m_string.c_str());
            emit  newText( text );
        }

        m_stream.rdbuf(m_old_buf);
    }

protected:
    virtual int_type overflow(int_type v)
    {
        if (v == '\n')
        {
            char * text = (char *) malloc(m_string.length() + 1);
            strcpy(text, m_string.c_str());
            emit  newText( text );
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
                char * text = (char *) malloc(m_string.length() + 1);
                strcpy(text, tmp.c_str());
                emit  newText( text );
                m_string.erase(m_string.begin(), m_string.begin() + pos + 1);
            }
        }
        return n;
    }

signals:
    void newText( char * text );

private:
    std::ostream &m_stream;
    std::streambuf *m_old_buf;
    std::string m_string;
};


#endif // MYDEBUGSTREAM_H
