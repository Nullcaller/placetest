#include<stdio.h>
#include<math.h>
#include<sodium.h>

#include<util/list.h>

#include "cell.h"

Chip* chip_create_empty(int func_cell_width, int func_cell_height, int io_ring_depth) {
	Chip* ret = malloc(sizeof(Chip));
	ret->filled = false;
	ret->func_cell_width = func_cell_width;
	ret->func_cell_height = func_cell_height;
	ret->io_ring_depth = io_ring_depth;
	ret->cells = list_create_empty();
	ret->func_cells = list_create_empty();
	ret->io_cells = list_create_empty();
	return ret;
}

static void chip_fill_(Chip* chip);

Chip* chip_create_filled(int func_cell_width, int func_cell_height, int io_ring_depth) {
	Chip* ret = chip_create_empty(func_cell_width, func_cell_height, io_ring_depth);
	chip_fill_(ret);
	return ret;
}

static void chip_fill_(Chip* chip) {
	int func_cell_width = chip->func_cell_width;
	int func_cell_height = chip->func_cell_height;
	int io_ring_depth = chip->io_ring_depth;

	int func_cell_count = func_cell_width*func_cell_height;
	int total_width = func_cell_width+2*io_ring_depth;
	int total_height = func_cell_height+2*io_ring_depth;
	int io_cell_count = total_width*total_height-func_cell_count;

	Cell* cell;

	unsigned int id = 0;

	for(int i = 0; i < func_cell_count; i++) {
		cell = cell_create_empty(chip, FUNC);
		cell->id = id;
		list_add_last(chip->cells, cell);
		list_add_last(chip->func_cells, cell);
		id++;
	}

	int x = 0, y = 0;
	for(int i = 0; i < io_cell_count; i++) {
		cell = cell_create_empty(chip, IO);
		cell->id = id;
		cell->placed = true;
		cell->x = x;
		cell->y = y;
		list_add_last(chip->cells, cell);
		list_add_last(chip->io_cells, cell);
		id++;

		x++;
		if(y < total_height-io_ring_depth && y >= io_ring_depth) {
			if(x < total_width-io_ring_depth && x >= io_ring_depth)
				x = total_width-io_ring_depth;
			else if(x >= total_width) {
				x = 0;
				y++;
			}
		} else if(x >= total_width) {
			x = 0;
			y++;
		}
	}

	chip->filled = true;
}

Chip* chip_copy(Chip* chip) {
	Chip* new_chip = chip_create_empty(chip->func_cell_width, chip->func_cell_height, chip->io_ring_depth);

	new_chip->filled = chip->filled;
	new_chip->io_cells_connected = chip->io_cells_connected;
	new_chip->func_cells_connected = chip->func_cells_connected;

	Cell* cell;
	Cell* new_cell;

	Cell** new_cell_arr = malloc(sizeof(Cell*)*list_size(chip->cells));

	for(ListNode* current = chip->func_cells->first; current != NULL; current = current->next) {
		cell = current->data;
		new_cell = cell_create_empty(new_chip, cell->type);
		new_cell->id = cell->id;
		new_cell->placed = cell->placed;
		new_cell->x = cell->x;
		new_cell->y = cell->y;
		new_cell->type = cell->type;
		list_add_last(new_chip->func_cells, new_cell);
		new_cell_arr[new_cell->id] = new_cell;
	}

	for(ListNode* current = chip->io_cells->first; current != NULL; current = current->next) {
		cell = current->data;
		new_cell = cell_create_empty(new_chip, cell->type);
		new_cell->id = cell->id;
		new_cell->placed = cell->placed;
		new_cell->x = cell->x;
		new_cell->y = cell->y;
		new_cell->type = cell->type;
		list_add_last(new_chip->io_cells, new_cell);
		new_cell_arr[new_cell->id] = new_cell;
	}

	for(ListNode* current = chip->cells->first; current != NULL; current = current->next)
		list_add_last(new_chip->cells, new_cell_arr[((Cell*) current->data)->id]);

	Cell* cell_current;
	Cell* new_cell_current;
	Cell* cell_conn;
	Cell* new_cell_conn;
	for(ListNode* current = chip->cells->first; current != NULL; current = current->next) {
		cell_current = current->data;
		new_cell_current = new_cell_arr[cell_current->id];

		for(ListNode* conn = cell_current->connections->first; conn != NULL; conn = conn->next) {
			cell_conn = conn->data;
			new_cell_conn = new_cell_arr[cell_conn->id];

			if(!cell_is_connected(new_cell_current, new_cell_conn))
				cell_connect(new_cell_current, new_cell_conn);
		}
	}

	free(new_cell_arr);

	return new_chip;
}

void chip_free(Chip* chip) {
	for(ListNode* node = chip->cells->first; node != NULL; node = node->next)
		cell_free(node->data);
	list_free(chip->cells);
	list_free(chip->func_cells);
	list_free(chip->io_cells);
	free(chip);
}

bool chip_create_connections(Chip* chip, unsigned int io_cell_connection_count, double avg_func_cell_connection_count) {
	if(chip->filled != true)
		return false;

	unsigned int io_cell_count;
	void** io_cells = list_to_array(chip->io_cells, &io_cell_count);
	
	unsigned int func_cell_count;
	void** func_cells = list_to_array(chip->func_cells, &func_cell_count);

	Cell* a;
	Cell* b;

	unsigned int i, j, p, q;

	for(i = 0; i < io_cell_count; i++) {
		a = io_cells[i];
		for(j = 0; j < io_cell_connection_count; j++) {
			q = randombytes_uniform(func_cell_count);
			b = func_cells[q];
			if(!cell_connect(a, b))
				return false;
		}
	}

	chip->io_cells_connected = true;

	int conn_count = round(func_cell_count*avg_func_cell_connection_count-io_cell_count*io_cell_connection_count);

#ifdef CONNECTION_THRESHOLD
	int conn_thresh = func_cell_count*(func_cell_count-1)/(func_cell_count > 2 ? 4 : 2);
	if(conn_count < conn_thresh)
		conn_count = conn_thresh;
#endif

#ifdef DEBUG_CONNECTION_COUNT
	printf("No. of connections between functional cells: %d\n", conn_count);
#endif

	for(i = 0; i < conn_count; i++) {
		p = randombytes_uniform(func_cell_count);
		q = randombytes_uniform(func_cell_count);
		while(p == q) {
			q = randombytes_uniform(func_cell_count);
		}
		a = func_cells[p];
		b = func_cells[q];
		while(cell_is_connected(a, b)) {
			p = randombytes_uniform(func_cell_count);
			q = randombytes_uniform(func_cell_count);
			while(p == q) {
				q = randombytes_uniform(func_cell_count);
			}
			a = func_cells[p];
			b = func_cells[q];
		}
		if(!cell_connect(a, b))
			return false;
	}

	chip->func_cells_connected = true;

	free(io_cells);
	free(func_cells);

#ifdef DEBUG
	chip_print(chip);
#endif

	return true;
}

unsigned int chip_get_opt_metric(Chip* chip) {
	unsigned int ret = 0;
	for(ListNode* i = chip->cells->first; i != NULL; i = i->next)
		ret += cell_get_opt_metric(i->data);
	return ret;
}

void chip_print(Chip* chip) {
	unsigned int cell_count;
	void** cell_arr = list_to_array(chip->cells, &cell_count);

	Cell* cell;
	for(unsigned int i = 0; i < cell_count; i++) {
		cell = cell_arr[i];
		printf("%02d/%s", cell->id, cell->type == IO ? "R" : cell->type == FUNC ? "F" : "U");
		if(cell->placed)
			printf("@(%d,%d):", cell->x, cell->y);
		else
			printf(":");
		for(ListNode* i = cell->connections->first; i != NULL; i = i->next) {
			printf(" %d%s", ((Cell*) i->data)->id, ((Cell*) i->data)->type == IO ? "R" : ((Cell*) i->data)->type == FUNC ? "F" : "U");
			if(i->next != NULL)
				printf(",");
		}
		printf("\n");
	}

	free(cell_arr);
}

Cell* cell_create_empty(Chip* chip, CellType type) {
	Cell* ret = malloc(sizeof(Cell));
	ret->chip = chip;
	ret->id = 0;
	ret->placed = false;
	ret->type = type;
	ret->connections = list_create_empty();
	return ret;
}

void cell_free(Cell* cell) {
	list_free(cell->connections);
	free(cell);
}

unsigned int cell_get_distance(Cell* a, Cell* b) {
	return abs(a->x - b->x)+abs(a->y - b->y);
}

unsigned int cell_get_opt_metric(Cell* cell) {
	unsigned int ret = 0;
	for(ListNode* i = cell->connections->first; i != NULL; i = i->next)
		ret += cell_get_distance(cell, i->data);
	return ret;
}

bool cell_is_connected(Cell* a, Cell* b) {
	return list_search_from_first(a->connections, b) != NULL;
}

bool cell_connect(Cell* a, Cell* b) {
	if(a->chip != b->chip)
		return false;

	list_add_last(a->connections, b);
	list_add_last(b->connections, a);
	
	return true;
}
