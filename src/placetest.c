#include<flags.h>

#include<stdio.h>

#include<algo/bruteforce.c>
#include<algo/annealing.c>
#include<util/list.c>
#include<util/cell.c>

#define CELL_FUNC_WIDTH							3
#define CELL_FUNC_HEIGHT						3
#define CELL_IO_RING_DEPTH						1
#define CONNECTIONS_NUM_PER_IO_CELL				1
#define CONNECTIONS_AVG_NUM_PER_FUNC_CELL		3.5

#define ANNEALING_START_TEMP					150.0
#define ANNEALING_SCHEDULE						-0.5
#define ANNEALING_TIMEOUT						10

int main(int argc, char* argv[]) {
	Chip* chip = chip_create_filled(CELL_FUNC_WIDTH, CELL_FUNC_HEIGHT, CELL_IO_RING_DEPTH);
	chip_create_connections(chip, CONNECTIONS_NUM_PER_IO_CELL, CONNECTIONS_AVG_NUM_PER_FUNC_CELL);

	printf("Randomized chip design:\n");
	chip_print(chip);
	printf("\n");

#ifdef ENABLE_BRUTEFORCE
	Chip* bruteforce_chip = chip_copy(chip);

	bruteforce_place(bruteforce_chip);

	printf("Resultant optimization metric (bruteforce): %d\n", chip_get_opt_metric(bruteforce_chip));
	printf("Chip design (bruteforce):\n");
	chip_print(bruteforce_chip);
	printf("\n");
#endif

#ifdef ENABLE_ANNEALING
	Chip* annealing_chip = chip_copy(chip);

	annealing_place(annealing_chip, ANNEALING_START_TEMP, ANNEALING_SCHEDULE, ANNEALING_TIMEOUT);

	printf("Resultant optimization metric (annealing): %d\n", chip_get_opt_metric(annealing_chip));
	printf("Chip design (annealing):\n");
	chip_print(annealing_chip);
	printf("\n");
#endif
}
