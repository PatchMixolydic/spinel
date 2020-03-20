#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <spinel/alloc.h>
#include <spinel/concurrency.h>
#include <spinel/cpu.h>
#include <spinel/tty.h>
#include <spinel/zero.h>
#include <spinel/vfs.h>

static const char* const DevDirectoryName = "dev";

void kernelMain(void) {
    printf("The system is coming up.\n");
    initAlloc();
    initVFS();

    VNode* devDir = malloc(sizeof(VNode));
    *devDir = (VNode) {
        .flags = FileReadWrite | FileIgnoreRefCount,
        .type = FileDirectory,
        // -rw- -r-- -r--
        .permissions = 0x544,
    };
    strlcpy(devDir->name, DevDirectoryName, strlen(DevDirectoryName) + 1);

    assert(vfsEmplace("/", devDir) == 0);

    initDevZero();

    printf("Opening /dev/zero\n");
    VNode* zero = vfsOpen("/dev/zero", FileReadWrite);
    assert(zero != NULL);
    uint8_t myBuf[] = {1, 2, 3, 4, 5};
    printf("Here is my buffer with random data: ");
    printf(
        "%u %u %u %u %u\n", myBuf[0], myBuf[1], myBuf[2], myBuf[3], myBuf[4]
    );
    int64_t count = vfsRead(zero, myBuf, 5);
    assert(count != -1);
    printf("Read %d bytes from /dev/zero\n", count);
    printf("Here is my buffer after reading from /dev/zero: ");
    printf(
        "%u %u %u %u %u\n", myBuf[0], myBuf[1], myBuf[2], myBuf[3], myBuf[4]
    );
    printf("Writing to /dev/zero\n");
    count = vfsWrite(zero, myBuf, 5);
    assert(count != -1);
    printf("Wrote %d bytes to /dev/zero\n", count);
    vfsClose(zero);
    printf("Closed /dev/zero\n");

    while (1) {
        haltCPU();
    }
}
