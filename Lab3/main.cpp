#include"bits/stdc++.h"
#include"unistd.h"
using namespace std;

double factor;
double global_cost;
int outline_width, outline_height;
int block_size, terminal_size;
unordered_map<string, int> name_to_id;
vector<pair<int, int>> blocks_and_terminals;
vector<vector<int>> nets;
int fail_counter;

//bstar tree
double cost = 10e15;
int root;
vector<int> left_child, right_child, parents;

//pre bstar tree
int pre_root;
vector<int> pre_left_child, pre_right_child, pre_parents;
vector<pair<int, int>> pre_blocks_and_terminals;
long pre_area = 10e15, pre_cost = 10e15, pre_wire_length = 10e15,
	 pre_chips_width = 10e15, pre_chips_height = 10e15;


//best tree
//double best_cost = 10e15;
int best_root;
vector<int> best_left_child, best_right_child, best_parents;
vector<pair<int, int>> best_blocks;
//for result
long best_cost = 10e15, best_area, best_wire_length,
	 best_chips_width, best_chips_height;
vector<vector<int>> best_blocks_coor;


//flags
bool find_flag = false;
bool alarm_flag = false;

void readBlocks(char* fname);
void readNets(char* fname);
void BstarTreeInit();
void SA();

void node_erase(int node);
void node_insert(int node, int pos);
void node_swap(int node1, int node2);

void update();
void outputRes(char* fname);

void alarm_func(int sig);

int main(int argc, char** argv){
	if(argc != 5){
		cerr << "./Lab3 [α value] [input.block name] [input.net name] [output name]" << endl;
		exit(1);
	}
	signal(SIGALRM, alarm_func);	
	alarm(250);

	factor = atof(argv[1]);
	readBlocks(argv[2]);
	readNets(argv[3]);
	
	//using full binary to initial
	while(!alarm_flag){
		BstarTreeInit();
		SA();
	}

	outputRes(argv[4]);
	return 0;
}

void readBlocks(char* fname){
	ifstream fin(fname, ios::in);
	string useless;
	fin >> useless >> outline_width >> outline_height;

	fin >> useless >> block_size;
	fin >> useless >> terminal_size;

	blocks_and_terminals.resize(block_size + terminal_size + 1);
	for(int i=1; i<=block_size; i++){
		string name;
		fin >> name;
		name_to_id[name] = i;
		fin >> blocks_and_terminals[i].first >> blocks_and_terminals[i].second;
	}

	for(int i = block_size + 1; i<= block_size + terminal_size; i++){
		string name;
		fin >> name >> useless;
		name_to_id[name] = i;
		fin >> blocks_and_terminals[i].first >> blocks_and_terminals[i].second;
	}
	fin.close();
}


void readNets(char* fname){
	ifstream fin(fname, ios::in);
	string useless;
	int net_size;
	fin >> useless >> net_size;
	nets.resize(net_size);

	for(int i=0; i<net_size; i++){
		int net_degree;
		fin >> useless >> net_degree;
		for(int j=0; j<net_degree; j++){
			string name;
			fin >> name;
			
			if(name_to_id.count(name))
				nets[i].push_back(name_to_id[name]);
			else {
				cerr << "NONSEEN BLOCK/TERMINAL!\n";
				exit(1);
			}
		}
	}
}

void BstarTreeInit(){
	vector<int> nodes(block_size + 1);	
	iota(nodes.begin() + 1, nodes.end(), 1);
	random_shuffle(nodes.begin() + 1, nodes.end());

	left_child.clear();
	left_child.resize(block_size + 1);

	parents.clear();
	parents.resize(block_size + 1);

	right_child.clear();
	right_child.resize(block_size + 1);

	root = nodes[1];
	for(int i=1; i <= block_size / 2; i++){
		if(!nodes[i]){
			cerr << "WRONG INDEX!\n";
			exit(1);
		}

		if(i * 2 <= block_size){
			left_child[nodes[i]] = nodes[i * 2];
			parents[nodes[i * 2]] = nodes[i];
		}
		if(i * 2 + 1 <= block_size){
			right_child[nodes[i]] = nodes[i * 2 + 1];
			parents[nodes[i * 2 + 1]] = nodes[i];	
		}
	}
	update();
}

void SA(){
	int fail_counter = 0;
	int t = 100000;
	while(t-- && fail_counter < block_size && !alarm_flag){
		//Op1: Rotate a module.
		//[Op2: Flip a module.] 
		//Op3: Move a module to another place.
		// Op4: Swap two modules.
		//cout << pre_chips_width << " " << pre_chips_height << " ";
		
		int oper = rand() % 3;
		int tar = (rand() % block_size) + 1;
		if(oper < 1){
			swap(blocks_and_terminals[tar].first, blocks_and_terminals[tar].second);
		}else if(oper < 2){
			//could be 0 to insert to root :)
			//int tar2 = rand() % (block_size + 1);
			int tar2 = (rand() % block_size) + 1;
			if(tar == tar2) continue;
			node_erase(tar);
			node_insert(tar, tar2);
		}else if(oper < 3){
			int tar2 = (rand() % block_size) + 1;
			if(tar == tar2) continue;
			node_swap(tar, tar2);
		}
		update();
	}
}

void update(){
	vector<int> x0(block_size + 1), y0(block_size + 1), 
		x1(block_size + 1), y1(block_size + 1),
		forward(block_size + 1), backward(block_size + 1);
	//
	function<void(int, int)> update_contour = [&](int cur, int pre){
		if (pre == 0) {
			x0[cur] = 0;
			x1[cur] = blocks_and_terminals[cur].first;
			y0[cur] = 0;
			y1[cur] = blocks_and_terminals[cur].second;
			forward[cur] = backward[cur] = 0;
			return;
		}
		if (left_child[pre] == cur) {
			x0[cur] = x1[pre];
			x1[cur] = x0[cur] + blocks_and_terminals[cur].first;
			if (forward[pre] == 0) {
				y0[cur] = 0;
				y1[cur] = y0[cur] + blocks_and_terminals[cur].second;
				forward[pre] = cur;
				backward[cur] = pre;
				forward[cur] = 0;
				return;
			}else {
				backward[cur] = pre;
				forward[cur] = forward[pre];
				backward[forward[cur]] = cur;
				forward[pre] = cur;
			}
		}else {
			x0[cur] = x0[pre];
			x1[cur] = x0[cur] + blocks_and_terminals[cur].first;
			if (backward[pre] == 0) {
				backward[cur] = 0;
				forward[cur] = pre;
				backward[pre] = cur;
			}else {
				forward[backward[pre]] = cur;
				backward[cur] = backward[pre];
				forward[cur] = pre;
				backward[pre] = cur;
			}
		}
		
		int i = forward[cur];
		while(i){
			y0[cur] = max(y0[cur], y1[i]);
			if(x1[i] >= x1[cur]) {
				if(x1[i] == x1[cur]) {
					forward[cur] = forward[i];
					if(forward[i] == 0)
						backward[forward[i]] = cur;
				}else {
					forward[cur] = i;
					backward[i] = cur;
				}
				break;
			}else i = forward[i];
		}
		
		if(i == 0)
			forward[cur] = 0;
		y1[cur] = y0[cur] + blocks_and_terminals[cur].second;

	};
	int count = 0;
	function<void(int, int)> dfs = [&](int cur, int pre){
		if(cur == 0) return;
		count++;
		update_contour(cur, pre);
		dfs(left_child[cur], cur);
		dfs(right_child[cur], cur);
	};
	dfs(root, 0);
	//cout << count << " ";
	//return	;
	long wire_length = 0;
	for(auto& net : nets){
		int tile_x0 = INT_MAX, tile_x1 = INT_MIN, tile_y0 = INT_MAX, tile_y1 = INT_MIN;
		for(int i=0; i<net.size(); i++){
			int& j = net[i];
			int x = (j <= block_size) ? x0[j] + (x1[j] - x0[j]) / 2 
				: blocks_and_terminals[j].first;
            int y = (j <= block_size) ? y0[j] + (y1[j] - y0[j]) / 2 
				: blocks_and_terminals[j].second;
            tile_x0 = min(tile_x0, x);
            tile_x1 = max(tile_x1, x);
            tile_y0 = min(tile_y0, y);
            tile_y1 = max(tile_y1, y);
		}
		wire_length += tile_x1 - tile_x0 + tile_y1 - tile_y0;
	}
	
	int bound_x = 0, bound_y = 0;
	for(int i=1; i<=block_size; i++){
		bound_x = max(bound_x, x1[i]);
		bound_y = max(bound_y, y1[i]);
	}
	//cout << bound_x << " " << bound_y << " ";
	long area = 1L * bound_x * bound_y;
	//cout << area << " " << wire_length << endl;
	double cost = factor * area + (1 - factor) * wire_length;
	if(cost != best_cost && (bound_x <= outline_width && bound_y <= outline_height)){
		find_flag = true;
		best_cost = cost;
		best_area = area;
		best_chips_width = bound_x;
		best_chips_height = bound_y;
		best_wire_length = wire_length;

		best_blocks_coor.resize(block_size + 1);
		for(int i=1; i<=block_size; i++){
			best_blocks_coor[i] = vector<int>({x0[i], y0[i], x1[i], y1[i]});
		}
	}

	if((((bound_x > outline_width * 1 && bound_x > pre_chips_width && bound_x > outline_width) ||
		(bound_y > outline_height * 1 && bound_y > pre_chips_height && bound_y > outline_height)) 
		&& bound_x + bound_y >= (pre_chips_width + pre_chips_height))
		//|| (bound_x == pre_chips_width && bound_y == pre_chips_height)
	 	){		
		root = pre_root;
		parents = pre_parents;
		left_child = pre_left_child;
		right_child = pre_right_child;
		blocks_and_terminals = pre_blocks_and_terminals;

		fail_counter++;
	}else{
		pre_root = root;
		pre_parents = parents;
		pre_left_child = left_child;
		pre_right_child = right_child;
		pre_blocks_and_terminals = blocks_and_terminals;

		pre_wire_length = wire_length;
		pre_chips_width = bound_x;
		pre_chips_height = bound_y;
		pre_area = area;
		pre_cost = cost;
		fail_counter = 0;
	}
		
}

void node_erase(int node){
	if(block_size == 1) return;
	if(!left_child[node] && !right_child[node]){
		if(left_child[parents[node]] == node)
			left_child[parents[node]] = 0;
		else if(right_child[parents[node]] == node)
			right_child[parents[node]] = 0;
		else{
			cerr << "ERROR3!\n";
			exit(1);
		}
	}/*else if(left_child[node] || right_child[node]){
	}*/else{
		while(left_child[node] || right_child[node]){
			if(left_child[node] && right_child[node]){
				if(rand() % 2)
					node_swap(node, left_child[node]);
				else
					node_swap(node, right_child[node]);
			}else if(left_child[node])
				node_swap(node, left_child[node]);
			else 
				node_swap(node, right_child[node]);
		}
		
		node_erase(node);
	}
}

void node_insert(int node, int pos){
	parents[node] = pos;
	if(rand() % 2){
		left_child[node] = left_child[pos];
		left_child[pos] = node;
		right_child[node] = 0;
		if(left_child[node])
			parents[left_child[node]] = node;
	}else{
		right_child[node] = right_child[pos];
		right_child[pos] = node;
		left_child[node] = 0;
		if(right_child[node])
			parents[right_child[node]] = node;
	}
}

void node_swap(int node1, int node2){
	//cheack root
	if(root == node1)
        root = node2;
    else if (root == node2) 
        root = node1;
	
	//swap parent
	int parent1 = parents[node1];
	int parent2 = parents[node2];
	//cout << parent1 << " " << node1 << " " <<parent2 << " " << node2;
	if(parent1){
		if (left_child[parent1] == node1)
           	left_child[parent1] = node2;
        else if (right_child[parent1] == node1)
           	right_child[parent1] = node2;
        else {
            cerr << "Tree Structure ERROR1!\n";
            exit(1);
        }
	}
	if(parent2){
		if (left_child[parent2] == node2)
           	left_child[parent2] = node1;
        else if (right_child[parent2] == node2)
           	right_child[parent2] = node1;
        else {
            cerr << "Tree Structure ERROR2!\n";
            exit(1);
        }
	}
	parents[node1] = parent2;
	parents[node2] = parent1;

	//swap children
	//left
	int left1 = left_child[node1], left2 = left_child[node2];
	left_child[node1] = left2;
	left_child[node2] = left1;
	if(left1)
		parents[left1] = node2;
	if(left2)
		parents[left2] = node1;
	
	int right1 = right_child[node1], right2 = right_child[node2];
	right_child[node1] = right2;
	right_child[node2] = right1;
	if(right1)
		parents[right1] = node2;
	if(right2)
		parents[right2] = node1;
	

	//correct
	if(parent1 == parents[node1]){
		parents[node1] = parent2;
	}else if(left_child[node1] == node1)
		left_child[node1] = node2;
	else if(right_child[node1] == node1)
		right_child[node1] = node2;

	if(parent2 == parents[node2]){
		parents[node2] = parent1;
	}else if(left_child[node2] == node2)
		left_child[node2] = node1;
	else if(right_child[node2] == node2)
		right_child[node2] = node1;
}

void outputRes(char* fname){
	fstream fout(fname, ios::out);
	fout << best_cost << endl;
	fout << best_wire_length << endl;
	fout << best_area << endl;
	fout << best_chips_width << " " << best_chips_height << endl;
	long clk = clock();
	fout << (double) clk / 1'000'000 << endl;
	for(auto[name, id] :  name_to_id){
		if(id > block_size) continue;
		fout << name 
			<< " " << best_blocks_coor[id][0] 
			<< " " << best_blocks_coor[id][1] 
			<< " " << best_blocks_coor[id][2] 
			<< " " << best_blocks_coor[id][3]
			<< endl;
	}
	fout.close();
}

void  alarm_func(int sig){
	alarm_flag = true;
}

