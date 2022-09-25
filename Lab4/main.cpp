#include "bits/stdc++.h"
#include "message.h"
#include "rplParser.h"
using namespace std;

int factor = 70;
struct Row_data{
	vector<int> contain;
	int width = 0;
	int left = 0, right = 0;
	rplRow_S *origialRow;
};
vector<Row_data> cur_rows;

struct Cluster{
	int x = 0;
	int weight = 0;
	int width = 0;
	int first = 0, last = 0;
	int quad = 0;
};


vector<pair<int, int>> res;
void abacus(rplData_S*);
double placerow(rplData_S* pData, int row_id, int x, int y, bool writein = false);
void addcell(rplData_S* pData, Cluster& cluster, int i);
void addcluster(rplData_S* pData, Cluster& cluster1, Cluster& cluster2);
void collapse(rplData_S*, vector<Cluster>& clusters, int row_id);
void printData(rplData_S* pData);

int main(int argc, char* argv[])
{
    /*Preliminary Information*/
    /*std::cout << "     ################################################################" << std::endl;
    std::cout << "     #                                                              #" << std::endl; 
    std::cout << "     #                        [5037 PDA - Lab4]                     #" << std::endl; 
    std::cout << "     #          Placement Legalization with Minimal Displacement    #" << std::endl; 
    std::cout << "     #   Compiled On : "<<__DATE__<<" at "<<__TIME__<<" on "<<sizeof(char*)*8<<"-bit machine    #" << std::endl;
    std::cout << "     #                    VLSI Design Automation Lab                #" << std::endl; 
    std::cout << "     #              Institute of Electronics Engineering            #" << std::endl; 
    std::cout << "     #                  National Chiao Tung University              #" << std::endl; 
    std::cout << "     #                                                              #" << std::endl; 
    std::cout << "     ################################################################" << std::endl;
    std::cout << std::endl;*/

    clock_t start, end;
    start = clock();
    rplParser_C* pRplParser = new rplParser_C(argv[1]);
    pRplParser->parse();
    
    /* Code Begin */

    //Example Code
	
    rplData_S *pData = pRplParser->getRplData();
	if(pData->vNode.size() > 2'000'000) factor = 30;	
	//rplNode_S* pNode(0);
    /*printf("node - width height\n");
  	 
	for(unsigned int nNodeIter=0; nNodeIter < 100 && pData->vNode.size();++nNodeIter)
    {
        pNode = pData->vNode[nNodeIter];
		if(!pNode) cout << "!!!!";
		else printf("%s - %d %d\n",pNode->nodeName, pNode->width, pNode->height);
    }*/
    //return 0;
	//cout << pData->vPlNode.size() << " "<< pData->vNode.size() << endl;
	abacus(pData);
	printData(pData);

    /* Code End */
    
	//Dumper dumpParser("output.pl"); //Optional
    //pRplParser->dump(dumpParser);    //Optional
    //delete pRplParser;

    end = clock();
    //InfoMsg("Execution Time: %lf seconds\n", (double)(end-start)/CLOCKS_PER_SEC);
}

void abacus(rplData_S* pData){
	vector<int> vNodeId;
	vector<int> terminalId;
	int error_flag = false;
	for(int i=0; i<pData->vNode.size(); i++){
		//if(pData->vNode[i] == NULL) cout<< "*****";
		if(pData->vNode[i]->nodeType == 0){
			vNodeId.push_back(i);
		}else if(pData->vNode[i]->nodeType == 1) {
			terminalId.push_back(i);
		}
		//if(terminalId.size() && pData->vNode[i]->nodeType == 0) { cout << "!";}
	}
	//cout << pData->nFixNode << endl;
	//cout << pData->vNode.size()<< " " << vNodeId.size() << " " << terminalId.size( ) << endl;
	//cout << pData->vNode[terminalId.front()]->nodeName << " "<< endl;
	//cout << pData->vPlNode[terminalId.back()]->plNodeName << " " << endl;
	
	//cur_rows.resize(pData->vRow.size());
	//cout << terminalId.size() << endl;
	for(int i=0; i<pData->vRow.size(); i++){
		auto cur_rplrow = pData->vRow[i];
		
		map<int, int> segs;
		for(int& t : terminalId){
			int bottom = pData->vPlNode[t]->plNodeY;
			int top = bottom + pData->vNode[t]->height;
			int row_bottom = cur_rplrow->rowCoord;
			int row_top = row_bottom + cur_rplrow->rowHeight;
			if(top <= row_bottom || bottom >= row_top) continue;
			
			int left = pData->vPlNode[t]->plNodeX;
			int right = left + pData->vNode[t]->width;
			segs[left]++;
			segs[right]--;
		}
		//cout << cur_rplrow->rowCoord + cur_rplrow->rowHeight<< ":" << segs.size() << " ";
		//if(i == 1000) exit(1);
		int row_left = cur_rplrow->rowSubRowOrig;
		int row_right = row_left + cur_rplrow->rowSiteSpace * cur_rplrow->numSite;
		int flag = 0;
		for(auto& seg : segs){
			if(flag==0 && seg.second > 0){
				if(row_left < seg.first){
					Row_data new_row; 
					/*	
					vector<int> contain;
					int width = 0;
					int left, right;
					rplRow_S *origialRow;
					*/
					new_row.left = row_left;
					new_row.right = seg.first;
					new_row.origialRow = cur_rplrow;
					cur_rows.push_back(new_row);
					if(seg.first >= row_right){
						cur_rows.back().right = row_right;
						row_left = row_right;
						break;
					}
				}
			}

			flag += seg.second;	
			if(!flag) {
				row_left = max(row_left, seg.first);
			}
		}
		
		if(row_left < row_right){
			Row_data new_row; 
			new_row.left = row_left;
			new_row.right = row_right;
			new_row.origialRow = cur_rplrow;
			cur_rows.push_back(new_row);
		}
		/*
		auto& row = cur_rows[i];
		row.width = 0;
		row.left = pData->vRow[i]->rowSubRowOrig;
		row.right = row.left + pData->vRow[i]->rowSiteSpace * pData->vRow[i]->numSite;*/
	}
	//cout << cur_rows.size() << " " << pData->vRow.size() << endl;

	sort(vNodeId.begin(), vNodeId.end(), [&](int& n1, int& n2){
		return pData->vPlNode[n1]->plNodeX < pData->vPlNode[n2]->plNodeX;
	});
	res.resize(pData->vPlNode.size());
/*
	for(auto row : cur_rows){
		cout << row.left << " ";
	}
	int tmp = 0;
	for(int i=0; i<100; i++)
		cout << " "<< pData->vPlNode[vNodeId[i]]->plNodeX << " \n";
	return ;
*/
	for(int& id : vNodeId){
		double best_cost = DBL_MAX;
		int best_row;

		int tar = distance(cur_rows.begin(), 
			lower_bound(cur_rows.begin(), cur_rows.end(), pData->vPlNode[id]->plNodeY, [&](const Row_data& r, const int val) {
			return r.origialRow->rowCoord < val; 
		}));
		//cout << left << endl;
		//if(tmp++ ==100) exit(1);
		int fail = false;
		for(int sl=0;  sl< cur_rows.size(); sl++){
			//int fail = true;
			for(int type = 0; type<2; type++)
			{
				int i = type ? tar - sl - 1 : tar + sl;
				if(i < 0 || i >= cur_rows.size()) continue;	
				if(//pData->vPlNode[id]->plNodeX  < cur_rows[i].left ||
				//		pData->vPlNode[id]->plNodeX  > cur_rows[i].right ||
						pData->vNode[id]->width > cur_rows[i].right - cur_rows[i].left - cur_rows[i].width
				  ) continue;
		
			/*	
			cout << row->rowCoord << " ";		
			cout << row->rowHeight << " ";		
			cout << row->rowSiteWidth << " ";		
			cout << row->rowSiteSpace << " ";		
			cout << row->rowSiteOrient << " ";		
			cout << row->rowSiteSym<< " ";		
			cout << row->rowSubRowOrig << " ";		
			cout << row->numSite << " ";		
			cout << endl;*/
			// if we cut row must cheack again;
			
			int original_x = pData->vPlNode[id]->plNodeX;
			if(pData->vPlNode[id]->plNodeX < cur_rows[i].left)
				pData->vPlNode[id]->plNodeX = cur_rows[i].left;
			
			if(pData->vPlNode[id]->plNodeX + pData->vNode[id]->width > cur_rows[i].right)
				pData->vPlNode[id]->plNodeX = cur_rows[i].right - pData->vNode[id]->width;
			
			cur_rows[i].contain.push_back(id);
			double cur_cost = placerow(pData, i, original_x, pData->vPlNode[id]->plNodeY);
			cur_rows[i].contain.pop_back();

			if(cur_cost < best_cost){
				best_cost = cur_cost;
				best_row = i;
			}			
			pData->vPlNode[id]->plNodeX = original_x;
			}
			if(sl > factor && best_cost != DBL_MAX) break;

		}
		//cout << best_row << endl;	
		if(pData->vPlNode[id]->plNodeX < cur_rows[best_row].left)
			pData->vPlNode[id]->plNodeX = cur_rows[best_row].left;

		if(pData->vPlNode[id]->plNodeX + pData->vNode[id]->width > cur_rows[best_row].right)
			pData->vPlNode[id]->plNodeX = cur_rows[best_row].right - pData->vNode[id]->width;
		
		cur_rows[best_row].contain.push_back(id);
		if(placerow(pData, best_row, pData->vPlNode[id]->plNodeX, pData->vPlNode[id]->plNodeY, true) == DBL_MAX){
			cout << "ERROR";
			exit(1);
		}
		cur_rows[best_row].width += pData->vNode[id]->width;
	}
}

double placerow(rplData_S* pData, int row_id, int x, int y, bool writein){
	vector<Cluster> clusters;
	for(int id : cur_rows[row_id].contain){
		if(!clusters.size() || 
			clusters.back().x + clusters.back().width < pData->vPlNode[id]->plNodeX){
			Cluster cur_cluster;
			addcell(pData, cur_cluster, id);
			cur_cluster.x = pData->vPlNode[id]->plNodeX;
			cur_cluster.first = id;
			clusters.push_back(cur_cluster);
		}else {
			addcell(pData, clusters.back(), id);
			collapse(pData, clusters, row_id);
			//cout << "****" << clusters.back().x;
		}
	}
	if(!writein){
		int fin_x = clusters.back().x + clusters.back().width 
			- pData->vNode[cur_rows[row_id].contain.back()]->width - x;
		int fin_y = cur_rows[row_id].origialRow->rowCoord - y;
		return fin_x * fin_x + fin_y * fin_y;
	}else{
		int contain_id = 0;
		for (auto& cluster : clusters) {
			int x = cluster.x;
			//cout << x << " ";
			while(contain_id < cur_rows[row_id].contain.size()) {
				res[cur_rows[row_id].contain[contain_id]] = {x, cur_rows[row_id].origialRow->rowCoord};
				if(cur_rows[row_id].contain[contain_id] == cluster.last) {
					contain_id++;
					break;
				}
				//cout << "\n***" << pData->vNode[cur_rows[row_id].contain[contain_id]]->width;
				x += pData->vNode[cur_rows[row_id].contain[contain_id]]->width;
				//cout << "\n***" << x << " ";
				contain_id++;
			}
		}
		return 0.0;
	}
}

void addcell(rplData_S* pData, Cluster& cluster, int i){
	cluster.last = i;
	cluster.weight++;
	cluster.quad = pData->vPlNode[i]->plNodeX - cluster.width;
	cluster.width += pData->vNode[i]->width;
}

void addcluster(rplData_S* pData, Cluster& cluster1, Cluster& cluster2){
	cluster1.last = cluster2.last;
	cluster1.weight += cluster2.weight;
	cluster1.quad += cluster2.quad - cluster1.width * cluster2.weight;
	cluster1.width += cluster2.width;
}

void collapse(rplData_S* pData, vector<Cluster>& clusters, int row_id){
	while(1){
		//cout << "!!!" << clusters.back().x;
		clusters.back().x = max(clusters.back().quad / clusters.back().weight, cur_rows[row_id].left);
		//cout << "!!!" << clusters.back().x;
		if(clusters.back().x + clusters.back().width > cur_rows[row_id].right)
			clusters.back().x = cur_rows[row_id].right - clusters.back().width;
		int size = clusters.size();
		//cout << "!!!" << clusters.back().width << " " << clusters.back().x;
		//exit(1);
		if(clusters.size() == 1 || clusters[size - 2].x + clusters[size - 2].width < clusters[size - 1].x)
			break;
 		addcluster(pData, clusters[size - 2], clusters[size - 1]);
		clusters.pop_back();
	}
}

void printData(rplData_S* pData){
	fstream fout("output.pl", ios::out);
	fout << endl;
	fout << endl;
	fout << endl;
	fout << endl;
	for(int i=0; i<pData->vPlNode.size(); i++){
		//o21144	4797	5451	:	N	/FIXED
		int type = pData->vPlNode[i]->plNodeFixed;
		fout << pData->vPlNode[i]->plNodeName << "\t";
		if(type != 0){
			fout << pData->vPlNode[i]->plNodeX << "\t";
			fout << pData->vPlNode[i]->plNodeY << "\t";
		}else {
			fout << res[i].first << "\t";
			fout << res[i].second << "\t";
		}
		fout << ":\t";
		fout << "N\t";
		if(pData->vPlNode[i]->plNodeFixed == 1) fout << "terminal\t";
		else if(pData->vPlNode[i]->plNodeFixed == 2) fout << "terminal_NI" << "\t";
		fout << endl;
	}	
}
