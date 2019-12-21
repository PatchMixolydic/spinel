#include <stddef.h>
#include <dataStructures/doublyLinkedList.h>
#include <kernel/memory.h>

static DoublyLinkedNode* createNode() {
    DoublyLinkedNode* node = (DoublyLinkedNode*)kernelMalloc(sizeof(DoublyLinkedNode));
    node->data = NULL;
    node->prev = NULL;
    node->next = NULL;
    return node;
}

void dllistAddBack(DoublyLinkedList* list, void* item) {
    if (list == NULL) {
        return; // :(
    }
    DoublyLinkedNode* node = createNode();
    node->data = item;
    if (list->last == NULL || list->first == NULL) {
        // We don't have a last element or we don't have a first
        // They should imply each other -- otherwise it's invalid and should be
        // fixed anyway
        list->last = node;
        list->first = node;
    } else {
        // We have a last node, so tack it on there
        node->prev = list->last;
        list->last->next = node;
        list->last = node;
    }
}

void dllistAddFront(DoublyLinkedList* list, void* item) {
    if (list == NULL) {
        return; // :(
    }
    DoublyLinkedNode* node = createNode();
    node->data = item;
    if (list->first == NULL || list->last == NULL) {
        list->first = node;
        list->last = node;
    } else {
        // We have a first node, so tack it on there
        node->next = list->first;
        list->first->prev = node;
        list->first = node;
    }
}

void* dllistPopBack(DoublyLinkedList* list) {
    void* res = NULL;
    if (list != NULL && list->last != NULL) {
        DoublyLinkedNode* thisNode = list->last;
        res = thisNode->data;
        thisNode->prev->next = NULL; // Sets next pointer of previous node
        list->last = thisNode->prev;
        kernelFree((void*)thisNode);
    }
    return res;
}

void* dllistPopFront(DoublyLinkedList* list) {
    void* res = NULL;
    if (list != NULL && list->first != NULL) {
        DoublyLinkedNode* thisNode = list->first;
        res = thisNode->data;
        thisNode->next->prev = NULL; // Sets previous pointer of next node
        list->first = thisNode->next;
        kernelFree((void*)thisNode);
    }
    return res;
}

void dllistRemoveItem(DoublyLinkedList* list, DoublyLinkedNode* node) {
    if (node == NULL || list == NULL || list->first == NULL) {
        return;
    }
    if (node == list->first) {
        dllistPopFront(list);
    } else if (node == list->last) {
        dllistPopBack(list);
    } else {
        // Set previous node's next pointer to new next node
        node->prev->next = node->next;
        // Set next node's previous pointer to new previous node
        node->next->prev = node->prev;
        // Bye
        kernelFree((void*)node);
    }
}

void dllistDelete(DoublyLinkedList* list) {
    DoublyLinkedNode* nextPtr = NULL;
    for (DoublyLinkedNode* x = list->first; x != NULL; x = nextPtr) {
        nextPtr = x->next; // Save the next pointer before...
        kernelFree((void*)x); // ...scuttling this node.
    }
    list->first = NULL;
    list->last = NULL;
}
