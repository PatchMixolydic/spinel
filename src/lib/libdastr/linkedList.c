#include <stdlib.h>
#include <string.h>
#include "dastr/linkedList.h"

LinkedList* linkedListCreate(void) {
    LinkedList* res = malloc(sizeof(LinkedList));
    memset(res, 0, sizeof(LinkedList));
    return res;
}

LinkedListNode* linkedListCreateNode(void* data) {
    LinkedListNode* res = malloc(sizeof(LinkedListNode));
    memset(res, 0, sizeof(LinkedListNode));
    res->data = data;
    return res;
}

void linkedListDestroy(LinkedList* list) {
    if (list == NULL) {
        return;
    }

    // weird... might be an error?
    if (list->first == NULL && list->last != NULL) {
        list->first = list->last;
    }

    // now traverse to the last element, freeing along the way
    // next should be cached because the list node is freed
    for (
        LinkedListNode* node = list->first, * next = node->next;
        node != NULL;
        node = next, next = node->next
    ) {
        free(node);
    }

    free(list);
}

void linkedListDestroyNode(LinkedListNode* node) {
    if (node == NULL) {
        return;
    }

    // remove this node from the list sequence
    // attach its previous node to its next node
    if (node->prev != NULL) {
        node->prev->next = node->next;
    }
    if (node->next != NULL) {
        node->next->prev = node->prev;
    }

    // now the node is disconnected from the list
    free(node);
}

LinkedListNode* linkedListInsertFirst(LinkedList* list, void* data) {
    if (list == NULL) {
        return NULL;
    }

    // create our new node
    LinkedListNode* res = linkedListCreateNode(data);
    // attach this node to the front of the list
    res->next = list->first;
    if (list->first != NULL) {
        list->first->prev = res;
    }
    list->first = res;

    if (list->last == NULL) {
        // if there were no elements, this is also the last element
        list->last = res;
    }
    return res;
}

LinkedListNode* linkedListInsertLast(LinkedList* list, void* data) {
    if (list == NULL) {
        // ?
        return NULL;
    }

    // list is the last element in the list
    // now we can add our next element
    LinkedListNode* res = linkedListCreateNode(data);
    if (list->last != NULL) {
        list->last->next = res;
    }
    res->prev = list->last;
    list->last = res;

    if (list->first == NULL) {
        // if there were no elements, this is also the first element
        list->first = res;
    }
    return res;
}

void* linkedListPopFirst(LinkedList* list) {
    if (list == NULL || list->first == NULL) {
        return NULL;
    }

    void* res = list->first->data;
    LinkedListNode* oldFirst = list->first;
    LinkedListNode* newFirst = list->first->next;
    newFirst->prev = NULL;
    list->first = newFirst;
    linkedListDestroyNode(oldFirst);
    return res;
}

void* linkedListPopLast(LinkedList* list) {
    if (list == NULL || list->last == NULL) {
        return NULL;
    }

    void* res = list->last->data;
    LinkedListNode* oldLast = list->last;
    LinkedListNode* newLast = list->last->prev;
    newLast->next = NULL;
    list->last = newLast;
    linkedListDestroyNode(oldLast);
    return res;
}

void* linkedListFirst(LinkedList* list) {
    if (list == NULL) {
        return NULL;
    }

    return list->first;
}

void* linkedListLast(LinkedList* list) {
    if (list == NULL) {
        return NULL;
    }

    return list->last;
}

LinkedListNode* linkedListFind(LinkedList* list, void* data) {
    if (list == NULL) {
        return NULL;
    }

    ForEachInList(list, node) {
        if (data  == node->data) {
            // found it
            return node;
        }
    }

    return NULL;
}

void linkedListRemove(LinkedList* list, void* data) {
    if (list == NULL) {
        return;
    }

    linkedListRemoveNode(list, linkedListFind(list, data));
}

void linkedListRemoveNode(LinkedList* list, LinkedListNode* node) {
    if (list == NULL || node == NULL) {
        return;
    }

    if (node == list->first) {
        linkedListPopFirst(list);
    } else if (node == list->last) {
        linkedListPopLast(list);
    } else {
        linkedListDestroyNode(node);
    }
}
