#ifndef DASTR_LINKEDLIST_H
#define DASTR_LINKEDLIST_H

#define ForEachInList(list, nodeName) for (\
    LinkedListNode* nodeName = list->first;\
    nodeName != NULL;\
    nodeName = nodeName->next\
)

typedef struct LinkedListNode {
    void* data;
    struct LinkedListNode* prev;
    struct LinkedListNode* next;
} LinkedListNode;

typedef struct {
    LinkedListNode* first;
    LinkedListNode* last;
} LinkedList;

// Creates a new, empty linked list object
LinkedList* linkedListCreate(void);
// Creates a node that contains the given data
LinkedListNode* linkedListCreateNode(void* data);
// Frees the linked list and its contents
void linkedListDestroy(LinkedList* list);
// Removes/frees a node from the list with no regard for first or last pointers
void linkedListDestroyNode(LinkedListNode* node);
// Inserts at the beginning of a linked list
LinkedListNode* linkedListInsertFirst(LinkedList* list, void* data);
// Inserts at the end of a linked list
LinkedListNode* linkedListInsertLast(LinkedList* list, void* data);
// Removes/frees the first element of a linked list, then returns its data
void* linkedListPopFirst(LinkedList* list);
// Removes/frees the last element of a linked list, returns its data
void* linkedListPopLast(LinkedList* list);
// Returns a pointer to the first element of a linked list
void* linkedListFirst(LinkedList* list);
// Returns a pointer to the last element of a linked list
void* linkedListLast(LinkedList* list);
// Returns the first occurence of the pointer data in the list,
// or NULL if data is not in the list
LinkedListNode* linkedListFind(LinkedList* list, void* data);
// Removes/frees the first node containing the give pointer, while managing
// the first and last pointers of the list
void linkedListRemove(LinkedList* list, void* data);
// Removes/frees a node from a linked list, managing first and last pointers
void linkedListRemoveNode(LinkedList* list, LinkedListNode* node);

#endif // ndef DASTR_LINKEDLIST_H
