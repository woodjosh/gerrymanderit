/*************************************************************************************
Author:      Joshua Woods
Uniqname:    woodjosh
Date:        04/10/2018
Lab Section: 102
Project #:   HACK
Filename:    Tract.hpp
 
COMPILE WITH: g++ -std=c++11 <FILE> -o <FILEOUT>

Program Description: Assigns information to the Tract struct. To use it in the program, 
just do: vector<Tract> Tracts = getTractData(); and Tracts will contain all of the 
census Tracts in Illinois with their information filled with county wide voting data.
Takes about 8 seconds to run through right now.  
  
*************************************************************************************/
#ifndef TRACT_H
#define TRACT_H
#include <string>
#include <vector>

using namespace std;

typedef struct {
	string tractid;
	double Vdem, Vgop;	
	double intPtLat, intPtLon;
	double area;
	vector<int> neighbours; //Indices of the neighboring Tracts
	string countyId;
	string countyName;  
	string districtId;
	string districtName; 
	int population;
	int district;
} Tract;

typedef struct {
	string countyName;
	int Vdem; 
	int Vgop; 	
} ctyVote;

typedef struct {
	string districtId;
	string districtName;
	string countyId;
	string countyName;  
	int popCount; 	
} nameIdCorr;

//Returns a vector of Tracts with all of the filled in information. 
vector<Tract> getTractData();
//Adds county names and ids to a struct made for reading from the txt file
vector<nameIdCorr> countyNameId();
//adds county name, dem votes and rep votes to a vector of vectors of strings
vector<ctyVote> getVoteDataCty();
//Returns position of n-th occurence of a char in a string
int strpos(string haystack, char needle, int nth);
//displays nameIdCorr at index n
void displayNameIdCorr(vector<nameIdCorr> nameId, int n);
//displays voteData at index n 
void displayVoteData(vector<vector<string> > vote, int n);
//displays Tract at index n
void displayTract(vector<Tract>, int n);

//these are not currently used, but were attempts for using precinct level data
vector<nameIdCorr> districtNameId();
vector<vector<string> > getVoteData();
vector<vector<string> > getVoteDataNew();

#endif
