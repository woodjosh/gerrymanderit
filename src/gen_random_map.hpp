#ifndef GEN_RANDOM_MAP_H
#define GEN_RANDOM_MAP_H
#include <vector>
#include "block.hpp"
using namespace std;

//Number of wasted votes, v1 is target party votes, v2 is opposition votes
//TODO move to other file
#define WASTED_VOTES(V1, V2)\
	((V1 > V2) ? V2 : V2 - (V2 + V1)/2)

//WARNING: must free the returned value to avoid memory leak
//NOTE: check if the return value is NULL before using
vector<vector<int> > gen_random_map(vector<Block> &map, int num_districts, double tolerance, int (*pd)(int*, int, int)); 
vector<vector<int> > gen_voronoi_map(vector<Block> &map, int num_districts, int *district_pops);

int pop_diff_m(int *district_pops, int num_districts, int target_pop_per_district);
int pop_diff_e(int*, int, int);

void calculate_edges(vector<Block> &map, vector<vector<int> > &edges);
//void update_edges(vector<Block> &map, vector<int> &edges);
#endif
