#include "gen_random_map.hpp"
#include "stdlib.h"
#include "math.h"
#include "stdio.h"
#include "assert.h"
#include "testBlocks.hpp"

using namespace std;

int main(){
	vector<Block> map = testBlocks(1000, 1000);
	printMap(map, 1000, 1000);
}