#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <spinel/concurrency.h>
#include <spinel/vfs.h>

// TODO: perhaps a red/black tree could be used for faster lookup by inode
// TODO: tree data structure library/abstracted out?

typedef struct VFSTreeNode {
    VNode* vnode;
    struct VFSTreeNode* parent;
    struct VFSTreeNode* children;
    size_t numChildren;
} VFSTreeNode;

static VFSTreeNode vfsRoot = {NULL, NULL, NULL, 0};
static bool vfsInitialized = false;
static Mutex vfsTreeMutex = false;

// Given a parent TreeNode and a VNode,
// create a new child TreeNode to hold the VNode
static VFSTreeNode* addChildToNode(VFSTreeNode* parent, VNode* data) {
    parent->numChildren++;
    // TODO: recalloc / reallocarray
    // TODO: or perhaps it'd be better to use a linked list? it'd reduce
    // fragmentation as well as runtime
    // (malloc is intrinsically faster than realloc in this implementation)
    parent->children = realloc(
        parent->children, parent->numChildren * sizeof(VFSTreeNode)
    );
    VFSTreeNode* newNode = &parent->children[parent->numChildren - 1];
    memset(newNode, 0, sizeof(VFSTreeNode));
    newNode->vnode = data;
    newNode->parent = parent;
    return newNode;
}

// Convenience function for checking VNode name, bounded by VFSFilenameLength,
// since the comparison is pretty long
static bool isVNodeNamed(const char name[], VNode* vnode) {
    return strncmp(name, vnode->name, VFSFilenameLength) == 0;
}

// Get a pointer to a file's node in the VFS tree, or NULL if a corresponding
// node doesn't exist
static VFSTreeNode* getNodeByPath(const char path[]) {
    assert(vfsInitialized);

    if (path == NULL || *path == '\0') {
        return NULL;
    } else if (strncmp(path, "/", 2) == 0) {
        return &vfsRoot;
    }

    VFSTreeNode* currNode = &vfsRoot;
    // strtok_r modifies its first argument, but path is const
    // can't just use strdup(...) as the first argument, we must free this
    char* mutPath = strdup(path);
    char* tokState;
    char* token = strtok_r(mutPath, "/", &tokState);
    assert(token != NULL);
    do {
        VFSTreeNode* nextNode = NULL;
        for (size_t i = 0; i < currNode->numChildren; i++) {
            if (isVNodeNamed(token, currNode->children[i].vnode)) {
                // found a match
                nextNode = &currNode->children[i];
                break;
            }
        }

        if (nextNode == NULL) {
            // The next directory wasn't found
            // TODO: try and open the next directory based off of the parent
            // directory's device
            free(mutPath);
            return NULL;
        }

        currNode = nextNode; // move onwards
    } while((token = strtok_r(NULL, "/", &tokState)) != NULL);

    // We found the node
    free(mutPath);
    return currNode;
}

void initVFS(void) {
    VNode* root = malloc(sizeof(VNode));
    strlcpy(root->name, "", 2);
    vfsRoot.vnode = root;
    vfsInitialized = true;
}

int vfsEmplace(const char parentDir[], VNode* vnode) {
    assert(vfsInitialized);
    if (parentDir == NULL || vnode == NULL || *parentDir == '\0') {
        return -1;
    }

    spinlockMutex(&vfsTreeMutex);
    if (strncmp(parentDir, "/", 2) == 0) {
        // Emplacing to the root directory, don't need to do anything fancy...
        addChildToNode(&vfsRoot, vnode);
        unlockMutex(&vfsTreeMutex);
        return 0;
    }

    // We need to find where this vnode should go, and then emplace it
    VFSTreeNode* immediateParent = getNodeByPath(parentDir);
    if (immediateParent == NULL) {
        unlockMutex(&vfsTreeMutex);
        return -1;
    }

    addChildToNode(immediateParent, vnode);
    unlockMutex(&vfsTreeMutex);
    return 0;
}

VNode* vfsOpen(const char path[], FileFlags flags) {
    assert(vfsInitialized);
    if (strlen(path) == 0) {
        // Not a path
        return NULL;
    }

    // TODO: flags
    // grab the node
    spinlockMutex(&vfsTreeMutex);
    VFSTreeNode* res = getNodeByPath(path);
    unlockMutex(&vfsTreeMutex);
    return res->vnode;
}

void vfsClose(VNode* vnode) {
    assert(vfsInitialized);
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
    assert(vfsInitialized);
    if (vnode == NULL) {
        return;
    }

    if (vnode->destroyCallback != NULL) {
        vnode->destroyCallback(vnode);
    }

    spinlockMutex(&vfsTreeMutex);
    free(vnode);
    unlockMutex(&vfsTreeMutex);
}

int64_t vfsRead(VNode* vnode, void* buf, size_t size) {
    assert(vfsInitialized);
    if (vnode == NULL) {
        return -1;
    }

    if (vnode->readCallback != NULL) {
        return vnode->readCallback(vnode, buf, size);
    }

    return -1;
}

int64_t vfsWrite(VNode* vnode, void* buf, size_t size) {
    assert(vfsInitialized);
    if (vnode == NULL) {
        return -1;
    }

    if (vnode->writeCallback != NULL) {
        return vnode->writeCallback(vnode, buf, size);
    }

    return -1;
}
