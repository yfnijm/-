#include<bits/stdc++.h>
using namespace std;

int total_block_num = 0;

class Block;
unordered_set<Block*> m;
class Block{
public:
    Block *rt, *tr, *bl, *lb;
    int x, y, w, h;
    bool marked = false;
    Block(int x_in, int y_in, int w_in, int h_in)
        : x(x_in), y(y_in), w(w_in), h(h_in){
		total_block_num++;
		m.insert(this);
    };
	~Block(){
		total_block_num--;
		m.erase(this);
	}
};


int main(int argc, char** argv){
    fstream fin(argv[1], ios::in);
    
    int width, height;
    fin >> width >> height;


    string type;
    vector<string> res;
    vector<Block*> blocks(3000, nullptr);

    Block* main_block = new Block(0, 0, width, height);
	Block* psuedo_lower_block = new Block(-1, -1, width + 2, 1);
	Block* psuedo_upper_block = new Block(-1, height, width + 2, 1);
	Block* psuedo_left_block = new Block(-1, 0, 1, height);
	Block* psuedo_right_block = new Block(width, 0, 1, height);
	
	main_block->lb = psuedo_lower_block;
	main_block->bl = psuedo_left_block;
	main_block->rt = psuedo_upper_block;
	main_block->tr = psuedo_right_block;
	
	psuedo_lower_block->rt = psuedo_right_block;
	
	psuedo_left_block->lb = psuedo_lower_block;
	psuedo_left_block->rt = psuedo_upper_block;
	psuedo_left_block->tr = main_block;

	psuedo_upper_block->lb = psuedo_left_block;
	
	psuedo_right_block->lb = psuedo_lower_block;
	psuedo_right_block->rt = psuedo_upper_block;
	psuedo_right_block->bl = main_block;

	while(fin >> type){
        if(type[0] == 'P'){
			int tar_x, tar_y;
			fin >> tar_x >> tar_y;


			auto find_block = [&](int tar_x, int tar_y) -> Block*{
				Block* cur_block = psuedo_right_block->bl;
				while(1){
					int flag = false;
					//we can reserve the vertex on upper and left bound.
					while(cur_block->y + cur_block->h < tar_y){
						cur_block = cur_block->rt;
						flag = true;
					}

					if(cur_block->x > tar_x){
						cur_block = cur_block->bl;
						flag = true;
					}
					if(!flag) break;
				}
				return cur_block;
			};

			auto finded = find_block(tar_x, tar_y);
			res.push_back(to_string(finded->x) + " " + to_string(finded->y));
        }else{
            int id = stoi(type);
			int tar_x, tar_y, tar_w, tar_h;
			fin >> tar_x >> tar_y >> tar_w >> tar_h;
    		Block* tar_block = new Block(tar_x, tar_y, tar_w, tar_h);			
			tar_block->marked = true;
			blocks[id] = tar_block;
			

			//upper find the upper block, else find the lower block.
			auto find_block = [&](int tar_x, int tar_y, bool upper) -> Block*{
				Block* cur_block = psuedo_right_block->bl;	
				while(1){
					int flag = false;
					//we can reserve the vertex on upper and left bound.
					while((cur_block->y + cur_block->h < tar_y)
						|| (!upper && cur_block->y + cur_block->h == tar_y)){
						cur_block = cur_block->rt;
						flag = true;
					}

					if(cur_block->x > tar_x){
						cur_block = cur_block->bl;
						flag = true;
					}
					if(!flag) break;
				}
				return cur_block;
			};

			Block* lower_block = find_block(tar_x, tar_y, false);
			Block* higher_block = find_block(tar_x, tar_y + tar_h, true);
			Block* cur_block = lower_block;
			vector<Block*> effected_blocks;
			while(1){
				//if(cur_block->rt && cur_block->rt->y < tar_y + tar_h){
				if(cur_block->y < tar_y + tar_h){
					effected_blocks.push_back(cur_block);
					cur_block = cur_block->rt;

					while(cur_block->x >= tar_x + tar_w) cur_block = cur_block->bl;
				}else break;
			}
			//higher_block = effected_blocks.back();
			cur_block = nullptr;
			
			//use for store the low & right block
			//if the tar is on the bound, it would be nullptr(not change)
			bool flag_lower = false, flag_higher = false;
			Block* lowest_block = nullptr;
			if(lower_block->y == tar_y){	
				lowest_block = lower_block->lb;
				while(lowest_block->x + lowest_block->w <= tar_x + tar_w){
					lowest_block = lowest_block->tr;
				}	

				tar_block->lb = lower_block->lb;
				while(tar_block->x + tar_block->w <= tar_x){
					tar_block->lb = lowest_block->lb->tr;
				}	


			}else{
				lowest_block = new Block(lower_block->x, lower_block->y, lower_block->w, tar_y - lower_block->y);
				lowest_block->rt = lower_block;
				lowest_block->tr = lower_block->tr;
				while(lowest_block->tr->y > lower_block->y) lower_block->tr = lower_block->tr->lb;
				lowest_block->bl = lower_block->bl;
				lowest_block->lb = lower_block->lb;
				
				lower_block->y = tar_y;
				lower_block->h -= lowest_block->h;
				lower_block->lb = lowest_block;
				while(lower_block->bl->y > lower_block->bl->y + lower_block->bl->h) lower_block->bl = lower_block->bl->rt;

				tar_block->lb = lowest_block;
			}


			Block* highest_block = nullptr;
			if(higher_block->y + higher_block->h == tar_y + tar_h){
				highest_block = higher_block->rt;
				while(highest_block->x >= tar_x) highest_block = highest_block->bl; 

				tar_block->rt = highest_block->rt;
				while(tar_block->rt->x >= tar_x + tar_w){
					tar_block->rt = tar_block->rt->bl;
				}	
			}else{
				highest_block = new Block(higher_block->x, tar_y + tar_h, higher_block->w, higher_block->y + higher_block->h - tar_y - tar_h);
				highest_block->lb = higher_block;
				highest_block->bl = higher_block->bl;
				while(highest_block->bl->y + highest_block->bl->h <= highest_block->y) highest_block->bl = highest_block->bl->rt;	
				highest_block->rt = higher_block->rt;
				highest_block->tr = higher_block->tr;

				higher_block->h -= highest_block->h;
				higher_block->rt = highest_block;
				while(higher_block->tr->y >=  higher_block->y + higher_block->h)  higher_block->tr = higher_block->tr->lb;
				
				tar_block->rt = highest_block;
			}


			//tar_block->rt = (lower_block->y == tar_y) ;
			//tar_block->lb = tar_block->bl->lb;

			auto combine_block = [&](Block* b1, Block* b2){
				if(b1->x == b2->x && b1->w == b2->w && b1->rt == b2 && b1->marked == b2->marked){
					b1->h += b2->h;
					b1->rt = b2->rt;
					b1->tr = b2->tr;
					delete b2;


					auto cur_block = b1->tr;
					while(cur_block->y >= b1->y){
						cur_block->bl = b1;
						cur_block = cur_block->lb;
					}

					cur_block = b1->rt;
					while(cur_block->x >= b1->x){
						cur_block->lb = b1;
						cur_block = cur_block->bl;
					}

					cur_block = b1->bl;
					while(cur_block->y + cur_block->h <= b1->y + b1->h){
						cur_block->tr = b1;
						cur_block =cur_block->rt;
					}

					cur_block = b1->lb;
					while(cur_block->x + cur_block->w <= b1->x + b1->w){
						cur_block->rt = b1;
						cur_block =cur_block->tr;
					}
				}
			};



			vector<Block*> new_right_blocks;
			Block* pre_block = lowest_block;



			if(lowest_block->tr && lowest_block->tr->y >= lowest_block->y) 
				lowest_block->tr->bl = lowest_block;

			for(auto block : effected_blocks){
				Block* new_right_block;
				int flag = false;
				if(block->x + block->w == tar_x + tar_w){
					new_right_block = block->tr;
					flag = true;
				}else{
					new_right_block = new Block(tar_x + tar_w, block->y, block->x + block->w - tar_x - tar_w, block->h);
					new_right_block->rt = block->rt;
					new_right_block->tr = block->tr;
				}
				
				new_right_block->bl = tar_block;
				new_right_block->lb = pre_block;

				if(new_right_blocks.size() && new_right_block == new_right_blocks.back()){
				
				}else if(new_right_blocks.size() &&new_right_block->w == new_right_blocks.back()->w
						&& new_right_block->marked == new_right_blocks.back()->marked){
					new_right_blocks.back()->h += new_right_block->h;
					new_right_blocks.back()->rt = new_right_block->rt;
					new_right_blocks.back()->tr = new_right_block->tr;

					delete new_right_block;
				}else {
					new_right_blocks.push_back(new_right_block);
				}

				
				if(new_right_blocks.back()->tr && new_right_blocks.back()->tr->y >= new_right_blocks.back()->y) 
					new_right_blocks.back()->tr->bl = new_right_blocks.back();

				pre_block = new_right_blocks.back();
			}
			
			//solve lb, rt later. REMIND


			if(highest_block->bl && highest_block->bl->y + highest_block->bl->h 
				<= highest_block->y + highest_block->h) 
				highest_block->bl->tr = highest_block;

			vector<Block*> new_left_blocks;
			pre_block = highest_block;
			for(auto it = effected_blocks.rbegin(); it != effected_blocks.rend(); it++){
				auto block = *it;
				Block* new_left_block;
				if(block->x == tar_x){
					new_left_block = block->bl;
				}else{
					new_left_block = new Block(block->x, block->y, tar_x - block->x, block->h);
					new_left_block->bl = block->bl;
					new_left_block->lb = block->lb;
				}

				new_left_block->tr = tar_block;
				new_left_block->rt = pre_block;

				if(new_left_blocks.size() && new_left_block == new_left_blocks.back()){

				}else if(new_left_blocks.size() &&  new_left_block->w == new_left_blocks.back()->w &&  new_left_block->marked == new_left_blocks.back()->marked){
					new_left_blocks.back()->h += new_left_block->h;
					new_left_blocks.back()->bl = new_left_block->bl;
					new_left_blocks.back()->lb = new_left_block->lb;
					
					delete new_left_block;
					
					//combine_block(new_left_block, new_right_blocks.back());
				}else{
					new_left_blocks.push_back(new_left_block);
				}

				if(new_left_blocks.back()->bl && new_left_blocks.back()->bl->y + new_left_blocks.back()->bl->h 
					<= new_left_blocks.back()->y + new_left_blocks.back()->h) 
					new_left_blocks.back()->bl->tr = new_left_blocks.back();
		
				pre_block = new_left_blocks.back();
			}
			
			tar_block->tr = new_right_blocks.back();
			tar_block->bl = new_left_blocks.back();

			//////////////
			/*
			tar_block->rt = tar_block->tr->rt;
			if(tar_block->rt->x >= tar_x + tar_w) tar_block->rt = tar_block->rt->bl;

			tar_block->lb = tar_block->bl->lb;
			if(tar_block->lb->x + tar_block->lb->w <= tar_x) tar_block->lb = tar_block->lb->tr;
			*/


			cur_block = new_right_blocks.back()->rt;
			while(cur_block){
				if(new_right_blocks.back()->x + new_right_blocks.back()->w <= cur_block->x){

				}else if(tar_x + tar_w <= cur_block->x){
					cur_block->lb = new_right_blocks.back();
				}else if(tar_x <= cur_block->x){
					cur_block->lb = tar_block;
				}else if(new_left_blocks.front()->x <= cur_block->x){
					cur_block->lb = new_left_blocks.front();	
				}else break;
				cur_block = cur_block->bl;
			}

			if(lowest_block->y != tar_y){
				cur_block = lowest_block->lb;
				while(cur_block){
					if(cur_block->x + cur_block->w <= lowest_block->x + lowest_block->w){
						cur_block->rt = lowest_block;
					}else break;
					cur_block = cur_block->tr;
				}
			}

			cur_block = new_left_blocks.back()->lb;
			while(cur_block){
				if(new_left_blocks.back()->x >= cur_block->x + cur_block->w){
					// do nothing
				}else if(tar_x  >= cur_block->x + cur_block->w){
					cur_block->rt = new_left_blocks.back();
				}else if(tar_x + tar_w >= cur_block->x + cur_block->w){
					cur_block->rt = tar_block;
				}else if(new_right_blocks.front()->x + new_right_blocks.front()->w >= cur_block->x + cur_block->w){
					cur_block->rt = new_right_blocks.front();
				}else break;
				cur_block = cur_block->tr;
			}

			if(highest_block->y + highest_block->h != tar_y + tar_h){
				cur_block = highest_block->rt;
				while(cur_block){
					if(cur_block->x >= highest_block->x){
						cur_block->lb = highest_block;
					}else break;
					cur_block = cur_block->bl;
				}
			}


			combine_block(new_right_blocks.back(), new_right_blocks.back()->rt);
			combine_block(new_right_blocks.front()->lb, new_right_blocks.front());
			combine_block(new_left_blocks.back()->lb, new_left_blocks.back());
			combine_block(new_left_blocks.front(), new_left_blocks.front()->rt);


			for(auto block : effected_blocks){
				delete block;
			}		
        }

    }


	fstream fout("output.txt", ios::out);
	fout << total_block_num - 4 << endl;
	for(int i=1; i<blocks.size(); i++){
		if(!blocks[i]) break;
		int sum_mark = 0, sum_other = 0;
		Block* cur_block;
		if(blocks[i]->x + blocks[i]->w != width){
			cur_block = blocks[i]->tr;
			if(cur_block->marked) sum_mark++;
			else sum_other++;

			while(cur_block->lb && cur_block->lb->y < cur_block->y && blocks[i]->y < cur_block->lb->y + cur_block->lb->h){
				cur_block = cur_block->lb;
				if(cur_block->marked) sum_mark++;
				else sum_other++;
			}
		}

		if(blocks[i]->y + blocks[i]->h != height){
			cur_block = blocks[i]->rt;
			if(cur_block->marked) sum_mark++;
			else sum_other++;

			while(cur_block->bl && cur_block->bl->x < cur_block->x &&blocks[i]->x < cur_block->bl->x + cur_block->bl->w){
				cur_block = cur_block->bl;
				if(cur_block->marked) sum_mark++;
				else sum_other++;
			}
		}

		if(blocks[i]->x != 0){
			cur_block = blocks[i]->bl;
				if(cur_block->marked) sum_mark++;
				else sum_other++;
			while(cur_block->rt && cur_block->rt->y > cur_block->y && blocks[i]->y + blocks[i]->h > cur_block->rt->y){
				cur_block = cur_block->rt;
				if(cur_block->marked) sum_mark++;
				else sum_other++;
			}
		}

		if(blocks[i]->y != 0){
			cur_block = blocks[i]->lb;
				if(cur_block->marked) sum_mark++;
				else sum_other++;
			while(cur_block->tr && cur_block->tr->x > cur_block->x &&  blocks[i]->x + blocks[i]->w > cur_block->tr->x){
				cur_block = cur_block->tr;
				if(cur_block->marked) sum_mark++;
				else sum_other++;
			}
		}
		fout << i << " " << sum_mark << " " << sum_other << endl;
	}

	for(auto str : res){
		fout << str << endl;
	}

/*
	unordered_set<Block*> bounds;
	bounds.insert(psuedo_lower_block);
	bounds.insert(psuedo_upper_block);
	bounds.insert(psuedo_left_block);
	bounds.insert(psuedo_right_block);
	for(auto mm : m){
		if(bounds.count(mm)) continue;
		cout << mm->x << " " << mm->y << " " << mm->w << " " << mm->h << " ";
		cout << " rt:" << mm->rt->x << " " << mm->rt->y << " ";
		cout << " tr:" << mm->tr->x << " " << mm->tr->y << " ";
		cout << " lb:" << mm->lb->x << " " << mm->lb->y << " ";
		cout << " bl:" << mm->bl->x << " " << mm->bl->y << " ";
		cout << endl;
	}

	for(auto mm : bounds){
		cout << mm->x << " " << mm->y << " " << mm->w << " " << mm->h << " ";
		if(mm->rt) cout << " rt:" << mm->rt->x << " " << mm->rt->y << " ";
		if(mm->tr) cout << " tr:" << mm->tr->x << " " << mm->tr->y << " ";
		if(mm->lb) cout << " lb:" << mm->lb->x << " " << mm->lb->y << " ";
		if(mm->bl) cout << " bl:" << mm->bl->x << " " << mm->bl->y << " ";
		cout << endl;

	}
*/
    return 0;
}
