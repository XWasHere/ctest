#include <utility>
#include <functional>

#include "./test.h"

namespace ctest {
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
        __result = 0;
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
        __result = 1;
        longjmp(__finish, 1);
#ifdef __cpp_lib_unreachable
        unreachable();
#endif
    }

    bool Test::exec() {
        bool res;
        unsigned int logger_depth;
        
        if (logger) logger_depth = logger->depth;

        if (setjmp(__finish) == 1) {
            res = __result;
        } else {
            res = run();
        }

        if (logger) logger->depth = logger_depth;
        
        return res;
    }
};