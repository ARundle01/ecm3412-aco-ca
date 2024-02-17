/*-- This header defines the custom struct, type and Graph class definition for graph.cpp --*/
#ifndef _GRAPH_H
#define _GRAPH_H

/*-- Includes --*/
#include <vector>
#include <tuple>

/* Defines the contents of an edge */
struct Edge {
    int source;
    int destination;
    float pheromone;
    int bin;
};

/* Defines a custom 3-Tuple */
typedef std::tuple<int, float, int> Triple;

/* Class representing the construction graph for the Bin Packing Problem */
class Graph
{
    private:
        std::vector<unsigned int> bins;
        unsigned int numBins;
        unsigned int numNodes;
        unsigned int problemType;
        unsigned int numItems;
    public:
        std::vector<std::vector<Triple>> adjList;
        Graph(std::vector<Edge> const &, unsigned int, unsigned int, unsigned int);

        /* Adds a weight to a bin */
        void addToBin(unsigned int, unsigned int);

        /* Gets fitness of the graph */
        int getFitness();

        /* Gets index of next node, weighted on pheromones */
        int generateIdx(unsigned int);

        /* Generates a path */
        std::vector<int> generatePath();

        /* Evaporates pheromones for all edges in graph */
        void evaporatePheromone(float);

        /* Updates pheromones for each edge in given path */
        void updatePheromone(std::vector<int>, int);

        /* Empties all bins in graph */
        void emptyBins();
};

/*-- End Header --*/
#endif