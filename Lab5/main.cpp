#include "bits/stdc++.h"
using namespace std;


void read(string, vector<int>&);
void greedy(vector<int>&, vector<vector<tuple<int, int, int>>>&, vector<vector<tuple<int, int, int>>>&);
void print(string, vector<vector<tuple<int, int, int>>>&, vector<vector<tuple<int, int, int>>>&);
int main(int argc, char** argv){
	vector<int> row;
	read(argv[1], row);

	vector<vector<tuple<int, int, int>>> vertical, horizonal;
	greedy(row, vertical, horizonal);

	print(argv[2], vertical, horizonal);
	return 0;
}

void read(string fileName, vector<int>& row){
	fstream fin(fileName, ios::in);

	int tmp;
	while(fin >> tmp){
		row.push_back(tmp);
	}
	//cout << row.size();
}

void greedy(vector<int>& row, vector<vector<tuple<int, int, int>>>& vertical, vector<vector<tuple<int, int, int>>>& horizonal){
	vector<map<int, int>> track;
	track.push_back(map<int, int>());
	int len = row.size() / 2;
	int track_upper = 0, track_lower = -1;

	int max_index = *max_element(row.begin(), row.end());
	//cout << max_index;
	vector<int> last_element(max_index + 1);

	
	vector<int> element_up(max_index + 1);
	vector<int> element_low(max_index + 1);
	for(int i=0; i<row.size(); i++){
		if(i >= len && !element_low[row[i]]) element_low[row[i]] = i - len;
		if(i < len && !element_up[row[i]]) element_up[row[i]] = i;

		//last_element[row[i] % len] = max(last_element[row[i] % len], i % len);
		last_element[row[i]] = max(element_up[row[i]], element_low[row[i]]);
	}

	//cout << element_up[2] << " " << element_low[2] << endl;
	/*for(int i=0; i<=max_index; i++){
		if(!element_up[i] || !element_low[i]){
			element_up[i] = 0;
			element_low[i] = 0;
		}
	}*/
	//cout << element_up[2] << " " << element_low[2] << endl;

	//vector<vector<tuple<int, int, int>>>
	vertical.resize(max_index + 1);
	horizonal.resize(max_index + 1);
	map<int, int> occupied;
	auto insert_vertical = [&](int id, int col, int start, int end){
		vertical[id].push_back({col, start, end});
		occupied[start]++;
		occupied[end]--;
	};
	
	map<int, int> cur_track;
	//unordered_map<int, set<int>> id_to_track; 
	for(int i=0; i<len || (cur_track.size() && i < 500); i++){
		occupied.clear();
		int fin = 0;
		if(i < len){
			int upper_id = row[i];
			int lower_id = row[i + len];
			if(element_up[upper_id] != i) upper_id = 0;
			if(element_low[lower_id] != i) lower_id = 0;
			

			auto produced_A = [&](){
				if(upper_id == lower_id){
					if(upper_id != 0) insert_vertical(upper_id, i, INT_MAX, INT_MIN);
					upper_id = 0, lower_id = 0;
					return 1;
				}else{
					if(upper_id){
						/*if((cur_track.size() && prev(cur_track.end())->first + 1 != track_upper)){
							insert_vertical(upper_id, i, INT_MAX, prev(cur_track.end())->first + 1);
							cur_track[prev(cur_track.end())->first + 1] = upper_id;
							upper_id = 0;
						/*}else if(!cur_track.size() && track_upper - 1 != track_lower){
						}else if(!cur_track.size() && track_upper - 1 != track_lower){
							/*insert_vertical(upper_id, i, INT_MAX, track_upper - 1);
							cur_track[track_upper - 1] = upper_id;
							upper_id = 0;
							int tar = track_upper - 1;
							while(!cur_track.count(tar)){ tar--;}
							insert_vertical(upper_id, i, INT_MAX, tar);
							cur_track[tar] = upper_id;
							upper_id = 0;*/
						//}else{
							//cout << cur_track[track_upper];
							cur_track[track_upper] = upper_id;
							track_upper++;
							insert_vertical(upper_id, i, INT_MAX, track_upper - 1);
						//}
					}
					if(lower_id){
						/*if((cur_track.size() && cur_track.begin()->first - 1 != track_lower)){
							cur_track[cur_track.begin()->first - 1] = lower_id;
							insert_vertical(lower_id, i, cur_track.begin()->first - 1, INT_MIN);
							lower_id = 0;
						/*}else if(!cur_track.size() && track_upper != track_lower + 1){

							int tar = track_lower + 1;
							while(!cur_track.count(tar)){ tar++;}
							cur_track[tar] = lower_id;
							insert_vertical(lower_id, i, tar, INT_MIN);
							lower_id = 0;*/
						//}else{
							//cout << cur_track[track_lower];
							cur_track[track_lower] = lower_id;
							track_lower--;
							insert_vertical(lower_id, i, track_lower + 1, INT_MIN);
						//}
					}
				}
				return 0;
			};
			fin = produced_A();	
		}
		//produces_B schedule		
		if(!fin){
			vector<tuple<int, int, int>> order;
			unordered_map<int, int> highest;
			for(auto& t : cur_track){
				if(highest.count(t.second)){
					order.push_back({highest[t.second], t.first, t.second});
					highest[t.second] = t.first;
				}else{
					highest[t.second] = t.first;
				}
			}


			int cur_col = i;
			sort(order.begin(), order.end());
			vector<int> selected;
			if(order.size()){
				selected.push_back(0);
				for(int i=1; i<order.size(); i++){
					auto [r1, r2, id] = order[i];
					if(get<1>(order[selected.back()]) <= r1){
						selected.push_back(i);
					}else if(get<1>(order[selected.back()]) > r2){
						selected.pop_back();
						selected.push_back(i);
					}
				}

				for(int i=0; i<selected.size(); i++){	
					auto [r1, r2, id] = order[selected[i]];
					insert_vertical(id, cur_col, r2, r1);
				}
			}
			
			track.push_back(cur_track);	
			//prepare next col's track
			for(int i=0; i<selected.size(); i++){
				cur_track.erase(get<0>(order[selected[i]]));
			}
		}
		//store cur track
		else track.push_back(cur_track);	


		//check reached
		unordered_map<int, unordered_set<int>> reached;
		for(auto [col, id] : cur_track){
			reached[id].insert(col);
		}
		for(auto [id, s] : reached){
			if(s.size() != 1 || last_element[id] >= i) continue;
			cur_track.erase(*s.begin());
		}
		
	}



	track_upper += 200;
	track_lower -= 200;
	
	for(int i=0; i<len; i++){
		int upper_id = row[i];
		int lower_id = row[i + len];
		if(upper_id && element_up[upper_id] != i){
			insert_vertical(upper_id, i, INT_MAX, track_upper - upper_id);		
			//cout << upper_id << endl;
		}
		if(lower_id && element_low[lower_id] != i){
			insert_vertical(lower_id, i, track_lower + lower_id, INT_MIN);			
			//cout << lower_id << endl;
		}

	}

	for(int i=0; i<=max_index; i++){
		for(auto& t : vertical[i]){
			if(get<1>(t) == INT_MAX) { get<1>(t) = track_upper; /*get<2>(t) = get<2>(t) == INT_MAX ? get<2>(t)-1 : track_upper;*/}
			if(get<2>(t) == INT_MIN) { get<2>(t) = track_lower;	/*get<1>(t) = get<1>(t) == INT_MIN ? get<1>(t)+1 : track_lower;*/ }
		}
		
		if(vertical[i].size() ||  horizonal[i].size()){
			if(element_up[i])
				horizonal[i].push_back({track_upper - i, 1, len});
			if(element_low[i])
				horizonal[i].push_back({track_lower + i, 1, len});
		}
	}
	//cout << "***" << len << endl;

	//vector<tuple<int, int, int>> horizonal(max_index);
	track.push_back(map<int, int>());
	for(int i=0; i<track.size() - 1; i++){
		int start = i;
		while(track[i].size()){
			auto [row, id] = *track[i].begin();
			track[i].erase(track[i].begin());

			int len = 1;
			while(track[i + len].count(row) && track[i + len][row] == id){
				track[i + len].erase(row);
				len++;
			}	
			horizonal[id].push_back({row, start, start + len - 1});
		}
	}

	//cout << track_upper << " " << track_lower << endl;
}

void print(string fileName,  
		vector<vector<tuple<int, int, int>>>& vertical,
		vector<vector<tuple<int, int, int>>>& horizonal){
	// preprocess
	fstream fout(fileName, ios::out);
	for(int i=1; i<vertical.size(); i++){
		if(vertical[i].size() == 0 && horizonal[i].size() == 0) continue;
		fout << ".begin " << i << endl;
		for(auto [col, start, end] : vertical[i])
			if(start != end) 
				fout << ".V " << col << " " << end  << " " << start << endl; 

		for(auto [row, start, end] : horizonal[i])
			if(start != end) 
				fout << ".H "  << " " << start - 1 << " " << row << " " << end - 1 << endl;
		fout << ".end" << endl;
	}
	fout.close();
}
