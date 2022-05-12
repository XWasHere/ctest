#include <stdlib.h>
#include <stdio.h>

#include "./testcontroller.h"
#include "./testio.h"

namespace ctest {
    TestController::TestController() {
        root = new TestGroup();

        logger       = new Logger();
        logger->info = logger->open(stdout, 1);
        logger->err  = logger->open(stdout, 1);

        root->logger = logger;
    }

    TestController::~TestController() {
        delete root;

        logger->close(logger->err);
        delete logger->err;
        logger->close(logger->info);
        delete logger->info;

        delete logger;
    }

    bool TestController::run_test(unsigned int id) {
        return root->run_test(id);
    }

    TestingResult* TestController::run_all() {
        logger->cprintf(logger->info, "running all tests%>\n");
        return root->run_all();
    }
}