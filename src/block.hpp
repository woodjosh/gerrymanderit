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
#include <map>
#include <iostream> 
#include <fstream> 
#include "csvstream.h"
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
/*************************************************************************************
Returns a vector of blocks with all of the filled in information. 
Pulls information from CensusDataIl.csv, PrecinctDataIl.csv, and VTDIlNoSpace.txt 
*************************************************************************************/
vector<Block> getBlockData(){
	vector<Block> blockData(451554); 
	//open file
	csvstream censusIn("CensusDataIl.csv"); 
	
	map<string, string> row; 
	
	//extract blockid, intPtLat, intPtLon,district, and population
	int n = 0; 
	while(censusIn >> row){
		//blockid
		blockData.at(n).blockid = row["GEOID10"];
		//intPtLat
		string s = row["INTPTLAT"]; 
		if(s.at(0) == '+')	
			blockData.at(n).intPtLat = stod(s);
		else 
			blockData.at(n).intPtLat = -1* stod(s);
		//intPtLon
		string t = row["INTPTLON"]; 
		if(s.at(0) == '+')	
			blockData.at(n).intPtLon = stod(t);
		else 
			blockData.at(n).intPtLon = -1* stod(t);
		//districtId
		blockData.at(n).districtId = row["VTD"];
		//countyId
		blockData.at(n).countyId = row["COUNTY"];
		//population
		blockData.at(n).population = stod(row["P0010001"]);
		n++;  
	}
	//add county name
	vector<nameIdCorr> nameId = countyNameId(); 
	
	for(int i = 0; i < blockData.size(); i++){
		for(int j = 0; j < nameId.size(); j++){
			if(blockData.at(i).countyId == nameId.at(j).countyId){
				blockData.at(i).countyName = nameId.at(j).countyName;
				nameId.at(j).blockCount++;  
			}
		}
	}
  
    //add vote data to block if countyName matches 
    //NOTE: Since vote and nameId are in the same order, can reference the same index 
    //      of both and get corresponding info (blocks per county in this case)
    vector<vector<string> > vote = getVoteDataCty(); 

    for(int i = 0; i < vote.size(); i++){
		for(int j = 0; j < blockData.size(); j++){
			if(blockData.at(j).countyName == vote.at(i).at(0) && vote.at(i).at(0)!=""){
				blockData.at(j).Vdem = stod(vote.at(i).at(1),nullptr)/nameId.at(i).blockCount;
				blockData.at(j).Vgop = stod(vote.at(i).at(2),nullptr)/nameId.at(i).blockCount;  
			}
		}
	}

	return blockData; 
}

/*************************************************************************************
adds county name, county id, to a vector of struct nameIdCorr
comes from "CountyID.csv" a correlation file for county ids and county names
*************************************************************************************/
vector<nameIdCorr> countyNameId(){
	vector<nameIdCorr> nameId; 
	nameIdCorr temp;
	csvstream voteIn("CountyID.csv"); 
	map<string, string> row; 
	
	while(voteIn >> row){
		temp.countyId = row["ID"].substr(2,3); 
		temp.countyName = row["NAME"] + " County"; 
		temp.blockCount = 0;
		nameId.push_back(temp); 		
	}
	
	return nameId; 	
}

/*************************************************************************************
adds county name, dem votes and rep votes to a vector of vectors of strings
comes from "CtyVote.csv" a file containing 2016 election info
column 0: county name
column 1: democratic votes
column 2: republican votes
*************************************************************************************/
vector<vector<string> > getVoteDataCty(){
	vector<vector<string> > voteDataCty;  
	vector<string> temp;
	csvstream voteIn("CtyVote.csv"); 
	map<string, string> row; 
	
	while(voteIn >> row){
		temp = {row["COUNTY"]+" County",
				row["DEM"],
				row["REP"]};
		
		voteDataCty.push_back(temp); 		
	}
 
	return voteDataCty; 	
}

/*************************************************************************************
Returns position of n-th occurence of a char in a string.
*************************************************************************************/
int strpos(string haystack, char needle, int nth)
{// Will return position of n-th occurence of a char in a string.
        string read ="";    // A string that will contain the read part of the haystack
        for (int i=1 ; i<nth+1 ; ++i)
        {
                size_t found = haystack.find(needle);
                // the read part of the haystack is stocked in the read string
                read += haystack.substr(0,found+1); 
                // remove the read part of the haystack up to the i-th needle
                haystack.erase(0, found+1);     
                if (i == nth)
                {
                        return read.size();
                }
        }
        return -1;
}

/*************************************************************************************
displays nameIdCorr at index n 
*************************************************************************************/
void displayNameIdCorr(vector<nameIdCorr> nameId, int n){
	cout << "CountyName:  " << nameId.at(n).countyName<< "\n" 
	     << "CountyId:    " << nameId.at(n).countyId << "\n"
		 << "BlockCt:     " << nameId.at(n).blockCount << "\n";
	return; 
}

/*************************************************************************************
displays voteData at index n 
*************************************************************************************/
void displayVoteData(vector<vector<string> > vote, int n){
	cout << "CountyName:  " << vote.at(n).at(0) << "\n" 
		 << "Dem votes:   " << vote.at(n).at(1) << "\n" 
		 << "Rep votes:   " << vote.at(n).at(2) << "\n";
	return; 
}

/*************************************************************************************
displays block at index n
*************************************************************************************/
void displayBlock(vector<Block> blockData, int n){
	cout << "BlockID:     " << blockData.at(n).blockid << "\n" 
		 << "IntPtLat:    " << blockData.at(n).intPtLat << "\n"
		 << "IntPtLon:    " << blockData.at(n).intPtLon << "\n"
		 << "CountyId:    " << blockData.at(n).countyId << "\n"
		 << "CountyName:  " << blockData.at(n).countyName << "\n"
		 << "Population:  " << blockData.at(n).population << "\n"
		 << "Vdem:        " << blockData.at(n).Vdem << "\n" 
		 << "Vgop:        " << blockData.at(n).Vgop<< "\n"; 
	return; 
}

/*************************************************************************************
 *************************************************************************************
Everything below this line was made in attempt to read precinct level data and 
is not being used
*************************************************************************************
*************************************************************************************/

//this was to be used in getBlockData()
	/*
	//add district name if districtId and nameId match in the correlation file
	//add one to block count so we know how many census blocks are in each precinct
	//make more efficient if time
	for(int i = 0; i < nameId.size(); i++){
		for(int j = 0; j < blockData.size(); j++){
			if(blockData.at(j).districtId == nameId.at(i).districtId 
			   && blockData.at(j).countyId == nameId.at(i).countyId){
				blockData.at(j).districtName = nameId.at(i).districtName;
				nameId.at(i).blockCount ++; 
				//while we are here, initialize Vgop and Vdem to 0
				blockData.at(j).Vgop = 0; 
				blockData.at(j).Vdem = 0;  
			}
		}
	}
	*/
	
/*************************************************************************************
adds district name, district id, county id, to a vector of struct nameIdCorr
comes from "VTDIl.txt" a correlation file for district ids and district numbers
*************************************************************************************/
vector<nameIdCorr> districtNameId(){
	vector<nameIdCorr> nameId; 
	
	ifstream infile("VTDIl.txt"); 
	 
	while(!infile.fail()){
		string s;
		getline(infile, s); 
		if(s == "")
			break;  
		int second = strpos(s,'|',2); 
		int third = strpos(s,'|',3); 
		int fourth = strpos(s,'|',4); 
		int fifth = strpos(s,'|',5);
		//remove "Voting District" from district name
		string district = s.substr(fifth,s.find("\r\n")-1); 
		if(district.find("Voting District") != district.npos)
			district = district.substr(0, district.size() - 18); 
		
		nameIdCorr temp;
		temp.districtId = s.substr(fourth,fifth-fourth-1);
		temp.districtName = district;
		temp.countyId = s.substr(second,3);
		temp.countyName = s.substr(third, fourth-third-1);  
		temp.blockCount = 0; 
				
		nameId.push_back(temp); 
	}
	infile.close(); 
	return nameId; 	
}

/*************************************************************************************
adds district name, dem votes and rep votes to a vector of vectors of strings
comes from "RepublicanPrecinctData.csv" and "DemocratPrecinctData.csv"
both files contain precinct names and votes for their respective parties
column 0: district name
column 1: democratic votes
column 2: republican votes
*************************************************************************************/
vector<vector<string> > getVoteDataNew(){
	vector<vector<string> > voteData;  
	vector<string> temp;
	csvstream voteInRep("RepublicanPrecinctData.csv"); 
	map<string, string> row1; 
	
	csvstream voteInDem("DemocratPrecinctData.csv"); 
	map<string, string> row2; 
	
	
	while(voteInRep >> row1 && voteInDem >> row2){
		temp = {row2["PrecinctName"],
				row2["VoteCount"],
				row1["VoteCount"]};
		
		voteData.push_back(temp); 		
	}
 
	return voteData; 	
}

/*************************************************************************************
adds district name, dem votes and rep votes to a vector of vectors of strings
comes from "PrecinctDataIl.csv" a file containing 2016 election info
column 0: district name
column 1: democratic votes
column 2: republican votes
*************************************************************************************/
vector<vector<string> > getVoteData(){
	vector<vector<string> > voteData;  
	vector<string> temp;
	csvstream voteIn("PrecinctDataIl.csv"); 
	map<string, string> row; 
	
	while(voteIn >> row){
		temp = {row["PrecinctName"],
				row["HILLARY CLINTON"],
				row["DONALD TRUMP"]};
		
		voteData.push_back(temp); 		
	}
 
	return voteData; 	
}
#endif
