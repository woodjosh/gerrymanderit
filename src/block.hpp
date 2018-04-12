/*************************************************************************************
Author:      Joshua Woods
Uniqname:    woodjosh
Date:        04/10/2018
Lab Section: 102
Project #:   HACK
Filename:    block.hpp
 
COMPILE WITH: g++ -std=c++11 <FILE> -o <FILEOUT>

Program Description: Assigns information to the block struct. To use it in the program, 
just do: vector<Block> blocks = getBlockData(); and blocks will contain all of the 
census blocks in Illinois with their information filled with county wide voting data.
Takes about 8 seconds to run through right now.  
  
*************************************************************************************/
#ifndef BLOCK_H
#define BLOCK_H
#include <string>
#include <vector>

using namespace std;

typedef struct {
	string blockid;
	double Vdem, Vgop;	
	double intPtLat, intPtLon;
	int area;
	vector<int> neighbours; //Indices of the neighboring blocks
	string countyId;
	string countyName;  
	string districtId;
	string districtName; 
	int population;
	int district;
} Block;

typedef struct {
	string districtId;
	string districtName;
	string countyId;
	string countyName;  
	int blockCount; 	
} nameIdCorr;

//Returns a vector of blocks with all of the filled in information. 
vector<Block> getBlockData();
//Adds county names and ids to a struct made for reading from the txt file
vector<nameIdCorr> countyNameId();
//adds county name, dem votes and rep votes to a vector of vectors of strings
vector<vector<string> > getVoteDataCty();
//Returns position of n-th occurence of a char in a string
int strpos(string haystack, char needle, int nth);
//displays nameIdCorr at index n
void displayNameIdCorr(vector<nameIdCorr> nameId, int n);
//displays voteData at index n 
void displayVoteData(vector<vector<string> > vote, int n);
//displays block at index n
void displayBlock(vector<Block>, int n);

//these are not currently used, but were attempts for using precinct level data
vector<nameIdCorr> districtNameId();
vector<vector<string> > getVoteData();
vector<vector<string> > getVoteDataNew();

#endif