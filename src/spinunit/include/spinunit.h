#ifndef SPINUNIT_H
#define SPINUNIT_H

// Unit test framework written for Spinel
// Test cases are defined as being functions
// that take no arguments and return a fail message, or NULL on success
#include <stdint.h>
#include <stdio.h>
#include <string.h>

const char* const SpinUnitTestMsg = "Running test %lu in %s...\x1b[74G[%s]\r";
const char* const SpinUnitRunning = "\x1b[1;33m....\x1b[0m";
const char* const SpinUnitPassed = "\x1b[32mpass\x1b[0m";
const char* const SpinUnitFailed = "\x1b[31mfail\x1b[0m";
const char* const SpinUnitStatsMsg = 
    "\nFinished.\n%lu tests run\n%lu tests passed\n%lu tests failed\n";

typedef char*(*SpinUnitTest)(void);

// Used in a test case: if expr is not true, fail the test with failMsg
#define SpinUnitAssert(expr, failMsg) do {\
    if (!(expr)) {\
        return failMsg;\
    }\
} while (0)

// Declare a unit test group. Please place this in global scope.
// name should be suitable for use as a variable name, but also readable
#define SpinUnitTestGroup(name, ...) \
const SpinUnitTest __##name[] = {\
    __VA_ARGS__, NULL\
}

// Include headers and define variables needed for the test runner
// This should go in the global scope, visible to the runner
#define SpinUnitRunnerDecls static uint64_t testsRun = 0, testsPassed = 0

// Run the tests in this test group
// This should go in the test runner's main function
// Prerequisite: SpinUnitRunnerDecls
#define SpinUnitRunTests(name) do {\
    extern const SpinUnitTest __##name[];\
    for (size_t __i = 0; __##name[__i] != NULL; __i++) {\
        testsRun++;\
        printf(SpinUnitTestMsg, __i + 1, #name, SpinUnitRunning);\
        char* res = __##name[__i]();\
        if (res != NULL) {\
            printf(SpinUnitTestMsg, __i + 1, #name, SpinUnitFailed);\
            printf("\n");\
            printf(res);\
            printf("\n");\
        } else {\
            printf(SpinUnitTestMsg, __i + 1, #name, SpinUnitPassed);\
            printf("\n");\
            testsPassed++;\
        }\
    }\
} while (0)

// Put this at the end of your test runner to see how many tests passed
#define SpinUnitPrintStatistics do {\
    printf(SpinUnitStatsMsg, testsRun, testsPassed, testsRun - testsPassed);\
} while (0)

#endif // ndef SPINUNIT_H
