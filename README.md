# placetest

This repository contains implementations for the following standard cell placement algorithms:

* Exhaustive search with factorial time complexity
* A simulated annealing algorithm which I presume is, in essence, very similar to what Intel might've used to design the i386 processor, the rest being history

Building the project requires [libsodium](https://doc.libsodium.org/doc). Generating random numbers using libsodium is probably not exactly ideal for performance, as the project obviously doesn't a require a CSPRNG, but I caried over a lot of code from [passutil](https://github.com/Nullcaller/passutil), so I used it for the sake of expedience.

To build the project and launch the executable, navigate to the cloned git repository and simply execute:

```
cmake .
make
./placetest
```

A system running Linux is required.

Configuration define statements are available in `src/flags.h` and `src/placetest.c`, the latter of which is the main source file, linking all other sources and containing the `main` function.

## Assumptions and simplifications

So as not to go too deep into the woods of standard cell placement shenanigans and make this project possible to implement (and run) in a timely manner, a number of key assumptions and simplifications were made.

These I would classify as 'assumptions':

* Chip designs are generated randomly, and thus make no actual sense and aren't grounded in reality
* IO cells are pre-placed in a ring around the functional cell area, each connecting to a pre-determined number of random functional cells (this is quite typical of actual semiconductor devices)
* Functional cells each have a random number of connections, with a certain pre-determined target average connection count per single functional cell
* There are exactly as many functional cells as there are places for the functional cells
* The distance between the cells is the [Manhattan norm](https://en.wikipedia.org/wiki/Taxicab_geometry) of a vector between their centers, also known as `abs(x1-x2)+abs(y1-y2)` (this is just actually how that works in semiconductor devices because of how metallization layers are arranged)

And these I would classify as 'simplifications':

* The grid and the standard cells are square (which means everything is just integer math, which is good for the CPU)
* The standard cells don't have physical locations for inputs/outputs, it is assumed that all connections are made to the center of the standard cell
* The value being optimized by the algorithms in this project is the total length of wire inside the chip, i.e. the sum of lengths of all connections, which I assume isn't the case for real semiconductor devices, as they would probably be optimized for the shortest length of the longest path instead (to reduce the maximum delay and increase the clocking frequency), but as finding the longest path is in and of itself an NP-hard problem, I decided not to tackle it here
