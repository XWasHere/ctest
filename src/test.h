#ifndef CTEST_TEST_H
#define CTEST_TEST_H

#include <type_traits>
#include <typeinfo>
#include <functional>
#include <setjmp.h>
#include "./testio.h"

namespace ctest {
    class Test;

    class TestingResult {
        public:
        Test* test;

        bool passed;
        int  result;
        
        unsigned int passedc;
        unsigned int failedc;
    };

    template<template<class OWNER, class ...> class FOR, class NEXT_TYPE, class ...CARGS> class ExpectClause;

    template<class OWNER> concept OWNERFINAL = std::is_same<typename OWNER::next_t, void>::value;

    template<class OWNER> class ExpectCheckNoop {
        public:
        OWNER* cl;

        template<class O = OWNER> requires(!OWNERFINAL<OWNER>) bool check(std::function<void()> e) {
            if (cl->next) {
                return cl->next->check->check(e);
            } else {
                e();
                return false;
            }
        }

        template<class O = OWNER> requires(OWNERFINAL<OWNER>) bool check(std::function<void()> e) {
            e();
            return false;
        }
    };

    template<class OWNER, class EX> class ExpectCheckException {
        public:
        OWNER* cl;

        template<class O = OWNER> requires(!OWNERFINAL<OWNER>) bool check(std::function<void()> e) {
            if (cl->next) {
                try {
                    return cl->next->check->check(e);
                } catch (EX& ex) {
                    return true;
                }
            } else {
                try {
                    e();
                    return false;
                } catch (EX& ex) {
                    return true;
                }
            }
        }

        template<class O = OWNER> requires(OWNERFINAL<OWNER>) bool check(std::function<void()> e) {
            try {
                e();
                return false;
            } catch (EX& ex) {
                return true;
            }
        }
    };
    
    template<template<class OWNER, class ...> class FOR, class NEXT_TYPE, class ...CARGS> class ExpectClause {
        public:
        typedef NEXT_TYPE next_t;

        Test*                        test;
        FOR<ExpectClause, CARGS...>* check;
        NEXT_TYPE*                   next;

        ExpectClause(Test* t) {
            next      = 0;
            check     = new FOR<ExpectClause, CARGS...>();
            check->cl = this;
            test      = t;
        }

        template<class EX> ExpectClause<ExpectCheckException, ExpectClause, EX>* exception() {
            ExpectClause<ExpectCheckException, ExpectClause, EX>* c = new ExpectClause<ExpectCheckException, ExpectClause, EX>(test);
            c->next = this;
            return c;
        }

        void in(std::function<void()> e) {
            if (!check->check(e)) test->fail();
        }
    };

    class Test {
        public: 
        bool    __passed;
        int     __result;
        jmp_buf __finish;

        // embedded programming nightmare
        unsigned int __childlen;
        Test**       __children;

        char* name;

        TestingResult* result;

        Logger* logger;

        Test();
        ~Test();

        void __children_extend(unsigned int count);

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

        Test* operator[](char* child);
        
        // dear god
#ifndef WIGNORE_CTEST_EXPECT_OVERHEAD
        [[ deprecated("expect has a LOT of overhead, CTEST_EXPECT should be used instead") ]]
#endif
        ExpectClause<ExpectCheckNoop, void>* expect();

        // runs a child by either a pointer or an index
        TestingResult* run_child(Test*        test);
        TestingResult* run_child(unsigned int test);
        
        // exec sets up crap and runs the test
        virtual TestingResult* exec();

        // run just runs the test
        virtual bool run() = 0;

        // add a child test
        template<class TEST> void add_child()           { add_child(new TEST()); }
        template<class TEST> void add_child(char* name) { add_child(new TEST(), name); }
        void add_child(Test* test);
        void add_child(Test* test, char* name);
    };
};

#endif