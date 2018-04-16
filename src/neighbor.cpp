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
#include "tract.hpp"
using namespace std;


void inputNeighbor(vector<Tract> &tract, vector < vector<string> > neighbors, vector<string> tractid)
{
    vector<int> neighborIndex;
    int tractnum = (unsigned)tract.size();
    for(int k = 0; k<tractnum; k++)
    {
    neighborIndex.clear();
    int idIndex = 0;// for the tract
    string findtractid = tract[k].tractid;
    for (int i = 0; i<tractnum;i++)
    {
        if(tractid[i] == findtractid)
        {
            idIndex = i;
            break;
            }
        }
    
    int sz = (unsigned)neighbors[idIndex].size();
    for(int i = 0; i<sz; i++)
    {
        for(int j = 0; j<tractnum;j++)
        {
            if(tractid[j] == neighbors[idIndex][i])
            {
                neighborIndex.push_back(j);
                break;
            }
        }
    }
    
    tract[k].neighbours = neighborIndex;// should be index of neighbours
    }
}
    


void readneighbor(vector<string> &tractid, vector< vector <string> > &neighbors) {
    vector<int> numOfNeighbor(3500);
    string in;
    string ids;
    
    ifstream queenfile;
    queenfile.open("rookTract.gal");
    int i = 0;
    getline(queenfile,in);
    
    while(!queenfile.fail())
    {
        tractid[i]=in.substr(9,11);
        numOfNeighbor[i]=stod(in.substr(21,2));
        //cout<<numOfNeighbor[i]<<endl;
        getline(queenfile, ids);
        int j = 0;
        while(j<numOfNeighbor[i])
        {
            string sub = ids.substr(j*21,20);
            neighbors[i][j] = sub.substr(9,11);
            //cout<<neighbors[i][j]<<endl;
            j++;
        }
        i++;
        getline(queenfile,in);
    }
}   
void inputArea(vector<Tract> &tract) {
        int sz = (unsigned) tract.size();
        vector<string> state(3500);
        vector<string> geoid(3500);
        vector<string> aland(3500);
        vector<string> awater(3500);
        vector<string> aland_sqmi(3500);
        vector<string> awater_sqmi(3500);
        vector<string> lat(3500);
        vector<string> lon(3500);
        vector<string> trash(3500);
        
        ifstream file("area.rtf");
        int i = 0;
        file>>state[i]>>geoid[i]>>aland[i]>>awater[i]>>aland_sqmi[i]>>awater_sqmi[i]>>lat[i]>>lon[i]>>trash[i];
        while (!file.fail())
        {
            for(int j = 0; j<sz; j++)
            {
                if(tract[j].tractid == geoid[i])
                {
                    double area_sqmi = stod(aland_sqmi[i]) + stod(awater_sqmi[i]);
                    tract[j].area = area_sqmi;
                    //cout<<area_sqmi<<endl;
                    break;
                }
            }
            i++;
            file>>state[i]>>geoid[i]>>aland[i]>>awater[i]>>aland_sqmi[i]>>awater_sqmi[i]>>lat[i]>>lon[i]>>trash[i];
        }
    }
