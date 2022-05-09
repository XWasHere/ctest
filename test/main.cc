#include <ctest.h>

using namespace ctest;

class PassTest : public Test {
    public:
    class Test : public ctest::Test {
        public:
        bool run() {
            pass();
        }
    };

    bool run() {
        Test test = Test();

        if (test.exec()) {
            pass();
        } else {
            fail();
        }
    }
};

class FailTest : public Test {
    public:
    class Test : public ctest::Test {
        public:
        bool run() {
            fail();
        }
    };

    bool run() {
        Test test = Test();

        if (test.exec()) {
            fail();
        } else {
            pass();
        }
    }
};

class QueueTest : public Test {
    public:
    class Test1 : public ctest::Test {
        public:
        bool run() {
            pass();
        }
    };

    class Test2 : public ctest::Test {
        public:
        bool run() {
            fail();
        }
    };

    bool run() {
        TestController tc = TestController();
        tc.logger->depth = logger->depth;

        tc.queue<Test1>();
        tc.queue<Test2>();

        TestingResult* res = tc.run_all();

        assert(res->failedc == 1, "1 test should have failed");
        assert(res->passedc == 1, "1 test should have passed");
        
        pass();
    }
};

class AssertTest : public Test {
    public:
    class PassTest : public ctest::Test {
        public:
        class Test : public ctest::Test {
            public:
            bool run() {
                assert(true, "if youre seeing this the thing broke");
                pass();
            }
        };

        bool run() {
            TestController tc = TestController();
            tc.logger->depth = logger->depth;

            tc.queue<Test>();

            TestingResult* res = tc.run_all();

            assert(res->passedc == 1, "the test should have passed");

            pass();
        }
    };
    
    class FailTest : public ctest::Test {
        public:
        class Test : public ctest::Test {
            public:
            bool run() {
                assert(false, "if youre seeing this the thing worked");
                pass();
            }
        };
        
        bool run() {
            TestController tc = TestController();
            tc.logger->depth = logger->depth;

            tc.queue<Test>();

            TestingResult* res = tc.run_all();

            assert(res->failedc == 1, "the test should have failed");
            
            pass();
        }
    };

    bool run() {
        TestController tc = TestController();
        tc.logger->depth = logger->depth;

        tc.queue<PassTest>();
        tc.queue<FailTest>();

        TestingResult* res = tc.run_all();

        assert(res->failedc == 0);

        pass();
    }
};

int main() {
    TestController tc = TestController();

    tc.queue<PassTest>();
    tc.queue<FailTest>();
    tc.queue<QueueTest>();
    tc.queue<AssertTest>();

    tc.run_all();
}