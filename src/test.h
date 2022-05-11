#ifndef CTEST_TEST_H
#define CTEST_TEST_H

#include <type_traits>
#include <functional>
#include <setjmp.h>
#include "./testio.h"

namespace ctest {
    class Test;

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

        // dear god
        ExpectClause<ExpectCheckNoop, void>* expect();

        // exec sets up crap
        virtual bool exec();

        // run actually runs the test
        virtual bool run() = 0;
    };
};

#endif