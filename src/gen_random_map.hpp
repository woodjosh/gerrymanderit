#ifndef GEN_RANDOM_MAP_H
#define GEN_RANDOM_MAP_H
#include <vector>
#include "block.hpp"
using namespace std;

int* gen_random_map(vector<Block> &map, int num_districts, double tolerance, int (*pd)(int*, int, int)); 
int* gen_voronoi_map(vector<Block> &map, int num_districts, int *district_pops);

int pop_diff_m(int *district_pops, int num_districts, int target_pop_per_district);
int pop_diff_e(int*, int, int);

void calculate_edges(vector<Block> &map, vector<vector<int> > &edges);
#endif
