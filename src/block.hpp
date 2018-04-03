#ifndef __BLOCK_H__
#define __BLOCK_H__
#include <string>
#include <vector>
typedef struct {
	std::string blockid;
	int Vdem, Vgop;	
	double intPtLat, intPtLon;
	int area;
	std::vector<Block*> neighbours;
	int district;
} Block;
#endif
