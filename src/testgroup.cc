#include "./testcontroller.h"
#include "./testgroup.h"

namespace ctest {
    TestGroup::TestGroup() {
        testc = 0;
        tests = malloc(sizeof(void*) * 16);
    }

    TestGroup::~TestGroup() {
        for (int i = 0; i < testc; i++) {
            if (tests[i]) delete tests[i];
        }

        free(tests);
    }

    bool TestGroup::run_test(unsigned int id) {
        TestInstance* instance = this->tests[id];

        logger->cprintf(logger->info, "run test \"%s\"%>\n", instance->instance->name);

        instance->instance->logger = logger;
        bool result = instance->instance->exec();

        if (result) {
            logger->cprintf(logger->info, "test passed%<\n");
        } else {
            logger->cprintf(logger->err, "test failed%<\n");
        }

        return result;
    }

    TestingResult* TestGroup::run_all() {
        unsigned int passed = 0;
        unsigned int failed = 0;

        if (__childlen == 0) {
            logger->cprintf(logger->err, "nothing to run, canceling\n");
            return 0;
        }

        for (int i = 0; i < __childlen; i++) {
            TestingResult* res = run_child(i);

            if (res->passed) passed++;
            else             failed++;

            delete res;
        }

        if (passed == 0) {
            logger->cprintf(logger->info, "%i %s failed%<\n", failed, failed == 1 ? "test" : "tests");
        } else if (failed == 0) {
            logger->cprintf(logger->info, "%i %s passed%<\n", passed, passed == 1 ? "test" : "tests");
        } else {
            logger->cprintf(logger->info, "%i %s passed. %i %s failed%<\n", 
                passed, 
                passed == 1 ? "test" : "tests", 
                failed,
                failed == 1 ? "test" : "tests");
        }

        TestingResult* res = new TestingResult();
        res->passedc = passed;
        res->failedc = failed;

        return res;
    }

    bool TestGroup::run() {
        TestingResult* res = run_all();
        
        if (!res)         fail();
        if (res->failedc) fail();
        
        pass();
    }
};