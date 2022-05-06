#include <stdlib.h>
#include <stdio.h>

#include "./testcontroller.h"
#include "./testio.h"

namespace ctest {
    TestController::TestController() {
        testc = 0;
        tests = malloc(sizeof(void*) * 16);

        logger       = new Logger();
        logger->info = logger->open(stdout, 1);
        logger->err  = logger->open(stdout, 1);
    }

    TestController::~TestController() {
        for (int i = 0; i < testc; i++) {
            if (tests[i]) delete tests[i];
        }

        free(tests);
        
        logger->close(logger->err);
        delete logger->err;
        logger->close(logger->info);
        delete logger->info;

        delete logger;
    }

    bool TestController::run_test(unsigned int id) {
        TestInstance* instance = this->tests[id];

        logger->cprintf(logger->info, "run test \"%s\"%>\n", instance->name);

        bool result = instance->instance->run();

        if (result) {
            logger->cprintf(logger->info, "test passed%<\n");
        } else {
            logger->cprintf(logger->err, "test failed%<\n");
        }

        return result;
    }

    void TestController::run_all() {
        unsigned int passed;
        unsigned int failed;

        if (testc == 0) {
            logger->cprintf(logger->err, "nothing to run, canceling");
            return;
        }

        logger->cprintf(logger->info, "running all tests%>\n");

        for (int i = 0; i < testc; i++) {
            if (run_test(i)) passed++;
            else             failed++;
        }

        if (passed == 0) {
            logger->cprintf(logger->info, "%i %s failed%<\n", failed, failed == 1 ? "tests" : "test");
        } else if (failed == 0) {
            logger->cprintf(logger->info, "%i %s passed%<\n", passed, passed == 1 ? "tests" : "test");
        } else {
            logger->cprintf(logger->info, "%i %s passed. %i %s failed%<\n", passed, failed);
        }
    }
}