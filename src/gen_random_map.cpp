#include "gen_random_map.hpp"
#include "stdlib.h"
#include "math.h"
#include "stdio.h"
#include <algorithm>
#include "assert.h"

//TODO: Optimize!!!

using namespace std;

double block_distance(Block a, Block b){
	double latDiff = a.intPtLat - b.intPtLat;
	double lonDiff = a.intPtLon - b.intPtLon;
	return sqrt(latDiff * latDiff + lonDiff * lonDiff);
}

//the heuristic we'll use to make sure they are the same populations
int pop_diff_m(int *district_pops, int num_districts, int target_pop_per_district){
	int diff = 0;
	for(int i = 0; i<num_districts; i++){
		diff += abs(district_pops[i] - target_pop_per_district);
	}
	return diff;
}

int pop_diff_e(int *district_pops, int num_districts, int target_pop_per_district){
	long long sum = 0;
	for(int i = 0; i<num_districts; i++){
		int diff = district_pops[i] - target_pop_per_district;
		sum += diff * diff;
	}
	return (int)sqrt(sum);
}

vector<vector<int> > gen_voronoi_map(vector<Block> &map, int num_districts, int *district_pops){
	printf("Generating A random voronoi districting...");
	//choose initial seeds
	size_t map_sz = map.size();
	
	int *init_seeds = (int*) malloc(sizeof(int) * num_districts);
	if(init_seeds == NULL){
		fprintf(stderr, "\nCould not allocate memory for initial seeds in gen_voronoi_map");
		return vector<vector<int> >(0);
	}

	vector<vector<int> > districts = vector<vector<int> >(num_districts);

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
		//printf("adding %d to district %d\n", next, i);
		districts[i].push_back(next);
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
		districts[closest_center].push_back(i);
		district_pops[closest_center] += map[i].population;
	}
	free(init_seeds);
	
	printf("Done\n");
	return districts;
}
void calculate_edges(vector<Block> &map, vector<vector<int> > &edges){
	size_t map_sz = map.size();
	//printf("Calculating edges\n");
	for(int i = 0; i <edges.size(); i++){
		edges[i].clear();
	}
	for(int i = 0; i < map_sz; i++){
		bool is_edge = false;
		int cur_district = map[i].district;
		for(int j = 0; j<map[i].neighbours.size(); j++){
			if(map[map[i].neighbours[j]].district != cur_district){
				//printf("index %u is an edge\n", i);
				is_edge = true;
				break;
			}
		}
		if(is_edge){
			edges[cur_district].push_back(i);
		}
	}
}
/*void update_edges(vector<Block> &map, vector<int> &edges){
	for(int i = 0; i < edges.size(); i++){
		bool is_edge = false;
		int cur_district = map[edges[i]].district;
		assert(map[edges[i]].neighbours.size() > 0);
		for(int j = 0; j<map[edges[i]].neighbours.size(); j++){
			if(map[map[edges[i]].neighbours[j]].district != cur_district){
				bool is_edge = true;
				break;
			}
		}
		if(!is_edge){
			printf("Erasing edges at %d\n", i);
			edges.erase(edges.begin() + i); //most of the time, the edges will still be edges, so not really a performance penalty
		}
	}
}*/

void print_map(vector<Block> &map){
	int width = 10, height = 10;
	for(int y = 0; y < height; y++){
		for(int x = 0; x < width; x++){
			printf("%d ", map[x + y * width].district);
		}
		putchar('\n');
	}
	putchar('\n');
}

vector<vector<int> > gen_random_map(vector<Block> &map, int num_districts, double tolerance, int (*pd)(int*, int, int)){
	printf("Generating a map with districts of equal population...\n");

	int *district_pops = (int*) calloc(num_districts, sizeof(int));
	if(district_pops == NULL){
		fprintf(stderr, "Could not allocate district_pops in gen_random_map\n");
		return vector<vector<int> >(0);
	}
	vector<vector<int> > districts = gen_voronoi_map(map, num_districts, district_pops);
	if(districts.size() == 0){
		free(district_pops);
		return districts;
	}
	//calculate the edges of the districts
	vector<vector<int> > edges = vector<vector<int> >(num_districts);
	//printf("Calculating Edges\n");
	calculate_edges(map, edges);
	
	//get target district pop
	int total_pop = 0;
	for(int i = 0; i < num_districts; i++){
		total_pop += district_pops[i];
	}
	int target_pop_per_district = total_pop / num_districts;
	//printf("total population: %d\tTarget Pop per district: %d\n", total_pop, target_pop_per_district);
	
	//keep updating until the average population difference is less than the tolerance
	int mod_block; //index of block to add or remove to district
	int new_distr, old_distr;
	bool add = false; //true if adding, false otherwise
	int cur_pop_diff = pd(district_pops, num_districts, target_pop_per_district);
	int num_iters = 0;
	while(cur_pop_diff / num_districts > tolerance*total_pop){
		//printf("Current population difference: %d\n", cur_pop_diff);
		//suggest modification
		unsigned int d = rand() % num_districts; //district to be modified
		//printf("Modifying district %d\n", d);
		if(district_pops[d] > target_pop_per_district){//too many people in the district
			//printf("Too many people in district %d\n");
			add = false;//removing not adding
			mod_block = edges[d][rand() % edges[d].size()]; //remove an edge block
			//printf("removing block %d from district %d...", mod_block, d);
			district_pops[d] -= map[mod_block].population;
			districts[d].erase(remove(districts[d].begin(), districts[d].end(), mod_block), districts[d].end());
			//printf("...removed\n");
			//give to other district
			int num_neighbors = map[mod_block].neighbours.size();
			do{
				new_distr = map[map[mod_block].neighbours[rand() % num_neighbors]].district;
			}while(new_distr == d);//repeat until the district != d
			//printf("Giving block %d to district %d..", mod_block, new_distr);
			districts[new_distr].push_back(mod_block);
			district_pops[new_distr] += map[mod_block].population;
			map[mod_block].district = new_distr;
			//printf(".done\n");
		}else{//not enough people in the district
			//printf("Not enough people in district %d\n", d);
			add = true;
			//take a block from a neighboring district
			int ed = edges[d][rand() % edges[d].size()];
			int num_neighbors = map[ed].neighbours.size();
			//printf("Taking from edge block #%d with %d neighbors\n", ed, num_neighbors);
			do{
				mod_block = map[ed].neighbours[rand() % num_neighbors];
				old_distr = map[mod_block].district; //old district in this case
			}while(old_distr == d);
			//printf("taking block %d from district %d\n", mod_block, old_distr);
			districts[old_distr].erase(remove(districts[old_distr].begin(), districts[old_distr].end(), mod_block),
									 districts[old_distr].end());
			district_pops[old_distr] -= map[mod_block].population;
			
			//add it to our district
			districts[d].push_back(mod_block);
			district_pops[d] += map[mod_block].population;
			map[mod_block].district = d;
		}
		//check if imporvement
		int new_pop_diff = pd(district_pops, num_districts, target_pop_per_district);
		if(new_pop_diff < cur_pop_diff){
			//printf("We have improvement\n");
			cur_pop_diff = new_pop_diff;
			//printf("recalculating edges\n");
			calculate_edges(map, edges);
			//print_map(map);
		}else{
			//printf("No improvement\n");
			//undo our work
			if(add){ //if we took a block
				districts[d].pop_back();
				districts[old_distr].push_back(mod_block);

				district_pops[old_distr] += map[mod_block].population;
				district_pops[d] -= map[mod_block].population;
			
				map[mod_block].district = old_distr;
			}else{ //if we gave a block
				districts[new_distr].pop_back();
				districts[d].push_back(mod_block);

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

	free(district_pops);

	printf("...Done\n");
	return districts;
}