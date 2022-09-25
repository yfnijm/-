#include "bits/stdc++.h"
#include <iterator>
#include <signal.h>
#include <unistd.h>

using namespace std;

void test_validity(int);
void read_file(char *, vector<vector<int>> &, vector<vector<int>> &, 
		map<int, int>&);
void produce_file(vector<int> &);
void initializing(vector<vector<int>>, vector<int>&);


void updating(vector<vector<int>> &, vector<vector<int>>&,
              vector<int>&, vector<int>&, 
			  vector<map<int, set<int>>>&, vector<int>&);

int parting(vector<vector<int>> &, vector<vector<int>>&,
              vector<int>&, vector<int>&, 
			  vector<map<int, set<int>>>&, vector<int>&,
			  vector<int>&);

void handler(int);
