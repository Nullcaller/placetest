#include<stdlib.h>

#include "list.h"

List* list_create_empty() {
	List* list = malloc(sizeof(List));
	list->first = NULL;
	list->last = NULL;
	return list;
}

List* list_create_filled(void** data_array, unsigned int data_array_size) {
	List* list = malloc(sizeof(List));
	ListNode** nodes = malloc(sizeof(ListNode*)*data_array_size);
	for(unsigned int i = 0; i < data_array_size; i++) {
		nodes[i] = malloc(sizeof(ListNode));
		nodes[i]->list = list;
		nodes[i]->data = data_array[i];
	}
	for(unsigned int i = 0; i < data_array_size; i++) {
		nodes[i]->previous = (i != 0) ? nodes[i-1] : NULL;
		nodes[i]->next = (i != (data_array_size-1)) ? nodes[i+1] : NULL;
	}
	list->first = nodes[0];
	list->last = nodes[data_array_size-1];
	free(nodes);
	return list;
}

List* list_copy(List* list) {
	List* new_list = list_create_empty();
	for(ListNode* current = list->first; current != NULL; current = current->next)
		list_add_last(new_list, current->data);
	return new_list;
}

void list_free(List* list) {
	ListNode* current = list->first;
	ListNode* next;
	while(current != NULL) {
		next = current->next;
		free(current);
		current = next;
	}
	free(list);
}

ListNode* list_add_first(List* list, void* data) {
	ListNode* new = malloc(sizeof(ListNode));
	new->list = list;
	new->data = data;
	new->next = list->first;
	new->previous = NULL;
	if(list->first != NULL)
		list->first->previous = new;
	else
		list->last = new;
	list->first = new;
	return new;
}

ListNode* list_add_last(List* list, void* data) {
	ListNode* new = malloc(sizeof(ListNode));
	new->list = list;
	new->data = data;
	new->next = NULL;
	new->previous = list->last;
	if(list->last != NULL)
		list->last->next = new;
	else
		list->first = new;
	list->last = new;
	return new;
}

static ListNode* list_add_between_(List* list, ListNode* previous, ListNode* next, void* data) {
	ListNode* new = malloc(sizeof(ListNode));
	new->list = list;
	new->data = data;
	new->next = next;
	new->previous = previous;
	if(next != NULL)
		next->previous = new;
	else
		list->last = new;
	if(previous != NULL)
		previous->next = new;
	else
		list->first = new;
	return new;
}

ListNode* list_add_after(List* list, ListNode* node, void* data) {
	ListNode* previous;
	ListNode* next;
	previous = node;
	if(node == NULL)
		next = list->first;
	else
		next = node->next;
	return list_add_between_(list, previous, next, data);
}

ListNode* list_add_before(List* list, ListNode* node, void* data) {
	ListNode* previous;
	ListNode* next;
	next = node;
	if(node == NULL)
		previous = list->last;
	else
		previous = node->previous;
	return list_add_between_(list, previous, next, data);
}

ListNode* list_add_at(List* list, unsigned int index, bool from_first, void* data) {
	if(from_first)
		return list_add_at_from_first(list, index, data);
	else
		return list_add_at_from_last(list, index, data);
}

ListNode* list_add_at_from_first(List* list, unsigned int index, void* data) {
	ListNode* previous;
	ListNode* next;
	previous = list_get_at_from_first(list, index);
	if(previous != NULL)
		next = previous->next;
	else
		next = NULL;
	return list_add_between_(list, previous, next, data);
}

ListNode* list_add_at_from_last(List* list, unsigned int index, void* data) {
	ListNode* previous;
	ListNode* next;
	next = list_get_at_from_last(list, index);
	if(next != NULL)
		previous = next->previous;
	else
		previous = NULL;
	return list_add_between_(list, previous, next, data);
}

ListNode* list_get_at(List* list, unsigned int index, bool from_first) {
	if(from_first)
		return list_get_at_from_first(list, index);
	else
		return list_get_at_from_last(list, index);
}

ListNode* list_get_at_from_first(List* list, unsigned int index) {
	ListNode* current = list->first;
	for(unsigned int i = 0; i < index; i++) {
		if(current != NULL)
			current = current->next;
		else
			return NULL;
	}
	return current;
}

ListNode* list_get_at_from_last(List* list, unsigned int index) {
	ListNode* current = list->first;
	for(unsigned int i = 0; i < index; i++) {
		if(current != NULL)
			current = current->previous;
		else
			return NULL;
	}
	return current;
}

ListNode* list_search(List* list, bool from_first, void* data) {
	if(from_first)
		return list_search_from_first(list, data);
	else
		return list_search_from_last(list, data);
}

ListNode* list_search_from_first(List* list, void* data) {
	for(ListNode* current = list->first; current != NULL; current = current->next)
		if(current->data == data)
			return current;
	return NULL;
}

ListNode* list_search_from_last(List* list, void* data) {
	for(ListNode* current = list->last; current != NULL; current = current->previous)
		if(current->data == data)
			return current;
	return NULL;
}

bool list_contains(List* list, ListNode* node) {
	return node->list == list;
}

void list_unlink_node(List* list, ListNode* node) {
	if(node == NULL)
		return;
	if(node->next != NULL)
		node->next->previous = node->previous;
	else
		list->last = node->previous;
	if(node->previous != NULL)
		node->previous->next = node->next;
	else
		list->first = node->next;
	node->next = NULL;
	node->previous = NULL;
	node->list = NULL;
}

void list_remove_node(List* list, ListNode* node) {
	list_unlink_node(list, node);
	free(node);
}

void list_unlink_sublist(List* list, ListNode* start, ListNode* end) {
	if(start == NULL)
		return;
	if(end == NULL)
		return;
	if(end->next != NULL)
		end->next->previous = start->previous;
	else
		list->last = start->previous;
	if(start->previous != NULL)
		start->previous->next = end->next;
	else
		list->first = end->next;
	end->next = NULL;
	start->previous = NULL;
	for(ListNode* current = start; current != NULL; current = current->next)
		current->list = NULL;
}

void list_remove_sublist(List* list, ListNode* start, ListNode* end) {
	list_unlink_sublist(list, start, end);
	ListNode* current = start;
	ListNode* next;
	while(current != NULL) {
		next = current->next;
		free(current);
		current = next;
	}
}

unsigned int list_size(List* list) {
	unsigned int ret = 0;
	ListNode* node = list->first;
	while(node != NULL) {
		ret++;
		node = node->next;
	}
	return ret;
}

void** list_to_array(List* list, unsigned int* p_size) {
	unsigned int size = list_size(list);
	*p_size = size;
	if(size == 0)
		return NULL;
	void** arr = malloc(sizeof(void*)*size);
	unsigned int it = 0;
	for(ListNode* i = list->first; i != NULL; i = i->next) {
		arr[it] = i->data;
		it++;
	}
	return arr;
}
