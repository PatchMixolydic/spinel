#include <stdlib.h>
#include <string.h>
#include <spinel/vfs.h>

static const char* const DeviceName = "zero";

size_t zeroReadCallback(struct VNode* vnode, uint8_t* buf, size_t length) {
    memset(buf, 0, length);
    return length;
}

size_t zeroWriteCallback(struct VNode* vnode, uint8_t* data, size_t len) {
    return 0;
}

void initDevZero(void) {
    VNode* vnode = malloc(sizeof(VNode));
    memset(vnode, 0, sizeof(VNode));

    strlcpy(vnode->name, DeviceName, strlen(DeviceName));
    *vnode = (VNode){
        .flags = FileReadWrite | FileIgnoreRefCount,
        .type = FileNormal,
        // -rwx-rwx-rwx
        .permissions = 0x666,
        .readCallback = zeroReadCallback,
        .writeCallback = zeroWriteCallback
    };

    vfsEmplace("/dev", vnode);
}
