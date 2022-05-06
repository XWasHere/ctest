#ifndef CTEST_TEST_H
#define CTEST_TEST_H

#include "./testio.h"

namespace ctest {
    class Test {
        public: 
        Logger* logger;
        
        virtual bool run() = 0;
    };
};

#endif