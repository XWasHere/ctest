#ifndef CTEST_TESTCONTROLLER_H
#define CTEST_TESTCONTROLLER_H

#include <typeinfo>
#include <cxxabi.h>

#include "./test.h"
#include "./testio.h"
#include "./testgroup.h"

namespace ctest {
    class TestGroup;

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

        template<class TEST> inline void queue() {
            root->add_child<TEST>();
        }

        inline void queue(Test* t) {
            root->add_child(t);
        }

        inline void queue(Test* t, char* name) {
            root->add_child(t, name);
        }
    };
};

#endif