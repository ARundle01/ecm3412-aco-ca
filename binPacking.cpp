#include "binPacking.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <iostream>
#include <string>
#include <vector>
#include <limits>
#include <cmath>
#include <random>
#include <iomanip>
#include <algorithm>
#include <numeric>
#include <chrono>
#include "binPackingExceptions.hpp"

/* Main function run by the executable */
int main(int argc, char const *argv[])
{
    try
    {
        // Check for invalid number of args
        if (argc != 4)
            throw new InvalidArgsException;

        // Check formatting of problem type
        string problemTypeStr = argv[1];
        int problemType = stoi(problemTypeStr);

        // Check that problem type is 1 or 2
        if (problemType != 1 and problemType != 2)
        {
            throw new InvalidProblemException;
        }

        // Check formatting of number of ants
        string numAntsStr = argv[2];
        int numAnts = stoi(numAntsStr);

        // Check that number of ants is positive
        if (numAnts < 1)
            throw new InvalidNumAntsException;

        // Check formatting of evaporation rate
        string evaporationStr = argv[3];
        float evaporation = stof(evaporationStr);

        // Check that evaporation is positive
        if (evaporation < 0)
            throw new InvalidEvaporationRateException;

        // Passed all exception checks, now start the 5 ACO trials
        for (int i = 0; i < 5; i++)
            start(problemType, numAnts, evaporation);

        // Output when all 5 ACO trials are complete
        cout << "ACO Trial Complete" << endl;
    }
    catch (invalid_argument const &e)
    {
        // Exception for arguments of invalid data type
        cerr << "Error: Cannot convert char to int in " << e.what() << endl;
        cout << "Problem Types and Number of Ants must be an Integer\nEvaporation Rate must be a float" << endl;
        exit(-1);
    }
    catch (out_of_range const &e)
    {
        // Error where evaporation rate or number of ants is out of range
        cerr << "Error: Out of Range in " << e.what() << endl;
        cout << "Floats (Evaporation Rate) must be in range "
            << numeric_limits<float>::min() << " / "
            << numeric_limits<float>::max() << endl;
            
        cout << "Ints (Number of Ants) must be in range "
            << numeric_limits<int>::min() << " / "
            << numeric_limits<int>::max() << endl;
    }
    catch (InvalidArgsException *e)
    {
        // Exception where the wrong number of arguments are supplied
        cerr << e->what() << endl;
        cout << "Usage: " << argv[0] << " PROBLEM NUM_ANTS EVAPORATION_RATE" << endl;
        exit(-1);
    }
    catch (InvalidProblemException *e)
    {
        // Exception where problem type not 1 or 2 is supplied
        cerr << e->what() << endl;
        cout << "Problem Type must be 1 or 2" << endl;
        exit(-1);
    }
    catch (InvalidNumAntsException *e)
    {
        // Exception where number of ants is <= 0
        cerr << e->what() << endl;
        cout << "Number of Ants must be greater than 0" << endl;
        exit(-1);
    }
    catch (InvalidEvaporationRateException *e)
    {
        // Exception where evaporation rate is <= 0
        cerr << e->what() << endl;
        cout << "Evaporation Rate must be greater than 0" << endl;
        exit(-1);
    }
}

void start(int problemType, int numAnts, float evaporation)
{
    // Output given problem type, number of ants and evaporation rate
    cout << "Problem Type: " << problemType << endl;
    cout << "Number of Ants: " << numAnts << endl;
    cout << "Evaporation Rate: " << evaporation << endl;

    // Set up constant values (number of items is always 500)
    int numItems = 500;
    int itemWeights [500];

    // Initialise number of bins counter
    int numBins = 0;

    // If BPP1, set number of bins to 10 and initialise the linear item weights
    if (problemType == 1)
    {
        numBins = 10;
        for (int i = 0; i < 500; i++)
        {
            itemWeights[i] = i + 1;
        }
    }
    // If BPP2, set number of bins to 50 and initialise the exponential item weights
    else if (problemType == 2)
    {
        numBins = 50;
        for (int i = 0; i < 500; i++)
        {
            itemWeights[i] = pow(i+1, 2);
        }
    }
    
    // Set up random pheromone selection variables and seed RNG
    const int FLOAT_MIN = 0;
    const int FLOAT_MAX = 1;
    srand(time(nullptr));

    // Calculate number of edges and create a vector of edges
    int numEdges = (numBins * numItems) + 2;
    vector<Edge> edges;
    edges.reserve(numEdges);

    // Create a vector of nodes and populate with index number
    vector<int> nodes(numItems * numBins);
    iota(begin(nodes), end(nodes), 1);
    
    // Create a vector of vectors, as well as a temporary vector for manipulation
    vector< vector<int> > subVecs;
    vector<int> temp;

    // Split vector of all nodes into sub-vectors of size `numBins`
    // Each sub-vector is equivalent to each layer in the construction graph
    int lastNum = 0;
    for (int i = 1; i < numItems + 1; i++)
    {
        for (int j = 1; j < numBins + 1; j++)
        {
            temp.push_back(j + lastNum);
        }
        subVecs.push_back(temp);
        lastNum += numBins;
        temp.clear();
    }

    // Create start (root) node 0
    for (auto &nextNode: subVecs.at(0))
    {
        float pheromone = FLOAT_MIN + (float)(rand()) / ((float)(RAND_MAX/(FLOAT_MAX - FLOAT_MIN)));
        edges.push_back({0, nextNode, pheromone, 1});
    }

    // Iterate over all nodes, find sub-vector they belong to
    for (auto &node: nodes)
    {
        for (auto &subVec: subVecs)
        {
            // If the current node is found in the current sub-vector
            if (binary_search(subVec.begin(), subVec.end(), node))
            {
                // Calculate the index of said node in sub-vector
                auto it = find(subVecs.begin(), subVecs.end(), subVec);
                int index = it - subVecs.begin();

                // If the node is not in the final layer
                if (index != numItems - 1)
                {
                    // Calculate the bin for the node
                    auto binit = find(subVec.begin(), subVec.end(), node);
                    int bin = binit - subVec.begin() + 1;

                    // Create an edge from current node to every node in the next layer (sub-vector)
                    for (auto &nextNode: subVecs.at(index + 1))
                    {
                        float pheromone = FLOAT_MIN + (float)(rand()) / ((float)(RAND_MAX/(FLOAT_MAX - FLOAT_MIN)));
                        edges.push_back({node, nextNode, pheromone, bin});
                    }
                }
                // If the node is in the final layer, it is only connected to one node
                else
                {
                    // Calculate bin for the node
                    auto binit = find(subVec.begin(), subVec.end(), node);
                    int bin = binit - subVec.begin() + 1;
                    
                    // Create a single edge from current node to the single output node
                    int nextNode = (numItems * numBins) + 1;
                    edges.push_back({node, nextNode, 1.0, bin});
                }
                break;
            }
        }
    }

    // Create a new construction graph using the previously calculated edges
    Graph acoGraph(edges, (numItems * numBins + 2), numBins, problemType);

    // This tracks the best fitness found in the trial
    double best = pow(numItems, 2) * numBins;

    // Initialise a vector of paths
    vector< vector<int> > paths;

    // Initialise a vector of integer fitnesses
    vector<int> fitnesses;

    // Initialise a single vector for the current path
    vector<int> path;

    // Create a system_clock variable to track time
    auto start = chrono::system_clock::now();
    // Get the current time before running trials
    time_t startTime = chrono::system_clock::to_time_t(start);
    // Output start time of current trial
    cout << "Started computation at: " << ctime(&startTime) << endl;

    // Start loop of 10,000 fitness evaluations
    for (int i = 0; i < 10000; i++)
    {
        // Generate a new path for each ant
        for (int i = 0; i < numAnts; i++)
        {
            // Generate a new path from the ACO construction graph
            path = acoGraph.generatePath();
            // Add path to vector of paths
            paths.push_back(path);

            // Calculate the fitness of the current path
            int fitness = acoGraph.getFitness();
            // Add fitness to vector of fitnesses
            fitnesses.push_back(fitness);
        }

        // Update the pheromones for each ant path
        for (int ant = 0; ant < paths.size(); ant++)
        {
            acoGraph.updatePheromone(paths.at(ant), fitnesses.at(ant));
        }
        // Evaporate all pheromones on the graph
        acoGraph.evaporatePheromone(evaporation);

        // Get best fitness for current fitness evaluation
        double tempBest = *min_element(fitnesses.begin(), fitnesses.end());
        // Assign temporary best as global best if better (smaller)
        if (tempBest < best)
        {
            best = tempBest;
        }
    }
    // Get current system time for end of trial
    auto end = chrono::system_clock::now();
    time_t endTime = chrono::system_clock::to_time_t(end);
    // Output end of trial time
    cout << "Finished computation at: " << ctime(&endTime) << endl;

    // Calculate the time difference (in seconds) between start and end time of trial
    std::chrono::duration<double> elapsedSeconds = end - start;
    cout << "Elapsed Time: " << elapsedSeconds.count() << endl;

    // Output best ant fitness of trial
    cout << "Best Ant Fitness: " << best << endl;
}

void printGraph(Graph const &graph, unsigned int numItems)
{    
    // For each edge in the adjacency list for a given node
    for (Triple vertex: graph.adjList.at(numItems))
    {
        // Print out each edge
        cout << "(" << numItems << ", " << get<0>(vertex) << ", " << get<1>(vertex) << ", " << get<2>(vertex) << ") " << endl;
    }
}