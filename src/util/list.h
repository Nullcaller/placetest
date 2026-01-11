#ifndef PLACETEST_UTIL_LIST_H
#define PLACETEST_UTIL_LIST_H

#include<stdbool.h>

typedef struct s_list_node ListNode;

typedef struct s_list List;

struct s_list_node {
	List* list;
	ListNode* next;
	ListNode* previous;
	void* data;
};

struct s_list {
	ListNode* first;
	ListNode* last;
};

List* list_create_empty();

List* list_create_filled(void** data_array, unsigned int data_array_size);

List* list_copy(List* list);

void list_free(List* list);

ListNode* list_add_first(List* list, void* data);

ListNode* list_add_last(List* list, void* data);

ListNode* list_add_after(List* list, ListNode* node, void* data);

ListNode* list_add_before(List* list, ListNode* node, void* data);

ListNode* list_add_at(List* list, unsigned int index, bool from_first, void* data);

ListNode* list_add_at_from_first(List* list, unsigned int index, void* data);

ListNode* list_add_at_from_last(List* list, unsigned int index, void* data);

ListNode* list_get_at(List* list, unsigned int index, bool from_first);

ListNode* list_get_at_from_first(List* list, unsigned int index);

ListNode* list_get_at_from_last(List* list, unsigned int index);

ListNode* list_search(List* list, bool from_first, void* data);

ListNode* list_search_from_first(List* list, void* data);

ListNode* list_search_from_last(List* list, void* data);

bool list_contains(List* list, ListNode* node);

void list_unlink_node(List* list, ListNode* node);

void list_remove_node(List* list, ListNode* node);

void list_unlink_sublist(List* list, ListNode* start, ListNode* end);

void list_remove_sublist(List* list, ListNode* start, ListNode* end);

unsigned int list_size(List* list);

void** list_to_array(List* list, unsigned int* p_size);

#endif
