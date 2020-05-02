#include <dirent.h>
#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <spinel/vfs.h>

int nullfsOpen(char* path, FileFlags flags, struct VNode** res);
int nullfsOpenDir(char* path, DIR** res);
int nullfsLink(struct VNode* vnode, char* path);
int nullfsUnlink(char* path);

static FSInfo nullfsInfo = {
    "nullfs",
    nullfsOpen,
    nullfsOpenDir,
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

struct dirent* nullfsReadDir(DIR* dir) {
    if (dir == NULL) {
        return NULL;
    }

    if (dir->entryIdx == 0 || dir->entryIdx == 1) {
            dir->currentDirent.d_ino = 2;
            strcpy(
                dir->currentDirent.d_name,
                dir->entryIdx == 0 ? "." : ".."
            );
            return &dir->currentDirent;
    }

    return NULL;
}

ssize_t nullfsRead(VNode* vnode, uint8_t buf[], size_t size, off_t offset) {
    (void)size;
    (void)offset;

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

ssize_t nullfsWrite(VNode* vnode, uint8_t buf[], size_t size, off_t offset) {
    (void)size;
    (void)offset;

    if (vnode == NULL || buf == NULL) {
        return -EFAULT;
    } else if (vnode->type == FileDirectory) {
        return -EISDIR;
    } else if ((vnode->flags & FileWrite) == 0) {
        return -EACCES;
    }

    return size;
}

int nullfsOpenDir(char* path, DIR** res) {
    if (path == NULL || res == NULL) {
        return -EFAULT;
    }

    *res = malloc(sizeof(DIR));
    memset((unsigned char*)*res, 0, sizeof(DIR));
    VNode* vnode = createVNode(0, FileRead, FileDirectory);
    // -r-- -r-- -r--
    vnode->permissions = 0x444;
    vnode->read = nullfsRead;
    vnode->write = nullfsWrite;
    vnode->readdir = nullfsReadDir;
    vfsEmplace(vnode);
    (*res)->vfsINode = vnode->vfsINode;
    return 0;
}

int nullfsCloseDir(DIR* dir) {
    if (dir == NULL) {
        return -EFAULT;
    }
    vfsClose(dir->vfsINode);
    free(dir);
    return 0;
}

int nullfsOpen(char* path, FileFlags flags, VNode** res) {
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
