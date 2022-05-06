#include <ctest.h>

using namespace ctest;

class PassTest : public Test {
    public:
    class Test : public ctest::Test {
        public:
        bool run() {
            return true;
        }
    };

    bool run() {
        Test test = Test();

        if (test.run()) {
            return true;
        } else {
            return false;
        }
    }
};

class FailTest : public Test {
    public:
    class Test : public ctest::Test {
        public:
        bool run() {
            return false;
        }
    };

    bool run() {
        Test test = Test();

        if (test.run()) {
            return false;
        } else {
            return true;
        }
    }
};

int main() {
    TestController tc = TestController();

    tc.queue<PassTest>();
    tc.queue<FailTest>();

    tc.run_all();
}