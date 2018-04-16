#ifndef GENETICOPTIMIZE
#define GENETICOPTIMIZE
#include <vector>
#include "tract.hpp"
#include <iostream>
#include <vector>



int calc_pop_stdev(vector <Tract> &map, unsigned int* districts, int num_districts);
vector<bool> non_cont_pop(vector<Tract> &map, unsigned int* districts, int num_districts, vector<vector<int> > edges);
vector<double> evaluateScore(vector <Tract> &map, unsigned int* &districts, int num_districts);
void optimize(vector<Tract> &map, int initialMaps, int num_districts, int num_generations, int offspring);

#endif
