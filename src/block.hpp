#ifndef BLOCK_H
#define BLOCK_H
#include <string>
#include <vector>

using namespace std;

typedef struct {
	string blockid;
	int Vdem, Vgop;	
	double intPtLat, intPtLon;
	int area;
	vector<int> neighbours; //Indices of the neighboring blocks
	int district;
	int population; 
} Block;
#endif
