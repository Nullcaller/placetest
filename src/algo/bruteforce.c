#include<stdlib.h>
#include<string.h>

#include<util/list.h>

#include "bruteforce.h"

static void bruteforce_place_(Chip* chip, unsigned int depth, unsigned int func_cell_count, void** func_cells, unsigned int** arrays, unsigned int* cur_placement, int* placement_x, int* placement_y, unsigned int* unoptimum, unsigned int* optimum, unsigned int* unoptimum_placement, unsigned int* optimum_placement, unsigned int* optimum_count) {
	unsigned int* array = arrays[depth];
	unsigned int* next_array;

#ifdef EXTRA_DEBUG
	printf("%d: ", depth);
	for(unsigned int i = 0; i < func_cell_count-depth; i++)
		printf(" %d", array[i]);
	printf("\n");
#endif

	if(depth + 1 != func_cell_count)
		next_array = arrays[depth+1];
	else
		next_array = NULL;

	unsigned int cur_place;
	unsigned int skip = 0;
	unsigned int opt_temp;
	for(unsigned int i = 0; i < func_cell_count-depth; i++) {
		cur_place = array[i];
		cur_placement[depth] = cur_place;
		((Cell*) func_cells[depth])->x = placement_x[cur_place];
		((Cell*) func_cells[depth])->y = placement_y[cur_place];
		if(depth + 1 == func_cell_count) {
			if((opt_temp = chip_get_opt_metric(chip)) < *optimum) {
				*optimum = opt_temp;
				*optimum_count = 0;
				memcpy(optimum_placement, cur_placement, func_cell_count*sizeof(unsigned int));
			}
			if(opt_temp == *optimum)
				*optimum_count += 1;
			if(opt_temp > *unoptimum) {
				*unoptimum = opt_temp;
				memcpy(unoptimum_placement, cur_placement, func_cell_count*sizeof(unsigned int));
			}
#ifdef EXTRA_DEBUG
			printf("(%d",cur_placement[0]);
			for(unsigned int j = 1; j < func_cell_count; j++)
				printf(",%d",cur_placement[j]);
			printf("): %d\n", opt_temp);
#endif
		} else {
			skip = 0;
			for(unsigned int j = 0; j < func_cell_count-depth-1; j++) {
				if(array[j] == cur_place)
					skip = 1;
				next_array[j] = array[j+skip];
			}
			bruteforce_place_(chip, depth+1, func_cell_count, func_cells, arrays, cur_placement, placement_x, placement_y, unoptimum, optimum, unoptimum_placement, optimum_placement, optimum_count);
		}
	}
}

void bruteforce_place(Chip* chip) {
	unsigned int func_cell_count;
	void** func_cells = list_to_array(chip->func_cells, &func_cell_count);

	unsigned int** arrays = malloc(sizeof(unsigned int*)*func_cell_count);
	for(unsigned int i = 0; i < func_cell_count; i++)
		arrays[i] = malloc(sizeof(unsigned int)*(func_cell_count-i));
	for(unsigned int i = 0; i < func_cell_count; i++)
		arrays[0][i] = i;

	unsigned int unoptimum = 0;
	unsigned int optimum = -1;
	unsigned int optimum_count = 0;
	unsigned int* unoptimum_placement = malloc(sizeof(unsigned int)*func_cell_count);
	unsigned int* optimum_placement = malloc(sizeof(unsigned int)*func_cell_count);
	unsigned int* cur_placement = malloc(sizeof(unsigned int)*func_cell_count);
	
	int* placement_x = malloc(sizeof(int)*func_cell_count);
	int* placement_y = malloc(sizeof(int)*func_cell_count);

	unsigned int it = 0;
	for(int y = 0; y < chip->func_cell_height; y++)
		for(int x = 0; x < chip->func_cell_width; x++) {
			placement_x[it] = x + chip->io_ring_depth;
			placement_y[it] = y + chip->io_ring_depth;
			it++;
		}

	bruteforce_place_(chip, 0, func_cell_count, func_cells, arrays, cur_placement, placement_x, placement_y, &unoptimum, &optimum, unoptimum_placement, optimum_placement, &optimum_count);

#ifdef ENABLE_BRUTEFORCE_OPTIMUM_COUNT
	printf("Optimum count: %d\n", optimum_count);
#endif

#ifdef ENABLE_BRUTEFORCE_DISOPTIMUM
	for(unsigned int i = 0; i < func_cell_count; i++) {
		((Cell*) func_cells[i])->x = placement_x[unoptimum_placement[i]];
		((Cell*) func_cells[i])->y = placement_y[unoptimum_placement[i]];
		((Cell*) func_cells[i])->placed = true;
	}

	printf("Maximally unoptimal cell placement chip (%d):\n", unoptimum);
	chip_print(chip);
	printf("\n");
#endif

	for(unsigned int i = 0; i < func_cell_count; i++) {
		((Cell*) func_cells[i])->x = placement_x[optimum_placement[i]];
		((Cell*) func_cells[i])->y = placement_y[optimum_placement[i]];
		((Cell*) func_cells[i])->placed = true;
	}

	for(unsigned int i = 0; i < func_cell_count; i++)
		free(arrays[i]);
	free(arrays);

	free(unoptimum_placement);
	free(optimum_placement);
	free(cur_placement);

	free(placement_x);
	free(placement_y);
}
