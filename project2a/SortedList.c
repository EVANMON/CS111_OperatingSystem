#include <sched.h>
#include <stdlib.h>

#include "SortedList.h"
// struct SortedListElement {
// 	struct SortedListElement *prev;
// 	struct SortedListElement *next;
// 	const char *key;
// };
// typedef struct SortedListElement SortedList_t;
// typedef struct SortedListElement SortedListElement_t;


void SortedList_insert(SortedList_t *list, SortedListElement_t *element) {
	if (!list) return;
	if (!element) return;
	SortedListElement_t *cur = list->next;
	SortedListElement_t *pre = list;
	if (pre->key != NULL) return;
	while (cur && cur->key < element->key) {
		pre = cur;
		cur = cur->next;
	}
	// if (opt_yield & INSERT_YIELD) sched_yield();
	// if (opt_yield & INSERT_YIELD) {printf("%s\n", "yield!");}
	element->prev = pre;
	element->next = cur;
	pre->next = element;
	if (cur) cur->prev = element;
	return;
}


int SortedList_delete( SortedListElement_t *element) {
	if (!element) return 1;
	if (!element->prev || element->prev->next != element) return 1;
	if (element->next && element->next->prev != element) return 1;
	if (opt_yield & DELETE_YIELD) sched_yield();
	if (element->prev) element->prev->next = element->next;
	if (element->next) element->next->prev = element->prev;
	//free(element); // new ?
	return 0;
}


SortedListElement_t *SortedList_lookup(SortedList_t *list, const char *key) {
	if (!list) return NULL;
	if (!key) return NULL;
	SortedListElement_t *cur = list;
	while (cur) {
		if (cur->key == key) {
			if (opt_yield & LOOKUP_YIELD) sched_yield();
			return cur;
		}
		cur = cur->next;
	}
	return NULL;
}


int SortedList_length(SortedList_t *list) {
	int len = 0;
	if (!list) return -1;
	SortedListElement_t *cur = list->next;
	while(cur) {
		++len;
		if (opt_yield & LOOKUP_YIELD) sched_yield();
		if (cur->prev->next != cur) return -1; 
		cur = cur->next;
	}
	return len;
}

// extern int opt_yield;
// #define	INSERT_YIELD	0x01	// yield in insert critical section
// #define	DELETE_YIELD	0x02	// yield in delete critical section
// #define	LOOKUP_YIELD	0x04	// yield in lookup/length critical esction