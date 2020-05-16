#include "spinunit.h"
#include "dastr/linkedList.h"

SpinUnitRunnerDecls;

char* testListCreateDestroy(void) {
    LinkedList* list = linkedListCreate();
    SpinUnitAssert(list != NULL, "Failed to create linked list");
    linkedListDestroy(list);
    return NULL;
}

char* testListNodeCreateDestroy(void) {
    void* dummyData = (void*)0xDEADBEEF;
    LinkedListNode* node = linkedListCreateNode(dummyData);
    SpinUnitAssert(node != NULL, "Failed to create linked list node");
    SpinUnitAssert(
        node->data == dummyData, "Data in node does not match provided pointer"
    );
    SpinUnitAssert(node->prev == NULL, "Node has nonnull prev pointer");
    SpinUnitAssert(node->next == NULL, "Node has nonnull next pointer");
    linkedListDestroyNode(node);
    return NULL;
}

char* testListInsertFirst(void) {
    void* dummyData1 = (void*)0xCAFE;
    void* dummyData2 = (void*)0xBEEF;
    LinkedList* list = linkedListCreate();
    SpinUnitAssert(list != NULL, "Failed to create linked list");
    linkedListInsertFirst(list, dummyData1);
    SpinUnitAssert(
        linkedListFirst(list) == dummyData1,
        "First pointer added to list didn't match list head"
    );
    SpinUnitAssert(
        list->first == list->last,
        "Head and tail mismatched in list w/ one element"
    );
    linkedListInsertFirst(list, dummyData2);
    SpinUnitAssert(
        linkedListFirst(list) == dummyData2,
        "Second pointer added to list didn't match list head"
    );
    linkedListDestroy(list);
    return NULL;
}

char* testListPopFirst(void) {
    void* dummyData1 = (void*)0xCAFE;
    void* dummyData2 = (void*)0xBEEF;
    LinkedList* list = linkedListCreate();
    SpinUnitAssert(list != NULL, "Failed to create linked list");
    linkedListInsertFirst(list, dummyData1);
    linkedListInsertFirst(list, dummyData2);
    void* res = linkedListPopFirst(list);
    SpinUnitAssert(
        res == dummyData2, "Pointer popped from front of list didn't match"
    );
    SpinUnitAssert(
        linkedListFirst(list) == dummyData1,
        "Head of list after popping from front didn't match next element"
    );
    SpinUnitAssert(
        list->first == list->last,
        "Head and tail mismatched in list w/ one element after popping"
    );
    linkedListDestroy(list);
    return NULL;
}

char* testListInsertLast(void) {
    void* dummyData1 = (void*)0xCAFE;
    void* dummyData2 = (void*)0xBEEF;
    LinkedList* list = linkedListCreate();
    SpinUnitAssert(list != NULL, "Failed to create linked list");
    linkedListInsertLast(list, dummyData1);
    SpinUnitAssert(
        linkedListLast(list) == dummyData1,
        "First pointer added to list didn't match list tail"
    );
    SpinUnitAssert(
        list->first == list->last,
        "Head and tail mismatched in list w/ one element"
    );
    linkedListInsertLast(list, dummyData2);
    SpinUnitAssert(
        linkedListLast(list) == dummyData2,
        "Second pointer added to list didn't match list tail"
    );
    linkedListDestroy(list);
    return NULL;
}

char* testListPopLast(void) {
    void* dummyData1 = (void*)0xCAFE;
    void* dummyData2 = (void*)0xBEEF;
    LinkedList* list = linkedListCreate();
    SpinUnitAssert(list != NULL, "Failed to create linked list");
    linkedListInsertLast(list, dummyData1);
    linkedListInsertLast(list, dummyData2);
    void* res = linkedListPopLast(list);
    SpinUnitAssert(
        res == dummyData2, "Pointer popped from back of list didn't match"
    );
    SpinUnitAssert(
        linkedListLast(list) == dummyData1,
        "Tail of list after popping from back didn't match previous element"
    );
    SpinUnitAssert(
        list->first == list->last,
        "Head and tail mismatched in list w/ one element after popping"
    );
    linkedListDestroy(list);
    return NULL;
}

char* testListFind(void) {
    void* dummyData1 = (void*)0x0BAD;
    void* dummyData2 = (void*)0xCAFE;
    void* dummyData3 = (void*)0xDEAD;
    LinkedList* list = linkedListCreate();
    SpinUnitAssert(list != NULL, "Failed to create linked list");
    linkedListInsertLast(list, dummyData1);
    linkedListInsertLast(list, dummyData2);
    linkedListInsertLast(list, dummyData3);
    linkedListInsertLast(list, dummyData2);
    LinkedListNode* res = linkedListFind(list, dummyData2);
    SpinUnitAssert(res != NULL, "Could not find second element in list");
    SpinUnitAssert(
        res == list->first->next, "linkedListFind returned wrong node"
    );
    res = linkedListFind(list, (void*)0x98411);
    SpinUnitAssert(res == NULL, "linkedListFind found invalid node");
    linkedListDestroy(list);
    return NULL;
}

char* testListRemove(void) {
    void* dummyData1 = (void*)0x0BAD;
    void* dummyData2 = (void*)0xCAFE;
    void* dummyData3 = (void*)0xDEAD;
    void* dummyData4 = (void*)0xBEEF;
    LinkedList* list = linkedListCreate();
    SpinUnitAssert(list != NULL, "Failed to create linked list");
    LinkedListNode* node1 = linkedListInsertLast(list, dummyData1);
    LinkedListNode* node2 = linkedListInsertLast(list, dummyData2);
    LinkedListNode* node3 = linkedListInsertLast(list, dummyData3);
    LinkedListNode* node4 = linkedListInsertLast(list, dummyData4);

    linkedListRemove(list, dummyData3);
    SpinUnitAssert(
        node2->next == node4,
        "Wrong next pointer for node2 after removing node3"
    );
    SpinUnitAssert(
        node4->prev == node2,
        "Wrong previous pointer for node4 after removing node3"
    );
    SpinUnitAssert(
        list->first == node1,
        "Wrong list first pointer after removing node3"
    );
    SpinUnitAssert(
        list->last == node4,
        "Wrong list last pointer after removing node3"
    );

    linkedListRemove(list, dummyData4);
    SpinUnitAssert(
        node2->next == NULL,
        "Wrong next pointer for node2 after removing node4"
    );
    SpinUnitAssert(
        list->first == node1,
        "Wrong list first pointer after removing node4"
    );
    SpinUnitAssert(
        list->last == node2,
        "Wrong list last pointer after removing node4"
    );

    linkedListRemove(list, (void*)0x98411);
    SpinUnitAssert(
        list->first == node1,
        "Wrong list first pointer after removing invalid data pointer"
    );
    SpinUnitAssert(
        list->last == node2,
        "Wrong list last pointer after removing invalid data pointer"
    );

    linkedListRemove(list, dummyData1);
    SpinUnitAssert(
        node2->prev == NULL,
        "Wrong prev pointer for node2 after removing node1"
    );
    SpinUnitAssert(
        list->first == node2,
        "Wrong list first pointer after removing node1"
    );
    SpinUnitAssert(
        list->last == node2,
        "Wrong list last pointer after removing node1"
    );


    linkedListRemove(list, dummyData2);
    SpinUnitAssert(
        list->first == NULL,
        "Wrong list first pointer after removing node2"
    );
    SpinUnitAssert(
        list->last == NULL,
        "Wrong list last pointer after removing node2"
    );

    linkedListDestroy(list);
    return NULL;
}

char* testListRemoveNode(void) {
    void* dummyData1 = (void*)0x0BAD;
    void* dummyData2 = (void*)0xCAFE;
    void* dummyData3 = (void*)0xDEAD;
    void* dummyData4 = (void*)0xBEEF;
    LinkedList* list = linkedListCreate();
    SpinUnitAssert(list != NULL, "Failed to create linked list");
    LinkedListNode* node1 = linkedListInsertLast(list, dummyData1);
    LinkedListNode* node2 = linkedListInsertLast(list, dummyData2);
    LinkedListNode* node3 = linkedListInsertLast(list, dummyData3);
    LinkedListNode* node4 = linkedListInsertLast(list, dummyData4);

    linkedListRemoveNode(list, node3);
    SpinUnitAssert(
        node2->next == node4,
        "Wrong next pointer for node2 after removing node3"
    );
    SpinUnitAssert(
        node4->prev == node2,
        "Wrong previous pointer for node4 after removing node3"
    );
    SpinUnitAssert(
        list->first == node1,
        "Wrong list first pointer after removing node3"
    );
    SpinUnitAssert(
        list->last == node4,
        "Wrong list last pointer after removing node3"
    );

    linkedListRemoveNode(list, node4);
    SpinUnitAssert(
        node2->next == NULL,
        "Wrong next pointer for node2 after removing node4"
    );
    SpinUnitAssert(
        list->first == node1,
        "Wrong list first pointer after removing node4"
    );
    SpinUnitAssert(
        list->last == node2,
        "Wrong list last pointer after removing node4"
    );

    linkedListRemoveNode(list, node1);
    SpinUnitAssert(
        node2->prev == NULL,
        "Wrong prev pointer for node2 after removing node1"
    );
    SpinUnitAssert(
        list->first == node2,
        "Wrong list first pointer after removing node1"
    );
    SpinUnitAssert(
        list->last == node2,
        "Wrong list last pointer after removing node1"
    );


    linkedListRemoveNode(list, node2);
    SpinUnitAssert(
        list->first == NULL,
        "Wrong list first pointer after removing node2"
    );
    SpinUnitAssert(
        list->last == NULL,
        "Wrong list last pointer after removing node2"
    );

    linkedListDestroy(list);
    return NULL;
}

SpinUnitTestGroup(testGroupLinkedList,
    testListCreateDestroy,
    testListNodeCreateDestroy,
    testListInsertFirst,
    testListPopFirst,
    testListInsertLast,
    testListPopLast,
    testListFind,
    testListRemove,
    testListRemoveNode
);

int main(void) {
    SpinUnitRunTests(testGroupLinkedList);
    SpinUnitPrintStatistics;
    return SpinUnitDidAnyFail;
}
