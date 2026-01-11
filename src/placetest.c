#include<flags.h>

#include<stdio.h>

#include<algo/bruteforce.c>
#include<algo/annealing.c>
#include<util/list.c>
#include<util/cell.c>
#include<util/clock.c>

#include "placetest.h"

#define SANITY_CHECK_CELL_FUNC_WIDTH						3
#define SANITY_CHECK_CELL_FUNC_HEIGHT						3
#define SANITY_CHECK_CELL_IO_RING_DEPTH						1
#define SANITY_CHECK_CONNECTIONS_NUM_PER_IO_CELL			1
#define SANITY_CHECK_CONNECTIONS_AVG_NUM_PER_FUNC_CELL		3.5

#define SANITY_CHECK_ANNEALING_START_TEMP					150.0
#define SANITY_CHECK_ANNEALING_SCHEDULE						-0.5
#define SANITY_CHECK_ANNEALING_TIMEOUT						10

int main(int argc, char* argv[]) {
	sanity_check();
}

void sanity_check() {
	Chip* chip = chip_create_filled(SANITY_CHECK_CELL_FUNC_WIDTH, SANITY_CHECK_CELL_FUNC_HEIGHT, SANITY_CHECK_CELL_IO_RING_DEPTH);
	chip_create_connections(chip, SANITY_CHECK_CONNECTIONS_NUM_PER_IO_CELL, SANITY_CHECK_CONNECTIONS_AVG_NUM_PER_FUNC_CELL);

#ifdef SANITY_CHECK_PRINTOUT_CHIPS
	printf("Randomized chip design:\n");
	chip_print(chip);
	printf("\n");
#endif

	float sec;

#ifdef SANITY_CHECK_ENABLE_BRUTEFORCE
	Chip* bruteforce_chip = chip_copy(chip);

	clock_start();
	bruteforce_place(bruteforce_chip);
	sec = clock_stop();

	printf("Finished in %f seconds.\n", sec);
	printf("Resultant optimization metric (bruteforce): %d\n", chip_get_opt_metric(bruteforce_chip));
#ifdef SANITY_CHECK_PRINTOUT_CHIPS
	printf("Chip design (bruteforce):\n");
	chip_print(bruteforce_chip);
	printf("\n");
#endif
#endif

#ifdef SANITY_CHECK_ENABLE_ANNEALING
	Chip* annealing_chip = chip_copy(chip);

	clock_start();
	annealing_place(annealing_chip, SANITY_CHECK_ANNEALING_START_TEMP, SANITY_CHECK_ANNEALING_SCHEDULE, SANITY_CHECK_ANNEALING_TIMEOUT);
	sec = clock_stop();

	printf("Finished in %f seconds.\n", sec);

	printf("Resultant optimization metric (annealing): %d\n", chip_get_opt_metric(annealing_chip));
#ifdef SANITY_CHECK_PRINTOUT_CHIPS
	printf("Chip design (annealing):\n");
	chip_print(annealing_chip);
	printf("\n");
#endif
#endif
}
