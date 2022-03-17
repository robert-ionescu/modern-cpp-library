#include "log.h"

unsigned& log::_loglevel()
{
    static unsigned _ll_internal = LOG_DEFAULT;
    return _ll_internal;
}

inline void log::set_log_level(unsigned ll)
{
    _loglevel() = ll;
}
