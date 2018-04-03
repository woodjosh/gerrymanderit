#ifndef __BLOCK_H__
#define __BLOCK_H__
#include <string>
typedef struct {
	std::string blockid;
	int Vdem, Vgop;	
	double intPtLat, intPtLon;
	int area;
	int district;
} Block;
#endif
