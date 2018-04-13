//
//  main.cpp
//  inputNei
//
//  Created by Yuxi Xie on 4/7/18.
//  Copyright © 2018 Yuxi Xie. All rights reserved.
//



#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <stdlib.h>
#include <block.hpp>
using namespace std;


void inputNeighbor(vector<Block> &block, vector < vector<string> > neighbors, vector<string> blockid)
{
    vector<int> neighborIndex;
    int blocknum = (unsigned)block.size();
    for(int k = 0; k<blocknum; k++)
    {
    neighborIndex.clear();
    int idIndex = 0;// for the block
    string findblockid = block[k].blockid;
    for (int i = 0; i<blocknum;i++)
    {
        if(blockid[i] == findblockid)
        {
            idIndex = i;
            break;
            }
        }
    
    int sz = (unsigned)neighbors[idIndex].size();
    for(int i = 0; i<sz; i++)
    {
        for(int j = 0; j<blocknum;j++)
        {
            if(blockid[j] == neighbors[idIndex][i])
            {
                neighborIndex.push_back(j);
                break;
            }
        }
    }
    
    block[k].neighbours = neighborIndex;// should be index of neighbours
    }
}
    

vector<string> blockid(451600); // in main()
vector<string> neighborId(100); //in main()
vector < vector<string> > neighbors(451600, neighborId);// in main()

void readneighbor(vector<string> &blockid, vector< vector <string> > &neighbors) {
    vector<int> numOfNeighbor(451600);
    string in;
    string ids;
    
    ifstream queenfile;
    queenfile.open("queen.gal");
    int i = 0;
    getline(queenfile,in);
    
    while(!queenfile.fail())
    {
        blockid[i]=in.substr(0,15);
        numOfNeighbor[i]=stod(in.substr(16,2));
        //cout<<numOfNeighbor[i]<<endl;
        getline(queenfile, ids);
        int j = 0;
        while(j<numOfNeighbor[i])
        {
            string sub = ids.substr(j*16,15);
            neighbors[i][j] = sub;
            //cout<<neighbors[i][j]<<endl;
            j++;
        }
        i++;
        getline(queenfile,in);
    }
    
}
