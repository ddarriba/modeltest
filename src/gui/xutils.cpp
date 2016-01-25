#include "xutils.h"

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
