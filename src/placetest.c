#include<flags.h>
#include<config.h>

#include<stdio.h>

#include<algo/bruteforce.c>
#include<algo/annealing.c>
#include<util/list.c>
#include<util/cell.c>
#include<util/clock.c>

#include "placetest.h"

int main(int argc, char* argv[]) {

#ifdef ENABLE_SANITY_CHECK
	printf("Sanity check:\n");
	sanity_check();
#endif
	
#ifdef ENABLE_BRUTEFORCE_COMPLEXITY_TEST
	printf("Bruteforce complexity test:\n");
	bruteforce_complexity_test();
#endif
	
#ifdef ENABLE_ANNEALING_SCHEDULE_TEST_3x3
	printf("Annealing schedule choice test (3x3):\n");
	annealing_schedule_test(3, 3);
#endif
	
#ifdef ENABLE_ANNEALING_SCHEDULE_TEST_10x10
	printf("Annealing schedule choice test (10x10):\n");
	annealing_schedule_test(10, 10);
#endif

#ifdef ENABLE_ANNEALING_COMPLEXITY_TEST
	printf("Annealing complexity test:\n");
	annealing_complexity_test();
#endif

#ifdef ENABLE_LARGE_TEST
	printf("Annealing large test:\n");
	annealing_large_test();
#endif

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
	printf("Bruteforce sanity check:\n");
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
	chip_free(bruteforce_chip);
#endif

#ifdef SANITY_CHECK_ENABLE_ANNEALING
	printf("Annealing sanity check:\n");
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
	chip_free(annealing_chip);
#endif

	chip_free(chip);
}

void bruteforce_complexity_test() {
	bool tick = true;

	unsigned int n = 2, m = 3;

	Chip* chip;
	float sec;
	while(n*m <= COMPLEXITY_TEST_MAX_SIZE_BRUTEFORCE) {
		for(unsigned int i = 0; i < COMPLEXITY_TEST_RETRIES; i++) {
			chip = chip_create_filled(n, m, COMPLEXITY_TEST_IO_RING_DEPTH);
			chip_create_connections(chip, COMPLEXITY_TEST_CONNECTIONS_NUM_PER_IO_CELL, COMPLEXITY_TEST_CONNECTIONS_AVG_NUM_PER_FUNC_CELL);

			clock_start();
			bruteforce_place(chip);
			sec = clock_stop();

			chip_free(chip);

			printf("%d(%d): %f\n", n*m, i+1, sec);
		}

		if(tick)
			n += 1;
		else
			m += 1;
		tick = !tick;
	}
}

void annealing_schedule_test(unsigned int cell_func_width, unsigned int cell_func_height) {
	Chip* chip;
	Chip* work_chip;
	float sec;
	double schedule;
	int opt;

	for(unsigned int i = 0; i < SCHEDULE_TEST_RETRIES; i++) {
		printf(">TRY %d\n", i+1);

		chip = chip_create_filled(cell_func_width, cell_func_height, SCHEDULE_TEST_IO_RING_DEPTH);
		chip_create_connections(chip, SCHEDULE_TEST_CONNECTIONS_NUM_PER_IO_CELL, SCHEDULE_TEST_CONNECTIONS_AVG_NUM_PER_FUNC_CELL);

		schedule = SCHEDULE_TEST_START_SCHEDULE;
		while(fabs(schedule) > fabs(SCHEDULE_TEST_STOP_SCHEDULE)) {
			work_chip = chip_copy(chip);

			clock_start();
			annealing_place(work_chip, SCHEDULE_TEST_START_TEMPERATURE, schedule, SCHEDULE_TEST_TIMEOUT);
			sec = clock_stop();

			opt = chip_get_opt_metric(work_chip);
			chip_free(work_chip);

			printf("%lf: %f sec -> %d\n", schedule, sec, opt);

			schedule /= 2;
		}
	}
}

void annealing_complexity_test() {
	bool tick = true;

	unsigned int n = 2, m = 3;

	Chip* chip;
	float sec;
	while(n*m <= COMPLEXITY_TEST_MAX_SIZE_ANNEALING) {
		for(unsigned int i = 0; i < COMPLEXITY_TEST_RETRIES; i++) {
			chip = chip_create_filled(n, m, COMPLEXITY_TEST_IO_RING_DEPTH);
			chip_create_connections(chip, COMPLEXITY_TEST_CONNECTIONS_NUM_PER_IO_CELL, COMPLEXITY_TEST_CONNECTIONS_AVG_NUM_PER_FUNC_CELL);

			clock_start();
			annealing_place(chip, COMPLEXITY_TEST_START_TEMPERATURE, COMPLEXITY_TEST_SCHEDULE, COMPLEXITY_TEST_TIMEOUT);
			sec = clock_stop();

			chip_free(chip);

			printf("%d(%d): %f\n", n*m, i+1, sec);
		}

		if(tick)
			n += 1;
		else
			m += 1;
		tick = !tick;
	}
}

void annealing_large_test() {
	Chip* chip;
	float sec;

	chip = chip_create_filled(LARGE_TEST_CELL_FUNC_WIDTH, LARGE_TEST_CELL_FUNC_HEIGHT, LARGE_TEST_IO_RING_DEPTH);
	chip_create_connections(chip, LARGE_TEST_CONNECTIONS_NUM_PER_IO_CELL, LARGE_TEST_CONNECTIONS_AVG_NUM_PER_FUNC_CELL);

	clock_start();
	annealing_place(chip, LARGE_TEST_START_TEMPERATURE, LARGE_TEST_SCHEDULE, LARGE_TEST_TIMEOUT);
	sec = clock_stop();

	chip_free(chip);

	printf("%d: %f\n", LARGE_TEST_CELL_FUNC_WIDTH*LARGE_TEST_CELL_FUNC_HEIGHT, sec);
}
