#include <typeinfo>
#include <utility>
#include <functional>
#include <cxxabi.h>

#include "./test.h"

namespace ctest {
    Test::Test() {
        this->name = 0;

        this->result = 0;

        this->__childlen = 0;
        this->__children = 0;
    }

    Test::~Test() {
        if (this->result)     delete this->result;
        if (this->__children) free(this->__children);
    }

    ExpectClause<ExpectCheckNoop, void>* Test::expect() {
        return new ExpectClause<ExpectCheckNoop, void>(this);
    }

    bool Test::assert(bool test, char* reason) {
        if (!test) {
            logger->cprintf(logger->err, "assertion failed: %s\n", reason);
            fail();
#ifdef __cpp_lib_unreachable
            unreachable();
#endif
        }
        return true;
    }

    bool Test::assert(bool test) {
        if (!test) {
            logger->cprintf(logger->err, "assertion failed: no reason given\n");
            fail();
#ifdef __cpp_lib_unreachable
            unreachable();
#endif
        }
        return true;
    }

    [[ noreturn ]]
    void Test::fail(char* reason) {
        logger->cprintf(logger->err, reason);
        fail();
#ifdef __cpp_lib_unreachable
        unreachable();
#endif
    }

    [[ noreturn ]]
    void Test::fail() {
        __passed = false;
        __result = -1;
        longjmp(__finish, 1);
#ifdef __cpp_lib_unreachable
        unreachable();
#endif
    }

    [[ noreturn ]]
    void Test::pass(char* reason) {
        logger->cprintf(logger->info, reason);
        pass();
#ifdef __cpp_lib_unreachable
        unreachable();
#endif
    }

    [[ noreturn ]]
    void Test::pass() {
        __passed = true;
        __result = 0;
        longjmp(__finish, 1);
#ifdef __cpp_lib_unreachable
        unreachable();
#endif
    }

    TestingResult* Test::run_child(Test* test) {
        logger->cprintf(logger->info, "run test \"%s\"%>\n", test->name);
        
        test->logger = logger;
        TestingResult* res = test->exec();

        if (res && res->passed) {
            logger->cprintf(logger->info, "test passed%<\n");
        } else {
            logger->cprintf(logger->err, "test failed%<\n");
        }

        return res;
    }

    TestingResult* Test::run_child(unsigned int test) {
        if (test >= 0 && test < __childlen) {
            return run_child(__children[test]);
        } else return 0;
    }

    void Test::__children_extend(unsigned int length) {
        if (__childlen < length) {
            unsigned long size = sizeof(*__children) * (length - length % 32 + 32);
//            printf("%i\n", size);

            if (__children == 0) {
                __children = malloc(size);
            } else __children = realloc(__children, size);
        }
    }

    void Test::add_child(Test* test) {
        add_child(test, abi::__cxa_demangle(typeid(*test).name(), 0, 0, 0));
    }

    void Test::add_child(Test* test, char* name) {
        __children_extend(__childlen + 1);

        if (!test->name) test->name = name;
        __children[__childlen++] = test;
    }

    TestingResult* Test::exec() {
        TestingResult* res = new TestingResult();
        unsigned int logger_depth;
        
        if (logger) logger_depth = logger->depth;

        if (setjmp(__finish) == 1) {
            res->passed = __passed;
            res->result = __result;
        } else {
            res->result = run();
            res->passed = true;
        }

        if (logger) logger->depth = logger_depth;
        
        this->result = res;

        return res;
    }
};