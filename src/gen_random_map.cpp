#include "gen_random_map.hpp"
#include "stdlib.h"
#include "math.h"
#include "stdio.h"
#include <algorithm>
#include "assert.h"

using namespace std;

double block_distance(Block a, Block b){
	double latDiff = a.intPtLat - b.intPtLat;
	double lonDiff = a.intPtLon - b.intPtLon;
	return sqrt(latDiff * latDiff + lonDiff * lonDiff);
}
#define ABS(X) (X < 0 ? -X : X)
//the heuristic we'll use to make sure they are the same populations
uint64_t pop_diff_m(uint64_t *district_pops, unsigned int num_districts, unsigned int target_pop_per_district){
	uint64_t diff = 0;
	for(int i = 0; i<num_districts; i++){
		diff += ABS((int64_t)(district_pops[i] - target_pop_per_district));
	}
	return diff;
}

uint64_t pop_diff_e(uint64_t *district_pops, unsigned int num_districts, unsigned int target_pop_per_district){
	uint64_t sum = 0;
	for(int i = 0; i<num_districts; i++){
		int diff = district_pops[i] - target_pop_per_district;
		sum += diff * diff;
	}
	return sqrt(sum);
}

unsigned int* gen_voronoi_map(vector<Block> &map, unsigned int num_districts, uint64_t *district_pops){
	printf("Generating A random voronoi districting...");
	//choose initial seeds
	size_t map_sz = map.size();
	
	int *init_seeds = (int*) malloc(sizeof(int) * num_districts);
	if(init_seeds == NULL){
		fprintf(stderr, "\nCould not allocate memory for initial seeds in gen_voronoi_map\n");
		return NULL;
	}

	unsigned int* districts = (unsigned int*) malloc(sizeof(unsigned int) * map.size());
	if(districts == NULL){
		fprintf(stderr, "\nCould not allocate memory for districts in gen_voronoi_map\n");
		free(init_seeds);
		return NULL;
	}

	bool already_chosen;
	for(int i = 0; i < num_districts; i++){
		already_chosen = false;
		int next;
		do{
			next = rand() % map_sz; //generate next index
			for(int j = 0; j<i; j++){//check if has already been chosen
				if(init_seeds[j] == next){
					already_chosen = true;
					break;
				}
			}
		}while(already_chosen);
		init_seeds[i] = next;

		districts[next] = i;
		district_pops[i] += map[next].population;
		map[next].district = i;
	}
	
	//loop over each block and assign it to the closest district
	for(int i = 0; i<map_sz; i++){
		double closest_dist = block_distance(map[init_seeds[0]], map[i]);
		int closest_center = 0;
		for(int j = 1; j < num_districts; j++){
			double dist = block_distance(map[init_seeds[j]], map[i]);
			if(dist < closest_dist){
				closest_dist = dist;
				closest_center = j;
			}
		}
		map[i].district = closest_center;
		districts[i] = closest_center;
		district_pops[closest_center] += map[i].population;
	}
	free(init_seeds);
	
	printf("Done\n");
	return districts;
}
void calculate_edges(vector<Block> &map, vector<vector<int> > &edges){
	size_t map_sz = map.size();
	for(int i = 0; i <edges.size(); i++){
		edges[i].clear();
	}
	for(int i = 0; i < map_sz; i++){
		bool is_edge = false;
		int cur_district = map[i].district;
		for(int j = 0; j<map[i].neighbours.size(); j++){
			if(map[map[i].neighbours[j]].district != cur_district){
				is_edge = true;
				break;
			}
		}
		if(is_edge){
			edges[cur_district].push_back(i);
		}
	}
}

unsigned int* gen_random_map(vector<Block> &map, unsigned int num_districts, double tolerance, uint64_t (*pd)(uint64_t*, unsigned int, unsigned int)){
	printf("Generating a map with districts of equal population...\n");

	uint64_t *district_pops = (uint64_t*) calloc(num_districts, sizeof(uint64_t));
	if(district_pops == NULL){
		fprintf(stderr, "Could not allocate district_pops in gen_random_map\n");
		return NULL;
	}
	unsigned int* districts = gen_voronoi_map(map, num_districts, district_pops);
	if(districts == NULL){
		free(district_pops);
		return NULL;
	}
	//calculate the edges of the districts
	vector<vector<int> > edges = vector<vector<int> >(num_districts);
	calculate_edges(map, edges);
	
	//get target district pop
	unsigned int total_pop = 0;
	for(int i = 0; i < num_districts; i++){
		total_pop += district_pops[i];
	}
	unsigned int target_pop_per_district = total_pop / num_districts;
	printf("Total population: %u\tTarget Population per District: %u\n", total_pop, target_pop_per_district);
	//keep updating until the average population difference is less than the tolerance
	int mod_block; //index of block to add or remove to district
	int new_distr, old_distr;
	bool add = false; //true if adding, false otherwise
	uint64_t cur_pop_diff = pd(district_pops, num_districts, target_pop_per_district);
	unsigned int num_iters = 0;
	while(cur_pop_diff / num_districts > tolerance*total_pop){
		//suggest modification
		unsigned int d = rand() % num_districts; //district to be modified
		if(district_pops[d] > target_pop_per_district){//too many people in the district
			add = false;//removing not adding
			mod_block = edges[d][rand() % edges[d].size()]; //remove an edge block
			district_pops[d] -= map[mod_block].population;
			//give to other district
			int num_neighbors = map[mod_block].neighbours.size();
			do{
				new_distr = map[map[mod_block].neighbours[rand() % num_neighbors]].district;
			}while(new_distr == d);//repeat until the district != d
			district_pops[new_distr] += map[mod_block].population;
			map[mod_block].district = new_distr;
		}else{//not enough people in the district
			add = true;
			//take a block from a neighboring district
			int ed = edges[d][rand() % edges[d].size()];
			int num_neighbors = map[ed].neighbours.size();
			do{
				mod_block = map[ed].neighbours[rand() % num_neighbors];
				old_distr = map[mod_block].district; //old district in this case
			}while(old_distr == d);
			district_pops[old_distr] -= map[mod_block].population;
			
			//add it to our district
			district_pops[d] += map[mod_block].population;
			map[mod_block].district = d;
		}
		//check if imporvement
		uint64_t new_pop_diff = pd(district_pops, num_districts, target_pop_per_district);
		if(new_pop_diff < cur_pop_diff){
			cur_pop_diff = new_pop_diff;
			calculate_edges(map, edges);
		}else{
			//undo our work
			if(add){ //if we took a block
				district_pops[old_distr] += map[mod_block].population;
				district_pops[d] -= map[mod_block].population;
			
				map[mod_block].district = old_distr;
			}else{ //if we gave a block

				district_pops[new_distr] -= map[mod_block].population;
				district_pops[d] += map[mod_block].population;

				map[mod_block].district = d;
			}
		}
		++num_iters;

		if(num_iters % 1000 == 0){
			printf("Iteration: %d\tCurrent Population Difference: %d\n", num_iters, cur_pop_diff);
		}
	}

	size_t map_sz = map.size();
	for(int i = 0; i<map_sz; i++){
		districts[i] = map[i].district;
	}
	printf("...Done\n");
	int avg_pop_diff = cur_pop_diff/num_districts;
	float per_pop_diff = 100 * ((float)avg_pop_diff)/total_pop;
	printf("Total Population Difference: %u\n", cur_pop_diff);
	printf("Average Population Difference: %d(%f%%)\n",avg_pop_diff, per_pop_diff);
	printf("Completed in %d iterations\n", num_iters);

	free(district_pops);
	return districts;
}