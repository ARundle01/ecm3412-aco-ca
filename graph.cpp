#include "graph.hpp"
#include <iostream>
#include <stdio.h>
#include <functional>
#include <algorithm>
#include <random>
#include <cmath>

Graph::Graph(std::vector<Edge> const &edges, unsigned int numNodes, unsigned int numBins, unsigned int problemType)
{
    // The constructor for the Graph class

    // Initialise the adjacency list
    this->adjList.resize(numNodes);

    // Initialise the list of bins, where each element is the weight of the bin and index is the bin number
    this->bins.resize(numBins);

    // Initialise the class attributes
    this->numBins = numBins;
    this->numNodes = numNodes;
    this->problemType = problemType;
    this->numItems = (numNodes - 2) / numBins;

    // For each edge in edges
    for (auto &edge: edges)
    {
        // Extract each attribute of the edge
        int source = edge.source;
        int destination = edge.destination;
        float pheromone = edge.pheromone;
        int bin = edge.bin;
        
        // Create a Triple containing destination, pheromone and bin
        Triple content = std::make_tuple(destination, pheromone, bin);

        // Place Triple at index same as the source node (creating the adjacency list representation)
        this->adjList[source].push_back(content);
    }
}

void Graph::addToBin(unsigned int bin, unsigned int weight)
{
    // Add a given weight to the give bin
    this->bins[bin-1] += weight;
}

int Graph::getFitness()
{
    // Retrieve the fitness of the graph, where fitness is heaviest bin - lightest bin
    std::vector<unsigned int>::iterator min = std::min_element(this->bins.begin(), this->bins.end());
    std::vector<unsigned int>::iterator max = std::max_element(this->bins.begin(), this->bins.end());

    int fitness = *max - *min;
    return fitness;
}

int Graph::generateIdx(unsigned int item)
{
    // Function to pick a random index for the next node, weighted based on the pheromone of each edge

    // Initialise a vector of cumulative sums
    std::vector<float> weightCum;
    float cumSum = 0;

    // Create then weighted cumulative sum vector
    for (auto &choice: this->adjList[item])
    {
        cumSum += std::get<1>(choice);
        weightCum.push_back(cumSum);
    }

    // Pick a random number in range 0 to sum of all pheromones
    float random = ((float) rand()) / (float) RAND_MAX;
    float r = random * cumSum;

    // Get index of the first element in weighted cumulative vector that is <= random number
    auto idx = std::lower_bound(weightCum.begin(), weightCum.end(), r);

    // Return the index of the next node to be visited
    return std::distance(weightCum.begin(), idx);
}

std::vector<int> Graph::generatePath()
{
    // Start by emptying all bins (as bins might contain leftover items)
    this->emptyBins();

    // Initialise trackers for current node and current item
    int currNode = 0;
    int currItem = 0;

    // Calculate the final node of the whole graph
    int endNode = numNodes - 1;

    // Calculate the index of the final layer
    int finalLayerNode = endNode - this->numBins;

    // Initialise the vector to store the path (a vector of each visited node)
    std::vector<int> path;

    // Create a flag to signal end of path generation
    bool done = false;

    // Begin flag-based loop
    while (!done)
    {
        // If at final node
        if (currNode == endNode)
        {
            // Push final node to path
            path.push_back(currNode);
            // Set flag to end loop
            done = true;
        }
        // If current node is in final layer
        else if (currNode >= finalLayerNode)
        {
            // Index of next node is always 0 as there is only the single final node to move to
            int idx = 0;
            // Get the details of the edge to the next node
            Triple nextNode = this->adjList.at(currNode).at(idx);
            int destination = std::get<0>(nextNode);
            int bin = std::get<2>(nextNode);

            // If BPP1, add linear item weight
            if (this->problemType == 1)
            {
                int weight = currItem + 1;
                this->addToBin(bin, weight);
            }
            // If BPP2, add exponential item weight
            else
            {
                int weight = std::pow((currItem + 1), 2);
                this->addToBin(bin, weight);
            }
            // Add current node to path
            path.push_back(currNode);
            // Set current node to next node
            currNode = destination;
            // Add one to current item number
            currItem += 1;
        }
        // If current node is not in the final layer
        else
        {
            // Get index of next node
            int idx = this->generateIdx(currNode);

            // Get details of edge to next node
            Triple nextNode = this->adjList.at(currNode).at(idx);
            int destination = std::get<0>(nextNode);
            int bin = std::get<2>(nextNode);

            // If BPP1, add linear weight to bin
            if (this->problemType == 1)
            {
                int weight = currItem + 1;
                this->addToBin(bin, weight);
            }
            // If BPP2, add exponential weight to bin
            else
            {
                int weight = std::pow((currItem + 1), 2);
                this->addToBin(bin, weight);
            }

            // Add current node to path
            path.push_back(currNode);
            // Set current node to next node
            currNode = destination;
            // Increase current item number
            currItem += 1;
        }
    }
    // Return the finished path
    return path;
}

void Graph::evaporatePheromone(float evaporationRate)
{
    // Evaporates the pheromone based on given evaporation rate, for all edges
    // For every node in the adjacency list
    for (auto &node: this->adjList)
    {
        // For every edge from that node
        for (auto &edge: node)
        {
            // Get the destination and bin
            int destination = std::get<0>(edge);
            int bin = std::get<2>(edge);

            // Multiply pheromone by given evaporation rate
            float pheromone = std::get<1>(edge);
            pheromone *= evaporationRate;
            
            // Create a new edge
            Triple newEdge = std::make_tuple(destination, pheromone, bin);
            // Swap contents of old edge inplace of new edge
            std::swap(edge, newEdge);
        }
    }
}

void Graph::updatePheromone(std::vector<int> path, int fitness)
{
    // Updates all pheromones in a given path, based on a given fitness

    // Calculate the amount to update pheromones by (100/fitness)
    double fitnessDiff = 100.0 / (double) fitness;

    // Iterate over every node in the path
    for (int node = 0; node < path.size(); node++)
    {
        // Get current node in path
        int source = path.at(node);

        // If not the final node in path
        if (source != numNodes - 1)
        {
            // For every edge in adjacency list for that node
            for (auto &edge: this->adjList.at(source))
            {
                // Get the destination in that edge
                int destination = std::get<0>(edge);
                // If the destination in edge is also in the path
                if (destination == path.at(node + 1))
                {
                    // Get the bin
                    int bin = std::get<2>(edge);

                    // Add update value to pheromone
                    float pheromone = std::get<1>(edge);
                    pheromone += fitnessDiff;
                    
                    // Create a new edge with new pheromone value
                    Triple newEdge = std::make_tuple(destination, pheromone, bin);
                    // Swap old edge inplace with new edge
                    std::swap(edge, newEdge);
                }
            } 
        }
        // If final node in path
        else if (source == numNodes - 1)
        {
            // No further edges, so no further pheromones to update
            break;
        }    
    }
}

void Graph::emptyBins()
{
    // Sets weight of all bins to 0
    for (auto &bin: bins)
    {
        bin = 0;
    }
}