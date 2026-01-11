#include "clock.h"

void clock_start() {
	rec = clock();
}

float clock_stop() {
	clock_t end = clock();
	return (float) (end-rec) / CLOCKS_PER_SEC;
}