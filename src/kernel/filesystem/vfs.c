#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <spinel/vfs.h>

// TODO: perhaps a red/black tree could be used for faster lookup by inode
// TODO: tree data structure library/abstracted out?

typedef struct VFSTreeNode {
    VNode* vnode;
    struct VFSTreeNode* parent;
    struct VFSTreeNode* children;
    size_t numChildren;
} VFSTreeNode;

VFSTreeNode vfsRoot = {NULL, NULL, NULL, 0};

bool vfsInitialized = false;

void initVFS(void) {
    VNode* root = malloc(sizeof(VNode));
    strlcpy(root->name, "", 2);
    vfsRoot.vnode = root;
    vfsInitialized = true;
}

void vfsEmplace(const char parentDir[], VNode* vnode) {
    assert(vfsInitialized);

    // TODO: path traversal
    // for this prototype just put everything in /
    if (vnode == NULL) {
        return;
    }

    vfsRoot.numChildren++;
    // TODO: recalloc / reallocarray
    vfsRoot.children = realloc(
        vfsRoot.children, vfsRoot.numChildren * sizeof(VFSTreeNode)
    );
    VFSTreeNode* newNode = &vfsRoot.children[vfsRoot.numChildren - 1];
    memset(newNode, 0, sizeof(VFSTreeNode));
    newNode->vnode = vnode;
    newNode->parent = &vfsRoot;
}

VNode* vfsOpen(const char path[], FileFlags flags) {
    if (strlen(path) == 0) {
        // Not a path
        return NULL;
    }

    // skip root slash
    path++;
    // TODO: path traversal, will probably need tokenization
    // for now, assume everything is located in /
    // TODO: handle flags
    for (size_t i = 0; i < vfsRoot.numChildren; i++) {
        if (
            strncmp(
                vfsRoot.children[i].vnode->name, path, VFSFilenameLength
            ) == 0
        ) {
            VNode* res = vfsRoot.children[i].vnode;
            res->refCount++;
            if (res->openCallback != NULL) {
                res->openCallback(res, flags);
            }
            return res;
        }
    }

    return NULL;
}

void vfsClose(VNode* vnode) {
    if (vnode == NULL) {
        return;
    }

    if (vnode->closeCallback != NULL) {
        vnode->closeCallback(vnode);
    }
    vnode->refCount--;

    if (vnode->refCount == 0 && !(vnode->flags & FileIgnoreRefCount)) {
        // No references to this vnode, drop it
        vfsDestroy(vnode);
    }
}

void vfsDestroy(VNode* vnode) {
    if (vnode == NULL) {
        return;
    }

    if (vnode->destroyCallback != NULL) {
        vnode->destroyCallback(vnode);
    }
    free(vnode);
}

size_t vfsRead(VNode* vnode, void* buf, size_t size) {
    if (vnode == NULL) {
        return -1;
    }

    if (vnode->readCallback != NULL) {
        return vnode->readCallback(vnode, buf, size);
    }

    return -1;
}

size_t vfsWrite(VNode* vnode, void* buf, size_t size) {
    if (vnode == NULL) {
        return -1;
    }

    if (vnode->writeCallback != NULL) {
        return vnode->writeCallback(vnode, buf, size);
    }

    return -1;
}
