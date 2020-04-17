#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <spinel/vfs.h>

ssize_t zeroReadCallback(struct VNode* vnode, uint8_t* buf, size_t len) {
    (void)vnode;
    memset(buf, 0, len);
    return len;
}

ssize_t zeroWriteCallback(struct VNode* vnode, uint8_t* data, size_t len) {
    (void)vnode;
    (void)data;
    return len;
}

void initDevZero(void) {
    /*VNode* vnode = malloc(sizeof(VNode));
    memset(vnode, 0, sizeof(VNode));

    *vnode = (VNode){
        .flags = FileReadWrite | FileIgnoreRefCount,
        .type = FileNormal,
        // -rwx-rwx-rwx
        .permissions = 0x666,
        .readCallback = zeroReadCallback,
        .writeCallback = zeroWriteCallback
    };

    assert(vfsEmplace("/dev", vnode) == 0);*/
}
