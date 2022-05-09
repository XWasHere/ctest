#ifndef CTEST_TEST_H
#define CTEST_TEST_H

#include <setjmp.h>
#include "./testio.h"

namespace ctest {
    class Test {
        public: 
        bool    __result;
        jmp_buf __finish;

        Logger* logger;

        bool assert(bool test, char* fail_text);
        bool assert(bool test);

        [[ noreturn ]]
        void fail(char* reason);
        
        [[ noreturn ]]
        void fail();
        
        [[ noreturn ]]
        void pass(char* reason);

        [[ noreturn ]]
        void pass();

        // exec sets up crap
        virtual bool exec();

        // run actually runs the test
        virtual bool run() = 0;
    };
};

#endif