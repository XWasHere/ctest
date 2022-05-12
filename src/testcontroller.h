#ifndef CTEST_TESTCONTROLLER_H
#define CTEST_TESTCONTROLLER_H

#include <typeinfo>
#include <cxxabi.h>

#include "./test.h"
#include "./testio.h"
#include "./testgroup.h"

namespace ctest {
    class TestGroup;

    class TestingResult {
        public:
        unsigned int passedc;
        unsigned int failedc;
    };

    class TestController {
        public:
        LoggerChannel* out_info;
        LoggerChannel* out_err;
        Logger* logger;
        
        TestGroup* root;

        TestController();
        ~TestController();

        bool           run_test(unsigned int id);
        TestingResult* run_all();

        template<class TEST> void queue() {
            root->queue<TEST>();
        }

        template<class TEST> void queue(TEST* t) {
            root->queue<TEST>(t);
        }
    };
};

#endif