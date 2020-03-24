#include <stdlib.h>
#include <string.h>
#include "dastr/linkedList.h"
#include "dastr/tree.h"

TreeNode* treeCreate(void* data) {
    TreeNode* res = malloc(sizeof(TreeNode));
    memset(res, 0, sizeof(TreeNode));
    res->children = linkedListCreate();
    res->data = data;
    return res;
}

void treeDestroy(TreeNode* tree) {
    if (tree == NULL) {
        return;
    }

    if (tree->parent != NULL) {
        // Assumes you want to remove everything from this node downwards
        // Disconnect this node from the tree
        linkedListRemove(tree->parent->children, tree);
        tree->parent = NULL;
    }

    if (tree->children != NULL) {
        // Recursively destroy children
        TreeNode* treeNode = NULL;
        ForEachInList(tree->children, listNode) {
            treeNode = (TreeNode*)listNode->data;
            if (treeNode != NULL) {
                treeDestroy(treeNode);
            }
        }
        // Free the child list
        linkedListDestroy(tree->children);
    }

    // Now, free this node
    free(tree);
    return;
}

TreeNode* treeAddChild(TreeNode* node, void* data) {
    if (node == NULL) {
        return NULL;
    }

    if (node->children == NULL) {
        // what the? might be an error
        node->children = linkedListCreate();
    }

    TreeNode* res = treeCreate(data);
    res->parent = node;
    linkedListInsertLast(node->children, (void*)res);

    return res;
}

void* treeRemoveNode(TreeNode* node) {
    if (node == NULL) {
        return NULL;
    }

    void* res = node->data;

    if (node->parent == NULL) {
        // deleting the root node! this kills the tree
        treeDestroy(node);
        return res;
    }

    // Remove this node from its parent's child list
    linkedListRemove(node->parent->children, node);

    if (node->children != NULL) {
        // Move children to parent
        ForEachInList(node->children, listNode) {
            TreeNode* child = (TreeNode*)listNode->data;
            if (child != NULL) {
                // attach each child to this node's parent
                child->parent = node->parent;
                // and put them in the parent's child list
                linkedListInsertLast(node->parent->children, (void*)child);
            }
        }

        // remove this node's child list
        linkedListDestroy(node->children);
    }

    // save data and free tree
    free(node);

    return res;
}

void treeRemoveChild(TreeNode* node, void* childData) {
    if (node == NULL) {
        return;
    }

    if (node->children != NULL) {
        // Find child and remove it
        ForEachInList(node->children, listNode) {
            TreeNode* child = listNode->data;
            if (child != NULL && childData == child->data) {
                // Found the child, remove it
                treeRemoveNode(child);
                break;
            }
        }
    }
}
