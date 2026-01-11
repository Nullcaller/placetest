#ifndef PLACETEST_UTIL_CELL_H
#define PLACETEST_UTIL_CELL_H

#include<stdbool.h>

#include<util/list.h>

typedef enum e_cell_type CellType;

typedef struct s_cell Cell;

typedef struct s_chip Chip;

enum e_cell_type {
	IO, FUNC
};

struct s_cell {
	Chip* chip;
	unsigned int id;
	bool placed;
	int x, y;
	CellType type;
	List* connections;
};

struct s_chip {
	List* cells;
	List* func_cells;
	List* io_cells;
	bool filled;
	bool io_cells_connected;
	bool func_cells_connected;
	int func_cell_width, func_cell_height;
	int io_ring_depth;
};

Chip* chip_create_empty(int func_cell_width, int func_cell_height, int io_ring_depth);

Chip* chip_create_filled(int func_cell_width, int func_cell_height, int io_ring_depth);

Chip* chip_copy(Chip* chip);

void chip_free(Chip* chip);

bool chip_create_connections(Chip* chip, unsigned int io_cell_connection_count, double avg_func_cell_connection_count);

unsigned int chip_get_opt_metric(Chip* chip);

void chip_print(Chip* chip);

Cell* cell_create_empty(Chip* chip, CellType type);

void cell_free(Cell* cell);

unsigned int cell_get_distance(Cell* a, Cell* b);

unsigned int cell_get_opt_metric(Cell* cell);

bool cell_is_connected(Cell* a, Cell* b);

bool cell_connect(Cell* a, Cell* b);

#endif
