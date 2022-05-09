#ifndef CTEST_TESTCONTROLLER_H
#define CTEST_TESTCONTROLLER_H

#include <typeinfo>
#include <cxxabi.h>

#include "./test.h"
#include "./testio.h"

namespace ctest {
    class TestingResult {
        public:
        unsigned int passedc;
        unsigned int failedc;
    };

    class TestInstance {
        public:
        Test* instance;
        char* name;
    };

    class TestController {
        public:
        LoggerChannel* out_info;
        LoggerChannel* out_err;
        Logger* logger;
        
        unsigned int   testc;
        TestInstance** tests;

        TestController();
        ~TestController();

        bool           run_test(unsigned int id);
        TestingResult* run_all();

        template<class TEST> void queue() {
            TestInstance* test = new TestInstance();
            test->instance     = new TEST();
            test->name         = abi::__cxa_demangle(typeid(TEST).name(), 0, 0, 0);
            tests[testc++]     = test;
        }
    };
};

#endif