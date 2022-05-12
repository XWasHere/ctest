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

class ExpectTest : public Test {
    public:
    class ExpectException : public ctest::Test {
        public:
        class Exception1 : public std::exception {};
        class Exception2 : public std::exception {};

        class Fails : public ctest::Test {
            public:
            bool run() {
                expect()->exception<Exception1>()->in([]{
                    
                });

                pass();
            }
        };

        class Passes : public ctest::Test {
            public:
            bool run() {
                expect()->exception<Exception1>()->in([]{
                    throw Exception1();
                });

                pass();
            }
        };

        class WouldCrash : public ctest::Test {
            public:
            bool run() {
                try {
                    expect()->exception<Exception1>()->in([]{
                        throw Exception2();
                    });
                } catch (Exception2& e) {
                    logger->cprintf(logger->info, "\"fake crash\"\n");
                }

                pass();
            }
        };

        class Multi11 : public ctest::Test {
            public:
            bool run() {
                expect()->exception<Exception1>()->exception<Exception2>()->in([]{
                    throw Exception1();
                });

                pass();
            }
        };

        class Multi12 : public ctest::Test {
            public:
            bool run() {
                expect()->exception<Exception1>()->exception<Exception2>()->in([]{
                    throw Exception2();
                });

                pass();
            }
        };

        class Multi21 : public ctest::Test {
            public:
            bool run() {
                expect()->exception<Exception2>()->exception<Exception1>()->in([]{
                    throw Exception1();
                });

                pass();
            }
        };

        class Multi22 : public ctest::Test {
            public:
            bool run() {
                expect()->exception<Exception2>()->exception<Exception1>()->in([]{
                    throw Exception2();
                });

                pass();
            }
        };

        bool run() {
            TestController tc = TestController();
            tc.logger->depth = logger->depth;

            tc.queue<Fails>();
            tc.queue<Passes>();
            tc.queue<WouldCrash>();
            tc.queue<Multi11>();
            tc.queue<Multi12>();
            tc.queue<Multi21>();
            tc.queue<Multi22>();

            TestingResult* res = tc.run_all();

            assert(res->failedc == 1);

            pass();    
        }
    };

    bool run() {
        TestController tc = TestController();
        tc.logger->depth = logger->depth;

        tc.queue<ExpectException>();

        TestingResult* res = tc.run_all();

        assert(res->failedc == 0);

        pass();
    }
};

class GroupTest : public Test {
    public:
    class Group111 : public Test {
        public:
        bool run() {
            pass();
        }
    };
    class Group211 : public Test {
        public:
        bool run() {
            fail();
        }
    };
    class Group221 : public Test {
        public:
        bool run() {
            pass();
        }
    };
    class Group311 : public Test {
        public:
        bool run() {
            fail();
        }
    };
    class Group312 : public Test {
        public:
        bool run() {
            pass();
        }
    };
    class Group313 : public Test {
        public:
        bool run() {
            fail();
        }
    };
    class Group321 : public Test {
        public:
        bool run() {
            pass();
        }
    };
    class Group322 : public Test {
        public:
        bool run() {
            fail();
        }
    };
    class Group323 : public Test {
        public:
        bool run() {
            pass();
        }
    };
    class Group331 : public Test {
        public:
        bool run() {
            fail();
        }
    };
    class Group332 : public Test {
        public:
        bool run() {
            pass();
        }
    };
    class Group333 : public Test {
        public:
        bool run() {
            fail();
        }
    };

    bool run() {
        TestController tc = TestController();
        tc.logger->depth = logger->depth;

        tc.queue<group_of<Group111>>();
        tc.queue<group_of<Group211, Group221>>();
        tc.queue<group_of<
            group_of<Group311, Group312, Group313>,
            group_of<Group321, Group322, Group323>,
            group_of<Group331, Group332, Group333>
        >>();

        tc.run_all();

        pass(); // unconditionally pass, i have no idea how to test that this works beyond sigsegv
    }
};

int main() {
    TestController tc = TestController();

    tc.queue<PassTest>();
    tc.queue<FailTest>();
    tc.queue<QueueTest>();
    tc.queue<AssertTest>();
    tc.queue<ExpectTest>();
    tc.queue<GroupTest>();

    tc.run_all();
}