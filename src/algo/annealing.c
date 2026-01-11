#include<sodium.h>

#include "annealing.h"

#define ANNEALING_FLOAT_CMP_THRESHOLD 0.0001

static void annealing_initialize_chip_(Chip* chip) {
	ListNode* current = chip->func_cells->first;
	Cell* cell_current;
	for(int y = 0; y < chip->func_cell_height; y++)
		for(int x = 0; x < chip->func_cell_width; x++) {
			cell_current = current->data;
			cell_current->x = x + chip->io_ring_depth;
			cell_current->y = y + chip->io_ring_depth;
			current = current->next;
		}
}

static void annealing_do_swap_(Cell* a, Cell* b) {
	int buf_x, buf_y;

	buf_x = a->x;
	buf_y = a->y;
	
	a->x = b->x;
	a->y = b->y;
	
	b->x = buf_x;
	b->y = buf_y;
}

void annealing_place(Chip* chip, double temp, double schedule, unsigned int timeout) {
	annealing_initialize_chip_(chip);

	unsigned int func_cell_count;
	void** func_cells = list_to_array(chip->func_cells, &func_cell_count);

	schedule /= func_cell_count;

	unsigned int p, q;
	int opt = chip_get_opt_metric(chip), new_opt;
	unsigned int tries = 0;
	while(temp > ANNEALING_FLOAT_CMP_THRESHOLD) {
		p = randombytes_uniform(func_cell_count);
		q = randombytes_uniform(func_cell_count);
		
#ifdef EXTRA_DEBUG
		printf("[T=%03.2lf] Attempting swap on %d & %d\n", temp, p, q);
#endif

		annealing_do_swap_(func_cells[p], func_cells[q]);
		new_opt = chip_get_opt_metric(chip);

		if(new_opt-opt >= temp) {
			annealing_do_swap_(func_cells[p], func_cells[q]);
			tries++;
#ifdef DEBUG
			printf("[T=%03.2lf] Swap unsuccessful, tries: %d\n", temp, tries);
#endif
			if(tries >= timeout) {
#ifdef DEBUG
				printf("[T=%03.2lf] Swap unsuccessful, giving up\n", temp);
#endif
				tries = 0;
				temp += schedule;
			}
		} else {
#ifdef EXTRA_DEBUG
			printf("[T=%03.2lf] SWAP SUCCESS\n", temp);
#endif
			tries = 0;
			opt = new_opt;
			temp += schedule;
		}
	}
}
