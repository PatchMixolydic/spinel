#ifndef DASTR_TREE_H
#define DASTR_TREE_H

#include "dastr/linkedList.h"

typedef struct TreeNode {
    void* data;
    struct TreeNode* parent;
    // List of TreeNodes
    LinkedList* children;
} TreeNode;

// Allocates a new tree node with the given data
TreeNode* treeCreate(void* data);
// Frees the tree and all of its children
void treeDestroy(TreeNode* tree);
// Add a child to a given node
TreeNode* treeAddChild(TreeNode* node, void* data);
// Remove a given node from the tree. Its children are attached to its parent,
// and the node is freed. If the root node is removed, the entire tree is
// destroyed, and therefore freed.
// To remove a node and its children, use treeDestroy.
void* treeRemoveNode(TreeNode* node);
// Remove a child with the given data from a given node.
void treeRemoveChild(TreeNode* node, void* childData);

#endif // ndef DASTR_TREE_H
