#ifndef CTEST_TESTGROUP_H
#define CTEST_TESTGROUP_H

#include <utility>
#include <typeinfo>
#include <cxxabi.h>

#include "./meta.h"
#include "./test.h"

namespace ctest {
    class TestingResult;

    class TestInstance {
        public:
        Test* instance;
        char* name;
    };

    class TestGroup : public Test {
        public:
        unsigned int   testc;
        TestInstance** tests;

        TestGroup();
        ~TestGroup();

        bool           run_test(unsigned int id);
        TestingResult* run_all();

        template<class TEST> void queue() {
            queue<TEST>(new TEST());
        }

        template<class TEST> void queue(TEST* t) {
            TestInstance* test = new TestInstance();
            test->instance     = t;
            test->name         = abi::__cxa_demangle(typeid(TEST).name(), 0, 0, 0);
            tests[testc++]     = test;
        }

        bool run();
    };

    template<class, class>           class __group_of_int;
    template<class, class, class...> class group_of;

    template<class TYPE> concept __is_group_of
         = is_instantiation_of<__group_of_int, TYPE>::value
        || is_instantiation_of<group_of, TYPE>::value;

    template<class TEST, class NEXT> class __group_of_int : public TestGroup {
        public:
        template<class T = TEST, class N = NEXT> requires(!std::is_same<N, void>::value && !__is_group_of<N>) 
        static void group_of_queue(TestGroup* g) {
            g->queue<TEST>();
            g->queue<NEXT>();
        }

        template<class T = TEST, class N = NEXT> requires(!std::is_same<N, void>::value && __is_group_of<N>)
        static void group_of_queue(TestGroup* g) {
            g->queue<TEST>();
            NEXT::group_of_queue(g);
        }

        template<class T = TEST, class N = NEXT> requires(std::is_same<N, void>::value) 
        static void group_of_queue(TestGroup* g) {
            g->queue<TEST>();
        }

        __group_of_int() {
            group_of_queue(this);
        }
    };

    template<class TEST, class NEXT = void, class ...REST> 
        class group_of : public __group_of_int<TEST, NEXT> {};
    
    template<class TEST, class NEXT, class ...REST> requires(sizeof...(REST) > 0)
        class group_of<TEST, NEXT, REST...> : public __group_of_int<TEST, group_of<NEXT, REST...>> {};

    template<class TEST, class ...SHIT>
        class group_of<TEST, group_of<SHIT...>> : public __group_of_int<TEST, __group_of_int<group_of<SHIT...>, void>> {};
};

#include "./testcontroller.h"

#endif