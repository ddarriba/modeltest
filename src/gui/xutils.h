#ifndef XUTILS_H
#define XUTILS_H

#include <QString>

enum msg_level_id {
    msg_lvl_error,
    msg_lvl_alert,
    msg_lvl_notify,
    msg_lvl_info
};

class xutils
{
public:
    static QString to_qstring(const char * msg, msg_level_id level);
};

#endif // XUTILS_H
