#ifndef CTEST_TESTGROUP_H
#define CTEST_TESTGROUP_H

#include <utility>
#include <typeinfo>

#include "./meta.h"
#include "./test.h"

namespace ctest {
    class TestInstance;

    class TestInstance {
        public:
        Test* instance;
    };

    class TestGroup : public Test {
        public:
        unsigned int   testc;
        TestInstance** tests;

        TestGroup();
        ~TestGroup();

        bool           run_test(unsigned int id);
        TestingResult* run_all();

        template<class TEST> inline void queue() {
            add_child<TEST>();
        }

        template<class TEST> inline void queue(TEST* t) {
            add_child(t);
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
        inline static void group_of_queue(TestGroup* g) {
            g->queue<TEST>();
            g->queue<NEXT>();
        }

        template<class T = TEST, class N = NEXT> requires(!std::is_same<N, void>::value && __is_group_of<N>)
        inline static void group_of_queue(TestGroup* g) {
            g->queue<TEST>();
            NEXT::group_of_queue(g);
        }

        template<class T = TEST, class N = NEXT> requires(std::is_same<N, void>::value) 
        inline static void group_of_queue(TestGroup* g) {
            g->queue<TEST>();
        }

        inline __group_of_int() {
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