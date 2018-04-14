#include "tract.hpp"
#include <map>
#include <iostream> 
#include <fstream> 
#include "csvstream.h"

using namespace std;

/*************************************************************************************
Returns a vector of Tracts with all of the filled in information. 
Pulls information from Tract_Pop_Loc.csv, PrecinctDataIl.csv, and VTDIlNoSpace.txt 
*************************************************************************************/
vector<Tract> getTractData(){
	vector<Tract> TractData(3124); 
	//open file
	csvstream censusIn("Tract_Pop_Loc.csv"); 
	
	map<string, string> row; 
	
	//exTract Tractid, intPtLat, intPtLon,district, and population
	int n = 0; 
	while(censusIn >> row){
		//Tractid
		TractData.at(n).Tractid = row["GEOID10"];
		//intPtLat
		string s = row["INTPTLAT"]; 
		if(s.at(0) == '+')	
			TractData.at(n).intPtLat = stod(s);
		else 
			TractData.at(n).intPtLat = -1* stod(s);
		//intPtLon
		string t = row["INTPTLON"]; 
		if(s.at(0) == '+')	
			TractData.at(n).intPtLon = stod(t);
		else 
			TractData.at(n).intPtLon = -1* stod(t);
		//countyId
		TractData.at(n).countyId = row["COUNTY"];
		//population
		TractData.at(n).population = stod(row["P0010001"]);
		n++;  
	}
	//add county name
	vector<nameIdCorr> nameId = countyNameId(); 
	
	for(int i = 0; i < TractData.size(); i++){
		for(int j = 0; j < nameId.size(); j++){
			if(TractData.at(i).countyId == nameId.at(j).countyId){
				TractData.at(i).countyName = nameId.at(j).countyName;
				nameId.at(j).popCount+=TractData.at(i).population;  
			}
		}
	}
    //add vote data to Tract if countyName matches 
    //NOTE: Since vote and nameId are in the same order, can reference the same index 
    //      of both and get corresponding info (population per county in this case)
    vector<ctyVote> vote = getVoteDataCty(); 

    for(int i = 0; i < vote.size(); i++){
		for(int j = 0; j < TractData.size(); j++){
			if(TractData.at(j).countyName == vote.at(i).countyName){
				TractData.at(j).Vdem = (((double)vote.at(i).Vdem) / ((double)nameId.at(i).popCount)) * ((double)TractData.at(j).population);
				TractData.at(j).Vgop = (((double)vote.at(i).Vgop) / ((double)nameId.at(i).popCount)) * ((double)TractData.at(j).population);  
			}
		}
	}

	return TractData; 
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
		temp.popCount = 0;
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
vector<ctyVote> getVoteDataCty(){
	vector<ctyVote> voteDataCty;  
	ctyVote temp;
	csvstream voteIn("CtyVote.csv"); 
	map<string, string> row; 
	
	while(voteIn >> row){
		temp.countyName = row["COUNTY"]+" County";
		temp.Vdem = stod(row["DEM"],nullptr);
		temp.Vgop =	stod(row["REP"],nullptr);
		
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
		 << "PopCt:       " << nameId.at(n).popCount << "\n";
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
displays Tract at index n
*************************************************************************************/
void displayTract(vector<Tract> TractData, int n){
	cout << "TractID:     " << TractData.at(n).Tractid << "\n" 
		 << "IntPtLat:    " << TractData.at(n).intPtLat << "\n"
		 << "IntPtLon:    " << TractData.at(n).intPtLon << "\n"
		 << "CountyId:    " << TractData.at(n).countyId << "\n"
		 << "CountyName:  " << TractData.at(n).countyName << "\n"
		 << "Population:  " << TractData.at(n).population << "\n"
		 << "Vdem:        " << TractData.at(n).Vdem << "\n" 
		 << "Vgop:        " << TractData.at(n).Vgop<< "\n"; 
	return; 
}

/*************************************************************************************
 *************************************************************************************
Everything below this line was made in attempt to read precinct level data and 
is not being used
*************************************************************************************
*************************************************************************************/

//this was to be used in getTractData()
	/*
	//add district name if districtId and nameId match in the correlation file
	//add one to Tract count so we know how many census Tracts are in each precinct
	//make more efficient if time
	for(int i = 0; i < nameId.size(); i++){
		for(int j = 0; j < TractData.size(); j++){
			if(TractData.at(j).districtId == nameId.at(i).districtId 
			   && TractData.at(j).countyId == nameId.at(i).countyId){
				TractData.at(j).districtName = nameId.at(i).districtName;
				nameId.at(i).TractCount ++; 
				//while we are here, initialize Vgop and Vdem to 0
				TractData.at(j).Vgop = 0; 
				TractData.at(j).Vdem = 0;  
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
		temp.popCount = 0; 
				
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
