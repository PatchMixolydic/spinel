#include "spinunit.h"

SpinUnitRunnerDecls;

char* testAssertTrue(void) {
    SpinUnitAssert(1, "Tried and failed to assert true");
    return NULL;
}

char* testAssertFalse(void) {
    SpinUnitAssert(0, "A fail here is a pass, actually.");
    return NULL;
}

SpinUnitTestGroup(testGroupTrueFalse, testAssertTrue, testAssertFalse);

char* testAssertEquation(void) {
    int x = 2 + 2;
    SpinUnitAssert(x == 4, "Two plus two doesn't equal four?");
    return NULL;
}

SpinUnitTestGroup(testGroup2, testAssertEquation);

int main(void) {
    SpinUnitRunTests(testGroupTrueFalse);
    SpinUnitRunTests(testGroup2);
    SpinUnitPrintStatistics;
    // expect one failure
    return testsRun - testsPassed == 1 ? 0 : 1;
}
