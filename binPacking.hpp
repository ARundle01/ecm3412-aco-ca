// This header defines function prototypes for binPacking.cpp
#ifndef BINPACKING_H
#define BINPACKING_H

/*-- Includes --*/
#include "graph.hpp"

/*-- Function Prototypes --*/

/* Runs a single ACO trial */
void start(int, int, float);

/* Prints all edges from a given node, in a given graph */
void printGraph(Graph const &, unsigned int);

/*-- End of Header --*/
#endif