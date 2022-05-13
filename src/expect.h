#ifndef CTEST_EXPECT_H
#   define CTEST_EXPECT_H
#   define CTEST_EXPECT(CODE) \
        { \
            bool __ctest_works = false; \
            CODE; \
            if (!__ctest_works) fail(); \
        }
#   define CTEST_EXPECT_EXCEPTION(EXCEPTION, CODE) \
        { \
            try { \
                CODE; \
            } catch (EXCEPTION& e) { \
                __ctest_works = true; \
            } \
        }
#   ifdef  CTEST_NNS_EXPECT
#       define EXPECT           CTEST_EXPECT
#       define EXPECT_EXCEPTION CTEST_EXPECT_EXCEPTION
#   endif
#endif