#ifndef __BLOCK_H__
#define __BLOCK_H__
#include <string>
#include <vector>

using namspace std

typedef struct {
	string blockid;
	int Vdem, Vgop;	
	double intPtLat, intPtLon;
	int area;
	vector<Block*> neighbours;
	int district;
	int population; 
} Block;
#endif
