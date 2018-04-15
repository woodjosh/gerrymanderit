#include "tract.hpp"
#include "gen_random_map.hpp"
#include "stdlib.h"
#include "stdio.h"
#include "time.h"
#include "signal.h"
#include <string.h>

using namespace std;
void sig_handler(int code){
	printf("\nSignal %d received\n", code);
	exit(code);
}
vector<Tract> generate_test_Tracts(){
	vector<Tract> map;
	int width = 100;
	int height = 50;
	for(int y = 0; y < height; y++){
		for(int x = 0; x < width; x++){
			Tract b;
			b.area = 10;
			b.Tractid = "";
			b.intPtLat = x;
			b.intPtLon = y;
			b.population = (rand() % 10) * 2;
			int voting_pop = b.population * rand()/RAND_MAX;
			b.Vdem = voting_pop * rand()/RAND_MAX;
			b.Vgop = voting_pop - b.Vdem;
			if(x < width-1){
				b.neighbours.push_back((x+1) + y * width);	//right
				/*if(y < height-1){
					b.neighbours.push_back((x+1) + (y+1) * width); //down right
				}
				if(y != 0){
					b.neighbours.push_back((x+1) + (y-1)*width); //up right
				}*/
			}
			if(x != 0){
				b.neighbours.push_back((x-1) + y * width); //left
				/*if(y < height-1){
					b.neighbours.push_back((x-1) + (y+1) * width); //down left
				}
				if(y != 0){
					b.neighbours.push_back((x-1) + (y-1) * width);//up left
				}*/
			}
			if(y < height-1){
				b.neighbours.push_back(x + (y+1) * width); //down
			}
			if(y != 0){
				b.neighbours.push_back(x + (y-1)*width);//up
			}
			map.push_back(b);
		}
	}
	return map;
}

int main(){
	signal(SIGABRT, sig_handler);
	signal(SIGINT, sig_handler);
	signal(SIGSEGV, sig_handler);
	signal(SIGFPE, sig_handler);
	signal(SIGILL, sig_handler);
	signal(SIGTERM, sig_handler);
	srand(time(NULL));
	vector<Tract> map = generate_test_Tracts();

	unsigned int* districts = gen_random_map_sa(map, 3, 0.01, pop_diff_m);
	if(districts == NULL){
		printf("Error generating districts");
		return 1;
	}
	for(int y = 0; y < 10; y++){
		for(int x = 0; x < 10; x++){
			printf("%d ", map[x + y * 10].district);
		}
		putchar('\n');
	}
	return 0;
}