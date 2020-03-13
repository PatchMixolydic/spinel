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

VNode* vfsOpen(const char path[]) {
    if (strlen(path) == 0) {
        // Not a path
        return NULL;
    }

    // skip root slash
    path++;
    // TODO: path traversal, will probably need tokenization
    // for now, assume everything is located in /
    for (size_t i = 0; i < vfsRoot.numChildren; i++) {
        if (
            strncmp(
                vfsRoot.children[i].vnode->name, path, VFSFilenameLength
            ) == 0
        ) {
            return vfsRoot.children[i].vnode;
        }
    }

    return NULL;
}
