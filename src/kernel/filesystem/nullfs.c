#include <dirent.h>
#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <spinel/vfs.h>

int nullfsOpen(char* path, FileFlags flags, struct VNode** res);
int nullfsLink(struct VNode* vnode, char* path);
int nullfsUnlink(char* path);

static FSInfo nullfsInfo = {
        "nullfs",
        nullfsOpen,
        nullfsLink,
        nullfsUnlink,
        NULL
};

void nullfsInit(void) {
    vfsRegisterFilesystem(&nullfsInfo);
}

int nullfsLink(struct VNode* vnode, char* path) {
    if (vnode == NULL || path == NULL) {
        return -EFAULT;
    }
    // okay, it's linked
    return 0;
}

int nullfsUnlink(char* path) {
    if (path == NULL) {
        return -EFAULT;
    }
    // okay, it's unlinked
    return 0;
}

struct dirent* nullfsReadDir(struct VNode* vnode, DIR* dir) {
    if (vnode == NULL || dir == NULL) {
        return NULL;
    } else if (vnode->type != FileDirectory) {
        return NULL;
    } else if ((vnode->flags & FileRead) == 0) {
        return NULL;
    }

    if (dir->currentDirent == NULL) {
        // epic awesome
        dir->currentDirent = malloc(sizeof(struct dirent));
    }

    if (dir->entryIdx == 0 || dir->entryIdx == 1) {
            dir->currentDirent->d_ino = 2;
            strcpy(
                dir->currentDirent->d_name,
                dir->entryIdx == 0 ? "." : ".."
            );
            return dir->currentDirent;
    }

    return NULL;
}

ssize_t nullfsRead(VNode* vnode, uint8_t buf[], size_t size) {
    (void)size;

    if (vnode == NULL || buf == NULL) {
        // even though we do nothing with them...
        return -EFAULT;
    } else if (vnode->type == FileDirectory) {
        return -EISDIR;
    } else if ((vnode->flags & FileRead) == 0) {
        return -EACCES;
    }

    return 0; // EOF
}

ssize_t nullfsWrite(VNode* vnode, uint8_t buf[], size_t size) {
    (void)buf;
    (void)size;

    if (vnode == NULL || buf == NULL) {
        return -EFAULT;
    } else if (vnode->type == FileDirectory) {
        return -EISDIR;
    } else if ((vnode->flags & FileWrite) == 0) {
        return -EACCES;
    }

    return size;
}

int nullfsOpen(char* path, FileFlags flags, struct VNode** res) {
    if (path == NULL || res == NULL) {
        return -EFAULT;
    } else if (path[strlen(path) - 1] == '/') {
        return -EISDIR;
    }
    *res = createVNode(0, flags, FileNormal);
    // -rw- -rw- -rw-
    (*res)->permissions = 0x666;
    (*res)->read = nullfsRead;
    (*res)->write = nullfsWrite;
    (*res)->readdir = nullfsReadDir;
    return 0;
}