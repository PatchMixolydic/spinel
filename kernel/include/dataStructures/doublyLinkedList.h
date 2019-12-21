#ifndef _DATA_STRUCTURES_DOUBLYLINKEDLIST_H
#define _DATA_STRUCTURES_DOUBLYLINKEDLIST_H

typedef struct DoublyLinkedNode {
    struct DoublyLinkedNode* prev;
    struct DoublyLinkedNode* next;
    void* data;
} DoublyLinkedNode;

typedef struct {
    DoublyLinkedNode* first;
    DoublyLinkedNode* last;
} DoublyLinkedList;

// Add an item, pointed by void*, to the back of the list
void dllistAddBack(DoublyLinkedList* list, void* item);
// Add an item, pointed by void*, to the front of the list
void dllistAddFront(DoublyLinkedList* list, void* item);
// Remove a node from the back of the list and return its data
void* dllistPopBack(DoublyLinkedList* list);
// Remove a node from the front of the list and return its data
void* dllistPopFront(DoublyLinkedList* list);
// Remove a node from the list. The node is no longer valid afterwards.
void dllistRemoveItem(DoublyLinkedList* list, DoublyLinkedNode* node);
// Free all memory associated with the list
void dllistDelete(DoublyLinkedList* list);

#endif // ndef _DATA_STRUCTURES_DOUBLYLINKEDLIST_H
