#include <assert.h>
#include <dirent.h>
#include <errno.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <dastr/linkedList.h>
#include <spinel/concurrency.h>
#include <spinel/vfs.h>

typedef struct {
    char* path;
    VNode* deviceNode;
} MountInfo;

// TODO: a red/black tree should be used for faster lookup, indexed by inode
LinkedList* vnodeList = NULL;
// List of mountpoints and their devices
LinkedList* mountList = NULL;

static VNode* vfsRoot = NULL;
static bool vfsInitialized = false;
static ino_t nextVFSINode = 1;

static VNode* getVNodeByINode(ino_t inode) {
    ForEachInList(vnodeList, listNode) {
        VNode* vnode = (VNode*)listNode->data;
        if (vnode != NULL && inode == vnode->vfsINode) {
            return vnode;
        }
    }
    return NULL;
}

VNode* createVNode(ino_t fsINode, FileFlags flags, FileType type) {
    VNode* res = malloc(sizeof(VNode));
    *res = (VNode) {
        .vfsINode = nextVFSINode++,
        .fsINode = fsINode,
        .flags = flags,
        .type = type
    };
    return res;
}

void initVFS(void) {
    VNode* root = malloc(sizeof(VNode));
    memset(root, 0, sizeof(VNode));
    root->vfsINode = nextVFSINode++;
    vfsRoot = root;
    vnodeList = linkedListCreate();
    linkedListInsertFirst(vnodeList, linkedListCreateNode(root));
    mountList = linkedListCreate();
    vfsInitialized = true;
}

ino_t vfsEmplace(VNode* vnode) {
    assert(vfsInitialized);
    if (vnode == NULL) {
        return 0;
    }

    vnode->vfsINode = nextVFSINode++;
    linkedListInsertLast(vnodeList, linkedListCreateNode(vnode));
    return vnode->vfsINode;
}

VNode* vfsOpen(ino_t inode, FileFlags flags) {
    assert(vfsInitialized);

    // TODO: flags
    VNode* vnode = getVNodeByINode(inode);
    if (vnode == NULL) {
        // not found!
        return NULL;
    }
    vnode->refCount++;
    if (vnode->open != NULL) {
        vnode->open(vnode, flags);
    }
    return vnode;
}

int vfsClose(ino_t inode) {
    assert(vfsInitialized);

    VNode* vnode = getVNodeByINode(inode);
    if (vnode == NULL) {
        // not found!
        return -EFAULT;
    }

    if (vnode->close != NULL) {
        vnode->close(vnode);
    }
    vnode->refCount--;

    if (vnode->refCount == 0 && !(vnode->flags & FileIgnoreRefCount)) {
        // No references to this vnode, drop it
        vfsDestroy(vnode);
    }
    return 0;
}

void vfsDestroy(VNode* vnode) {
    assert(vfsInitialized);
    if (vnode == NULL) {
        return;
    }

    if (vnode->destroy != NULL) {
        vnode->destroy(vnode);
    }
}

ssize_t vfsRead(ino_t inode, void* buf, size_t size) {
    assert(vfsInitialized);

    VNode* vnode = getVNodeByINode(inode);
    if (vnode == NULL) {
        return -EINVAL;
    }

    if (vnode->read != NULL) {
        return vnode->read(vnode, buf, size);
    }

    return -ENOTSUP;
}

ssize_t vfsWrite(ino_t inode, void* buf, size_t size) {
    assert(vfsInitialized);

    VNode* vnode = getVNodeByINode(inode);
    if (vnode == NULL) {
        return -EINVAL;
    }

    if (vnode->write != NULL) {
        return vnode->write(vnode, buf, size);
    }

    return -ENOTSUP;
}

struct dirent* vfsReadDir(DIR* dir) {
    assert(vfsInitialized);

    if (dir == NULL) {
        return NULL;
    }

    VNode* vnode = getVNodeByINode(dir->vfsINode);
    if (vnode == NULL) {
        return NULL;
    }

    if (vnode->readdir != NULL) {
        struct dirent* res = vnode->readdir(vnode, dir);
        if (res != NULL) {
            dir->entryIdx++;
        }
        return res;
    }

    return NULL;
}
