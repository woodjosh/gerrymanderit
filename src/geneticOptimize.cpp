#include "tract.hpp"
#include "gen_random_map.hpp"
#include "stdlib.h"
#include "stdio.h"
#include "time.h"
#include "signal.h"
#include <string.h>
#include <algorithm>  
#include <iostream>
#include <vector>
#include <cmath> 


using namespace std;


//Taken from gen_random_map, modified to use districts vector rather htan map
vector< vector<int> > calculate_edges_op(vector<Tract> &map, unsigned int* districts, int num_districts){
	vector<vector<int> > edges = vector<vector<int> >(num_districts);

	for(int i = 0; i < map.size(); i++){
		bool is_edge = false;
		int cur_district = districts[i];

		for(int j = 0; j<map[i].neighbours.size(); j++){
			/*for(int k = 0; k < map[i].neighbours.size(); k++){
				cerr << "spot " << i << " neighbor " << map[i].neighbours[k] << " district " << districts[map[i].neighbours[k]] << endl;
			}*/
			if(districts[map[i].neighbours[j]] != cur_district){
				is_edge = true;
				break;
			}
		}
		if(is_edge){
			edges[cur_district].push_back(i);
		}
	}
	return edges;
}

void add_cont_pop(vector<Tract> &map, unsigned int* districts, vector<int> &marked, int index, int &pop){
	if(marked[index]!=-1){
		marked[index]=-1;
		for(int i = 0; i <map[index].neighbours.size(); i++){
			if(districts[map[index].neighbours[i]]==districts[index]){
				add_cont_pop(map, districts, marked, map[index].neighbours[i], pop);
			}
		}
		pop+=map[index].population;
	}
}
int non_cont_pop(vector<Tract> &map, unsigned int* districts, int num_districts){
	vector<int> marked(map.size(), 0);
	vector< vector<int> > populations(num_districts);
	for(int i = 0; i < map.size(); i++){
		if(marked[i]!=-1){
			int pop = 0;
			add_cont_pop(map, districts, marked, i, pop);
			populations[districts[i]].push_back(pop);
		}
	}
	
	int wastedpop = 0; 
	for(int i = 0; i < populations.size(); i++){
		int largest = 0;
		for(int k = 0; k < populations[i].size(); k++){
			if(populations[i][k]>largest){
				largest=populations[i][k];
			}
			wastedpop += populations[i][k];
		}
		wastedpop-=largest;
	}
	return wastedpop;
}

double calc_pop_stdev(vector <Tract> &map, unsigned int* districts, int num_districts){
	vector<int> population(num_districts);
	double totalpop = 0;
	for(int i = 0; i < map.size(); i++){
		population[districts[i]]+=map[i].population;
		totalpop += map[i].population;
	}
	double sum = 0;
	for(int i = 0; i < num_districts; i++){
		sum += pow((population[i]-(totalpop/num_districts)), 2.0);
	}
	
	return pow((sum/(num_districts)),1.0/2);
}

double efficency_gap(vector <Tract> &map, unsigned int* &districts, int num_districts){
	vector <double> demVotes(num_districts);
	vector <double> gopVotes(num_districts);
	double wDem = 0;
	double wGop = 0;
	double votingPop =0;
	for(int i = 0; i < map.size(); i++){
		demVotes[districts[i]]+=map[i].Vdem;
		gopVotes[districts[i]]+=map[i].Vgop;
	}
	for(int i = 0; i < num_districts; i++){
		votingPop += demVotes[i]+gopVotes[i];
		if(demVotes[i]>gopVotes[i]){
			wDem += demVotes[i]-gopVotes[i];
			wGop += gopVotes[i];
		}
		else{
			wGop += gopVotes[i]-demVotes[i];
			wDem += demVotes[i];
		}
	}
	return abs((wDem-wGop)/votingPop);
}


vector<double> evaluateScore(vector <Tract> &map, unsigned int* &districts, int num_districts){
	vector<double> score; 

	score.push_back((double)non_cont_pop(map, districts, num_districts));
	cerr<<"contig done" << endl;
	score.push_back(calc_pop_stdev(map, districts, num_districts));
		cerr<<"pop done" << endl;

	score.push_back(efficency_gap(map, districts, num_districts));	
	cerr<<"efficency done" << endl;

	
	return score;
}

bool sort_1( const vector<double>& v1, const vector<double>& v2 ) {
	return v1[0] < v2[0];
}
bool sort_2( const vector<double>& v1, const vector<double>& v2 ) {
	return v1[1] < v2[1];
}
bool sort_3( const vector<double>& v1, const vector<double>& v2 ) {
	return v1[2] < v2[2];
}
bool sort_4( const vector<double>& v1, const vector<double>& v2 ) {
	return v1[3] < v2[3];
}
bool sort_5( const vector<double>& v1, const vector<double>& v2 ) {
	return v1[4] < v2[4];
}

vector<int> selection(vector<vector <double> > scores, double w1, double w2, double w3){
	vector<int> remv;

	for(int i = 0; i < scores.size(); i++){
		for(int k = 0; k < scores[i].size(); k++){
			cerr << scores[i][k] << " ";
		}
		cerr << endl;
	}
	
	sort(scores.begin(), scores.end(), sort_1);
	for(int i = 0; i < scores.size(); i++){
		scores[i][0]=(scores.size()-i)/(double)scores.size();
	}
	sort(scores.begin(), scores.end(), sort_2);
	for(int i = 0; i < scores.size(); i++){
		scores[i][1]=(scores.size()-i)/(double)scores.size();
	}

	sort(scores.begin(), scores.end(), sort_3);
	for(int i = 0; i < scores.size(); i++){
		scores[i][2]=(scores.size()-i)/(double)scores.size();
	}

	
	/*for(int i = 0; i < scores.size(); i++){
		for(int k = 0; k < scores[i].size(); k++){
			cerr << scores[i][k] << " ";
		}
		cerr << endl;
	}*/
	
	for(int i = 0; i < scores.size(); i++){
		double finScore = scores[i][0]*w1+scores[i][1]*w2;//+scores[i][2]*w3;
		scores[i].push_back(finScore);
	}

	/*sort(scores.begin(), scores.end(), sort_5);
	scores.erase(scores.begin()+scores.size()/2, scores.end());
	sort(scores.begin(), scores.end(), sort_4)*/;

	sort(scores.begin(), scores.end(), sort_5);
	if(scores.size()>1){
		scores.erase(scores.begin()+scores.size()/2, scores.end());

		sort(scores.begin(), scores.end(), sort_4);

		for(int i = scores.size()-1; i > -1; i--){
			remv.push_back(scores[i][3]);
		}
		return remv;
	}
	else{
		cerr << "MESSED UP SELECTION";
	}
}
unsigned int* mutate(vector<Tract> &map, unsigned int* districts, int num_districts){
	vector<vector<int> > edges = vector<vector<int> >(num_districts);
	unsigned int* mutatedDistricts = (unsigned int*) malloc(sizeof(unsigned int) * map.size());
	calculate_edges_op(map, districts, num_districts);

	for(int i = 0; i < edges.size(); i++){
		for(int k = 0; k < edges[i].size(); k++){
			if(rand()%2){ //MUTATE (TAKE DISTRICT OF NEIGHBOUR
				for(int j = 0; j < map[edges[i][k]].neighbours.size(); j++){
					if(districts[map[edges[i][k]].neighbours[j]]!=districts[edges[i][k]]){
						mutatedDistricts[edges[i][k]]=districts[map[edges[i][k]].neighbours[j]];
					}
				}
			}
			
			else{
				mutatedDistricts[edges[i][k]]=districts[edges[i][k]];
			}
		}
	}
	return mutatedDistricts;
}
void optimize(vector<Tract> &map, int initialMaps, int num_districts, int num_generations, int offspring){
	vector<unsigned int*> district_possibilities(initialMaps);
	vector<vector <double> > scores;
	
	for(int i = 0; i < initialMaps; i++){
		district_possibilities[i] = gen_random_map(map, num_districts, 0.01, pop_diff_m);
	}
	for(int i = 0; i < num_generations; i++){
		cerr << "index: " << i << "size: " << district_possibilities.size() << endl;

		for(int k = 0; k < district_possibilities.size(); k++){
			cerr << "start scoring" << endl;
			scores.push_back(evaluateScore(map, district_possibilities[k], num_districts));
			scores[k].push_back(k);
		}
		if(scores.size()>1){
		vector<int> select = selection(scores, 1, 1 , -1);
			for(int j = 0; j < select.size(); j++){
				district_possibilities.erase(district_possibilities.begin()+select[j]);
				//district_possibilities.erase(district_possibilities.begin()+select[j]);
			}
			int num_parents = district_possibilities.size();
			for(int j = 0; j < num_parents; j++){
				for(int l = 0; l < offspring; l++){
					cerr << j << " " << l << " " << num_parents << " " << district_possibilities.size() << endl;

					district_possibilities.push_back(mutate(map, district_possibilities[j], num_districts));
				}
				district_possibilities.erase(district_possibilities.begin()+j);
			}
			cerr << "index: " << i << endl;
		}
		else{
			break;
		}
		scores.clear();
	}
}