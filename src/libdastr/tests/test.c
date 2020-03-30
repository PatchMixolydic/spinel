#include "../../spinunit/include/spinunit.h"

SpinUnitRunnerDecls;

int main(void) {
    SpinUnitRunTests(testGroupTrueFalse);
    SpinUnitRunTests(testGroup2);
    SpinUnitPrintStatistics;
    return SpinUnitDidAnyFail;
}
