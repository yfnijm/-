#include "partition.hpp"
int alarm_flag;
void handler(int sig) { alarm_flag = true; }
void test_validity(int argc) {
    if (argc == 2)
        return;
    cout << "Usage: ./Lab2 <input file>" << endl;
    exit(1);
}

void read_file(char *fileName, vector<vector<int>> &nets,
               vector<vector<int>> &nodes, 
			   map<int, int> &node_map) {
    fstream input(fileName, ios::in);
    string oper;
    getline(input, oper);
    stringstream ss(oper);

    int net_num, node_num;
    ss >> net_num >> node_num;

    nets.resize(net_num);
    nodes.resize(node_num);

	int node_map_id = 0;
    for (int i = 0; i < net_num; i++) {
        getline(input, oper);
        ss.clear();
        ss.str(oper);
        int node_index;
        while (ss >> node_index) {
            //node_index--;
			//node_map[node_index] = node_index;

			if(!node_map.count(node_index))
				node_map[node_index] = node_map_id++;

			nets[i].push_back(node_map[node_index]);
            nodes[node_map[node_index]].push_back(i);
        }
    }
    input.close();
}

void produce_file(vector<int> &res) {
    fstream output("output.txt", ios::out | ios::trunc);
    for (bool side : res)
        output << side << "\n";
    output.close();
}
// only nets are not reference
void initializing(vector<vector<int>> nets, vector<int> & node_side){
	/*sort(nets.begin(), nets.end(), [](auto& v1, auto& v2){
		return v1.size() >  v2.size();
	});*/
	int count0 = 0, count1 = 0;
	for(auto& net : nets){
		if(count0 < node_side.size() / 2){
			for(int i : net){
				if(node_side[i] != -1) continue;
				node_side[i] = 0;
				count0++;
			}
		}else{
			for(int i : net){
				if(node_side[i] != -1) continue;
				node_side[i] = 1;
				count1++;
			}
		}
		//cout << count1 << " " << count0 << endl;
	}
	/*
	for(int i=0; i<node_side.size(); i++){
		if(node_side[i] == -1) node_side[i] = 1;
	/*	if(count0 < count1){
			node_side[i] = 0;
			count0++;
		}else{
			node_side[i] = 1;
			count1++;
		}
	}*/
	/*
	for(int i=0; i<node_side.size() / 2; i++)
		node_side[i] = 0;
	for(int i= node_side.size() / 2; i<node_side.size(); i++)
		node_side[i] = 1;
	*/
}


void updating(vector<vector<int>>& nets, vector<vector<int>>& nodes, 
		vector<int> & node_side, vector<int>& cut_line, 
		vector<map<int, set<int>>>& gain, vector<int>& node_gain){
	
	for(int i = 0; i < node_side.size(); i++){
		if(node_side[i] == 0){
			for(int& net : nodes[i])
				cut_line[net]++;
		}
	}
	//cout << "1"
	//	<< " " << clock() << endl;
	vector<int>& gain_tmp = node_gain; //(node_side.size());
	for(int i = 0; i < cut_line.size(); i++){
		if(nets[i].size() <= 1) continue;
		//cout << cut_line[i] << " ";
		if(cut_line[i] == 0){
			for(int j=0; j<nets[i].size(); j++)
				gain_tmp[nets[i][j]]--;
		}else if(cut_line[i] == nets[i].size()){
			for(int j=0; j<nets[i].size(); j++)
				gain_tmp[nets[i][j]]--;
		}else{
			if(cut_line[i] == 1)
				for(int j=0; j<nets[i].size(); j++){
					//int count = 0;
					if(node_side[nets[i][j]] == 0){
						gain_tmp[nets[i][j]]++;
						//count++;
						//is(count > 1) exit(1);
					}
				}
			if(cut_line[i] == nets[i].size() - 1 && nets[i].size() - 1 != 1)
				for(int j=0; j<nets[i].size(); j++){
					if(node_side[nets[i][j]] == 1)
						gain_tmp[nets[i][j]]++;
				}
		}
	}
	//cout << "2"
	//	<< " " << clock() << endl;
	for(int i=0; i<gain_tmp.size(); i++){
		gain[node_side[i]][gain_tmp[i]].insert(i);
		//cout << gain_tmp[i] << " ";
	}
	//cout << "3"
	//	<< " " << clock() << endl;
}

int parting(vector<vector<int>>& nets, vector<vector<int>>& nodes, 
		vector<int> & node_side, vector<int>& cut_line, 
		vector<map<int, set<int>>>& gain, vector<int>& node_gain,
		vector<int>& res){
		
		
	int count1 = accumulate(node_side.begin(), node_side.end(), 0);
	int gain_acc = 0;
	int best = 0;

	while((gain[0].size() || gain[1].size()) && !alarm_flag){
		int side;

		if(!gain[0].size()) side = 1;
		else if(!gain[1].size()) side = 0;
		else{
			if(gain[0].rbegin()->first > gain[1].rbegin()->first
					&& (double) (count1 + 1) / node_side.size() < 0.55)
				side = 0;
			else 
				side = 1;
		}
		
		//int flag = gain[side].rbegin()->first < 0;	
		gain_acc += gain[side].rbegin()->first;
	
		//auto tar_it = std::next(begin(gain[side].rbegin()->second), rand_between(0, gain[side].rbegin()->second.size()));
		auto tar_it = gain[side].rbegin()->second.begin();
		std::advance(tar_it, rand() % gain[side].rbegin()->second.size());	
		//auto tar_it = gain[side].rbegin()->second.begin();
		int tar = *tar_it;

		gain[side].rbegin()->second.erase(tar_it);
		node_side[tar] = !side;


		double ratio = (double) (count1 + 1) / node_side.size();
		if(gain_acc >= best && 0.45 < ratio && ratio <0.55){
			res = node_side;
			best = gain_acc;
		}

		for(auto& net : nodes[tar]){
			//cout << net << " ";
			if(nets[net].size() <= 1) continue;
			if(side == 0){
				if(cut_line[net] == 0 + 1){
					if(nets[net].size() == 2){
						for(int& node : nets[net]){
							if(!gain[node_side[node]][node_gain[node]]
									.count(node)) continue;	
							gain[node_side[node]][node_gain[node]].erase(node);
							node_gain[node] -= 2;	
							gain[node_side[node]][node_gain[node]].insert(node);
						}
					}else{
						for(int& node : nets[net]){
							if(!gain[node_side[node]][node_gain[node]]
									.count(node)) continue;	
							gain[node_side[node]][node_gain[node]].erase(node);
							node_gain[node]--;	
							gain[node_side[node]][node_gain[node]].insert(node);
						}

					}
				}else if(cut_line[net] == nets[net].size() - 1 + 1){
					if(nets[net].size() == 2){
						for(int& node : nets[net]){
							if(!gain[node_side[node]][node_gain[node]]
									.count(node)) continue;	
							gain[node_side[node]][node_gain[node]].erase(node);
							node_gain[node] += 2;	
							gain[node_side[node]][node_gain[node]].insert(node);
						}
					}else{
						for(int& node : nets[net]){
							if(!gain[node_side[node]][node_gain[node]]
									.count(node)) continue;	
							gain[node_side[node]][node_gain[node]].erase(node);
							node_gain[node]++;	
							gain[node_side[node]][node_gain[node]].insert(node);
						}
					}
				}else if(cut_line[net] == 1 + 1){	
					if(nets[net].size() == 3){
						for(int& node : nets[net]){
							if(!gain[node_side[node]][node_gain[node]]
									.count(node)) continue;	
							gain[node_side[node]][node_gain[node]].erase(node);
							if(node_side[node] == 1) node_gain[node]--;	
							else node_gain[node]++;	
							gain[node_side[node]][node_gain[node]].insert(node);
						}
					}else{
						for(int& node : nets[net]){
							if(!gain[node_side[node]][node_gain[node]]
									.count(node)) continue;	
							gain[node_side[node]][node_gain[node]].erase(node);
							if(node_side[node] == 0) node_gain[node]++;	
							gain[node_side[node]][node_gain[node]].insert(node);
						}

					}
				}else if(cut_line[net] == nets[net].size() - 2 + 1){
					if(nets[net].size() == 3){
						for(int& node : nets[net]){
							if(!gain[node_side[node]][node_gain[node]]
									.count(node)) continue;	
							gain[node_side[node]][node_gain[node]].erase(node);
							if(node_side[node] == 1) node_gain[node]--;	
							else node_gain[node]++;	
							gain[node_side[node]][node_gain[node]].insert(node);
						}
					}else{
						for(int& node : nets[net]){
							if(!gain[node_side[node]][node_gain[node]]
									.count(node)) continue;	
							gain[node_side[node]][node_gain[node]].erase(node);
							if(node_side[node] == 1) node_gain[node]--;	
							gain[node_side[node]][node_gain[node]].insert(node);
						}
					}
				}else if(cut_line[net] == 0)
					cout << "WORING: cut line behind the bound!\n" << cut_line[net];
				cut_line[net]--;
				count1++;
			}else{
				if(cut_line[net] == 0){
					if(nets[net].size() == 2){
						for(int& node : nets[net]){
							if(!gain[node_side[node]][node_gain[node]]
									.count(node)) continue;	
							gain[node_side[node]][node_gain[node]].erase(node);
							node_gain[node] += 2;	
							gain[node_side[node]][node_gain[node]].insert(node);
						}
					}else{
						for(int& node : nets[net]){
							if(!gain[node_side[node]][node_gain[node]]
									.count(node)) continue;	
							gain[node_side[node]][node_gain[node]].erase(node);
							node_gain[node]++;	
							gain[node_side[node]][node_gain[node]].insert(node);
						}
					}
				}else if(cut_line[net] == nets[net].size() - 1){
					if(nets[net].size() == 2){
						for(int& node : nets[net]){
							if(!gain[node_side[node]][node_gain[node]]
									.count(node)) continue;	
							gain[node_side[node]][node_gain[node]].erase(node);
							node_gain[node] -= 2;	
							gain[node_side[node]][node_gain[node]].insert(node);
						}
					}else{
						for(int& node : nets[net]){
							if(!gain[node_side[node]][node_gain[node]]
									.count(node)) continue;	
							gain[node_side[node]][node_gain[node]].erase(node);
							node_gain[node]--;	
							gain[node_side[node]][node_gain[node]].insert(node);
						}
					}
				}else if(cut_line[net] == 1){
					if(nets[net].size() == 3){
						for(int& node : nets[net]){
							if(!gain[node_side[node]][node_gain[node]]
									.count(node)) continue;	
							gain[node_side[node]][node_gain[node]].erase(node);
							if(node_side[node] == 0) node_gain[node]--;	
							else node_gain[node]++;	
							gain[node_side[node]][node_gain[node]].insert(node);
						}
					}else{
						for(int& node : nets[net]){
							if(!gain[node_side[node]][node_gain[node]]
									.count(node)) continue;	
							gain[node_side[node]][node_gain[node]].erase(node);
							if(node_side[node] == 0) node_gain[node]--;	
							gain[node_side[node]][node_gain[node]].insert(node);
						}

					}
				}else if(cut_line[net] == nets[net].size() - 2){
					if(nets[net].size() == 3){
						for(int& node : nets[net]){
							if(!gain[node_side[node]][node_gain[node]]
									.count(node)) continue;	
							gain[node_side[node]][node_gain[node]].erase(node);
							if(node_side[node] == 0) node_gain[node]--;	
							else node_gain[node]++;	
							gain[node_side[node]][node_gain[node]].insert(node);
						}
					}else{
						for(int& node : nets[net]){
							if(!gain[node_side[node]][node_gain[node]]
									.count(node)) continue;	
							gain[node_side[node]][node_gain[node]].erase(node);
							if(node_side[node] == 1) node_gain[node]++;	
							gain[node_side[node]][node_gain[node]].insert(node);
						}

					}
				}else if(cut_line[net] == nets[net].size())
					cout << "WORING: cut line beyond the bound!\n"  << net << " " << cut_line[net] << endl;
				cut_line[net]++;
				count1--;
			}
		}
/*
		cout << endl;
		for(int i=0; i < cut_line.size(); i++){
			cout << cut_line[i] << " ";
		}
		cout << endl;
*/
		// here to remove the empty cap from top
		while(gain[0].size() && gain[0].rbegin()->second.size() == 0){
			gain[0].erase(prev(gain[0].end()));
		}
		while(gain[1].size() && gain[1].rbegin()->second.size() == 0){
			gain[1].erase(prev(gain[1].end()));
		}
	}
	//if(gain_acc > 0) res = node_side;
	//cout << best << endl;
	return best;
}
