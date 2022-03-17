#pragma once
#include <iostream>
#include <vector>
#include <ctime>

#define LOG_SILENT 0
#define LOG_ERR 1
#define LOG_ERROR 1
#define LOG_WARN 2
#define LOG_WARNING 2
#define LOG_INFO 3
#define LOG_TIME 4
#define LOG_DEBUG 5
#define LOG_DEFAULT 4

#define SLOG_TIME "[ TIME    ]"
#define SLOG_DEBUG "[ DEBUG   ]"
#define SLOG_ERROR "[ ERROR   ]"
#define SLOG_WARNING "[ WARNING ]"
#define SLOG_INFO "[ INFO    ]"



class log
{
public:
    enum class Priority {
        Informational,
        Low,
        Medium,
        High,
        Critical
    };
    static unsigned& _loglevel();
    inline void set_log_level(unsigned ll);

private:
    time_t _now;
    time_t _start;
    std::vector<time_t> _snaps;
    std::vector<std::string> _snap_ns;
    unsigned _message_level;
    std::ostream& _fac;
    std::string _name;
};
