#include "gen_random_map.hpp"
#include "stdlib.h"
#include "math.h"
#include "stdio.h"
#include "assert.h"
#include <iostream>

using namespace std;

vector<Block> testBlocks(int n, int m){
	int N = n;
	int M = m;
	vector<Block> map;
	for(int i = 0; i < N; i++){
		for(int k = 0; k < M; k++){
			int pop = floor(rand()%40);
			double Vdem = floor(pop*(0.3+(rand()%4)/10));
			double Vgop = pop-Vdem;
			pop += 25;
			vector <int> neighbors;
			if(k!=0){
				neighbors.push_back((k-1)+i*N);
			}
			if(k!=M-1){
				neighbors.push_back((k+1)+i*N);
			}
			if(i!=0){
				neighbors.push_back(i*(N-1)+k);
			}
			if(i!=N-1){
				neighbors.push_back(i*(N+1)+k);
			}
			string blank = "placeholder";
			Block b = {blank, Vdem, Vgop, 0.000614*k,  0.000802*i, 50000, neighbors, blank, blank, blank, blank, pop, 1};
			map.push_back(b);
		}
	}
	return map;
}

void printMap(vector <Block> &map, int N, int M){
	for(int i = 0; i < N; i++){
		for(int k = 0; k < M; k++){
			cout << map[i*N+k].population << " " << map[i*N+k].Vdem << " " << map[i*N+k].Vgop << " | ";
		}
		cout << endl;
	}
}
