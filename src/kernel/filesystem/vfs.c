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

// TODO: a red/black tree should be used for faster lookup, indexed by inode
LinkedList* vnodeList = NULL;
// List of mountpoints and their devices
LinkedList* mountList = NULL;
// List of all registered filesystems
// TODO: faster way to compare to find the desired entry; hashmap?
LinkedList* filesystemList = NULL;

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
        .fsINode = fsINode,
        .flags = flags,
        .type = type
    };
    return res;
}

void initVFS(void) {
    // Temporary root VNode
    vnodeList = linkedListCreate();
    VNode* root = createVNode(0, 0, FileDirectory);
    // vfsEmplace will assert if we try and use it here
    root->vfsINode = nextVFSINode++;
    linkedListInsertLast(vnodeList, root);
    vfsRoot = root;
    mountList = linkedListCreate();
    filesystemList = linkedListCreate();
    vfsInitialized = true;
}

ino_t vfsEmplace(VNode* vnode) {
    assert(vfsInitialized);
    if (vnode == NULL) {
        return 0;
    }

    vnode->vfsINode = nextVFSINode++;
    linkedListInsertLast(vnodeList, vnode);
    return vnode->vfsINode;
}

VNode* vfsOpenINode(ino_t inode, FileFlags flags) {
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
        vfsDestroyVNode(vnode);
    }
    return 0;
}

void vfsDestroyVNode(VNode* vnode) {
    assert(vfsInitialized);
    if (vnode == NULL) {
        return;
    }

    if (vnode->destroy != NULL) {
        vnode->destroy(vnode);
    }

    free(vnode);
}

int vfsRegisterFilesystem(FSInfo* fsInfo) {
    assert(vfsInitialized);
    if (fsInfo == NULL) {
        return -EFAULT;
    }

    // Check to make sure the filesystem isn't already registered
    ForEachInList(filesystemList, listNode) {
        if (listNode->data != NULL) {
            FSInfo* other = (FSInfo*)listNode->data;
            if (strncmp(fsInfo->name, other->name, FSNameLength) == 0) {
                // Filesystem is already registered
                return -EEXIST;
            }
        }
    }

    linkedListInsertLast(filesystemList, fsInfo);
    return 0;
}

int vfsUnregisterFilesystem(char* name) {
    assert(vfsInitialized);
    if (name == NULL) {
        return -EFAULT;
    }

    ForEachInList(filesystemList, listNode) {
        if (listNode->data != NULL) {
            FSInfo* other = (FSInfo*)listNode->data;
            if (strncmp(name, other->name, FSNameLength) == 0) {
                // Found the node
                if (other->unregister != NULL) {
                    other->unregister(other);
                }
                linkedListDestroyNode(listNode);
                return 0;
            }
        }
    }

    return -ENOFS;
}

ssize_t vfsRead(ino_t inode, void* buf, size_t size, off_t offset) {
    assert(vfsInitialized);

    VNode* vnode = getVNodeByINode(inode);
    if (vnode == NULL) {
        return -EINVAL;
    }

    if (vnode->read != NULL) {
        return vnode->read(vnode, buf, size, offset);
    }

    return -ENOTSUP;
}

ssize_t vfsWrite(ino_t inode, void* buf, size_t size, off_t offset) {
    assert(vfsInitialized);

    VNode* vnode = getVNodeByINode(inode);
    if (vnode == NULL) {
        return -EINVAL;
    }

    if (vnode->write != NULL) {
        return vnode->write(vnode, buf, size, offset);
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
        struct dirent* res = vnode->readdir(dir);
        if (res != NULL) {
            dir->entryIdx++;
        }
        return res;
    }

    return NULL;
}
