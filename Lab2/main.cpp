#include "bits/stdc++.h"
#include "partition.hpp"
using namespace std;
extern int alarm_flag;
int main(int argc, char **argv) {
	//read file
    test_validity(argc);
    vector<vector<int>> nets, nodes;
	map<int, int> node_map;
    read_file(argv[1], nets, nodes, node_map);

	//set timer
    alarm_flag = false;
    signal(SIGALRM, handler);
    alarm(25); // can be 25~30

    //set config
    int net_num = nets.size(), node_num = nodes.size();
    vector<map<int, set<int>>> gain(2);
	vector<int> node_gain(node_num);
    vector<int> node_side(node_num, -1); 
	vector<int> cut_line(net_num);
    initializing(nets, node_side);
    vector<int> res = node_side;
	
	
	//retry FM
	
	int count = 0;
    while (!alarm_flag && count != 3) {
		updating(nets, nodes, node_side, cut_line, gain, node_gain);
		if(!parting(nets, nodes, node_side, cut_line, gain, node_gain, res)) count++;
		else count = 0;
    	cut_line = vector<int> (net_num);
    	gain = vector<map<int, set<int>>> (2);
		node_gain = vector<int>(node_num);
		node_side = res;
	}

	//output and return
	vector<int> real_res;
	real_res.reserve(node_num);
	for(auto& p : node_map)
		real_res.push_back(res[p.second]);
    produce_file(real_res);
    return 0;
}
