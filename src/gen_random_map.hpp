#ifndef GEN_RANDOM_MAP_H
#define GEN_RANDOM_MAP_H
#include <vector>
#include "block.hpp"
#include "stdint.h"
using namespace std;

unsigned int* gen_random_map(vector<Block> &map, unsigned int num_districts, double tolerance, uint64_t (*pd)(uint64_t*, unsigned int, unsigned int)); 
unsigned int* gen_voronoi_map(vector<Block> &map, unsigned int num_districts, uint64_t *district_pops);

uint64_t pop_diff_m(uint64_t *district_pops, unsigned int num_districts, unsigned int target_pop_per_district);
uint64_t pop_diff_e(uint64_t*, unsigned int, unsigned int);

void calculate_edges(vector<Block> &map, vector<vector<int> > &edges);
#endif
