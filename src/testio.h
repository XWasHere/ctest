#ifndef CTEST_TESTIO_H
#define CTEST_TESTIO_H

#include <stdio.h>
#include <stdarg.h>

namespace ctest {
    class Logger;

    class LoggerChannel {
        public:
        Logger* __logger;
        bool    __aalloc;
        FILE*   fd;
    };

    class Logger {
        public:
        LoggerChannel* info;
        LoggerChannel* err;

        unsigned int depth;

        Logger();

        LoggerChannel* open(FILE* fd, bool keep);
        int            close(LoggerChannel* c);

        int cprintf(LoggerChannel* channel, char* format, ...);
        int vcprintf(LoggerChannel* channel, char* format, va_list args);
    };
};

#endif