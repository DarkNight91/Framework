#include <iostream>
#include <vector>
#include <igraph.h>
#include <assert.h>
#include <algorithm>
using namespace std;

struct Gate{
	int id;
	/*PI,PO flags*/
	bool is_input;
	bool is_output;
	/*Arguments to find neighbors in the adjlist*/
	int start_in;
	int no_of_in;
	int start_out;
	int no_of_out;
	/*level*/
	int level;

	int nn;
	int in_degree_v;
	float k_v;
	int ready_cnt;
	int label;
	bool finish;

};
bool LabelSort(Gate* i_begin, Gate* i_end){
	return i_begin->label > i_end->label;
}
bool KValueSort(Gate* i_begin, Gate* i_end){
	return i_begin->k_v < i_end->k_v;
}
bool myfunction(int i, int j) { return (i>j); }

class Graph{
	igraph_t g;
	igraph_vector_t topo;
	// suppose Gates is arranged by gate id
	vector<Gate> Gates;
	// a copy of adjacent list - represent the connections
	vector<int> adj_list;

	// batches after schedule
	vector<vector<int>> res;
	int capcity; // -1 -> infinite
public:
	//Utils
	Graph(igraph_vector_t edges_idx){
		igraph_create(&g, &edges_idx, 0, 1);
		generate_adjlist();
	}

	void generate_adjlist(){
		int no_of_gates = igraph_vcount(&g);
		igraph_vector_t nei_gates_in;
		igraph_vector_init(&nei_gates_in, 0);
		igraph_vector_t nei_gates_out;
		igraph_vector_init(&nei_gates_out, 0);
		int counter = 0;
		for (int i = 0; i < no_of_gates; i++)
		{
			/*Generate adjlist*/
			int v = igraph_neighbors(&g, &nei_gates_in, Gates[i].id, IGRAPH_IN);
			assert(!v);
			v = igraph_neighbors(&g, &nei_gates_out, Gates[i].id, IGRAPH_OUT);
			assert(!v);
			int n1 = igraph_vector_size(&nei_gates_in);
			int n2 = igraph_vector_size(&nei_gates_out);
			Gates[i].no_of_in = n1;
			Gates[i].no_of_out = n2;
			Gates[i].start_in = counter;

			for (int j = 0; j < n1; j++)
			{

				adj_list[counter++] = VECTOR(nei_gates_in)[j];
			}
			Gates[i].start_out = counter;
			for (int k = 0; k < n2; k++)
			{
				adj_list[counter++] = VECTOR(nei_gates_out)[k];
			}
		}
		igraph_vector_destroy(&nei_gates_in);
		igraph_vector_destroy(&nei_gates_out);
	}

	// store topological sorting result
	void topo_sort(){
		igraph_vector_init(&topo, 0);
		igraph_topological_sorting(&g, &topo, IGRAPH_OUT);
	}

	void scheduling(bool coff){
		vector<int> topo_copy;
		vector<int> ready_gates;
		vector<int> new_graph;
		int no_of_gates = igraph_vcount(&g);
		int level = 0;
		int max_fanin = 0;
		int i;

		for (int ii = 0; ii < no_of_gates; ++ii){
			i = VECTOR(topo)[ii];
			Gates[i].ready_cnt = 0;
			Gates[i].in_degree_v = 0;
			Gates[i].k_v = 0.0f;
			Gates[i].label = 0;
			Gates[i].finish = false;
			topo_copy.push_back(i);
		}

		if (coff){
			//coff-gra priorty calculate
			vector<int> tmp1;
			vector<int> tmp2; //store the lexi of optimal indx
			int label_cnt = 1;
			for (int i = 0; i < topo_copy.size(); ++i)
			{
				if (Gates[topo_copy[i]].ready_cnt == Gates[topo_copy[i]].no_of_out){
					ready_gates.push_back(topo_copy[i]);
					topo_copy.erase(topo_copy.begin() + i);
					i--;
				}
			}
			while (!ready_gates.empty()){

				int index = 0;
				tmp2.clear();
				for (int j = Gates[ready_gates[0]].start_out; j < Gates[ready_gates[0]].start_out + Gates[ready_gates[0]].no_of_out; ++j){
					tmp2.push_back(Gates[adj_list[j]].label);
				}
				if (tmp2.empty())
					tmp2.push_back(0);
				else
					sort(tmp2.begin(), tmp2.end(), myfunction);

				for (int i = 1; i < ready_gates.size(); ++i){
					tmp1.clear();
					for (int j = Gates[ready_gates[i]].start_out; j < Gates[ready_gates[i]].start_out + Gates[ready_gates[i]].no_of_out; ++j){
						tmp1.push_back(Gates[adj_list[j]].label);
					}
					if (tmp1.empty())
						tmp1.push_back(0);
					else
						sort(tmp1.begin(), tmp1.end(), myfunction);
					int size;
					if (tmp1.size() < tmp2.size())
						size = tmp1.size();
					else
						size = tmp2.size();
					int flag = 1;
					for (int k = 0; k < size; ++k){
						if (tmp1[k] == tmp2[k]){
							if ((k == size - 1) && (tmp1.size() < tmp2.size()))
								flag = 0;
							continue;
						}
						if (tmp1[k] < tmp2[k]){
							flag = 0;
							break;
						}
					}
					if (!flag){
						index = i;
						tmp2 = tmp1;
					}
				}

				Gates[ready_gates[index]].label = label_cnt++;
				for (int k = Gates[ready_gates[index]].start_in; k < Gates[ready_gates[index]].start_in + Gates[ready_gates[index]].no_of_in; ++k){
					if (!(Gates[adj_list[k]].finish)){
						Gates[adj_list[k]].ready_cnt++;
						if (Gates[adj_list[k]].ready_cnt == Gates[adj_list[k]].no_of_out){
							ready_gates.push_back(adj_list[k]);
							Gates[adj_list[k]].finish = true;
						}
					}
				}
				ready_gates.erase(ready_gates.begin() + index);
				/*
				for (int i = 0; i < old_graph.size(); ++i)
				{
				if (old_graph[i]->ready_cnt == old_graph[i]->no_of_out){
				ready_gates.push_back(old_graph[i]);
				old_graph.erase(old_graph.begin() + i);
				i--;
				}
				}
				*/

				//cout << "Size: " << old_graph.size()<<endl;
			}

			topo_copy.clear();
			ready_gates.clear();
			for (int ii = 0; ii < no_of_gates; ++ii){
				i = VECTOR(topo)[ii];
				Gates[i].ready_cnt = 0;
				Gates[i].in_degree_v = 0;
				Gates[i].k_v = 0.0f;
				topo_copy.push_back(i);
			}
		}

		assert(topo_copy.size() == no_of_gates);
		int cc = 0;
		for (int i = 0; i < topo_copy.size(); ++i)
		{
			if (Gates[topo_copy[i]].ready_cnt == Gates[topo_copy[i]].no_of_in){
				cc++;
				ready_gates.push_back(topo_copy[i]);
				topo_copy.erase(topo_copy.begin() + i);
				i--;
			}
		}
		/////////////////////////////////////////////////////////////////////////////////////////
		while (!ready_gates.empty()){

			if (ready_gates.size() < capcity){
				for (int i = 0; i < ready_gates.size(); i++){
					Gates[ready_gates[i]].level = level;
					for (int j = Gates[ready_gates[i]].start_out; j < Gates[ready_gates[i]].start_out + Gates[ready_gates[i]].no_of_out; ++j)
						Gates[adj_list[j]].ready_cnt++;
				}
				res.insert(res.end(), ready_gates.begin(), ready_gates.end());
				ready_gates.clear();
				level++;
			}
			else{
				if (coff){
					sort(ready_gates.begin(), ready_gates.end(), LabelSort); //Error!
					for (int i = 0; i < ready_gates.size(); ++i){
					}
					int i_begin = 0, i_end = capcity;
					for (int i = i_begin; i < i_end; ++i){
						Gates[ready_gates[i]].level = level;
						//cout << ready_gates[i]->label << endl;
						for (int j = Gates[ready_gates[i]].start_out; j < Gates[ready_gates[i]].start_out + Gates[ready_gates[i]].no_of_out; ++j)
							Gates[adj_list[j]].ready_cnt++;
					}
					res.insert(res.end(), ready_gates.begin() + i_begin, ready_gates.begin() + i_end);
					ready_gates.erase(ready_gates.begin() + i_begin, ready_gates.begin() + i_end);
					int t2 = ready_gates.size();
					level++;
				}
				else {
					max_fanin = 0;
					for (int i = 0; i < ready_gates.size(); ++i){
						if (Gates[ready_gates[i]].no_of_in > max_fanin)
							max_fanin = Gates[ready_gates[i]].no_of_in;
					}

					if (max_fanin)
						for (int i = 0; i < ready_gates.size(); ++i)
							Gates[ready_gates[i]].k_v = Gates[ready_gates[i]].no_of_in / max_fanin;
					sort(ready_gates.begin(), ready_gates.end(), KValueSort);
					int i_begin, i_end;
					float min_k_diff = 10000.0f;
					for (int i = 0, j = capcity; j < ready_gates.size(); ++i, ++j){
						if (min_k_diff >(Gates[ready_gates[j]].k_v - Gates[ready_gates[i]].k_v))
						{
							min_k_diff = Gates[ready_gates[j]].k_v - Gates[ready_gates[i]].k_v;
							i_begin = i; i_end = j;
						}
					}

					for (int i = i_begin; i < i_end; ++i){
						Gates[ready_gates[i]].level = level;
						for (int j = Gates[ready_gates[i]].start_out; j < Gates[ready_gates[i]].start_out + Gates[ready_gates[i]].no_of_out; ++j)
							Gates[adj_list[j]].ready_cnt++;
					}
					new_graph.insert(new_graph.end(), ready_gates.begin() + i_begin, ready_gates.begin() + i_end);
					ready_gates.erase(ready_gates.begin() + i_begin, ready_gates.begin() + i_end);
					level++;
				}
			}

			for (int i = 0; i < topo_copy.size(); ++i)
			{
				if (Gates[topo_copy[i]].ready_cnt == Gates[topo_copy[i]].no_of_in){
					cc++;
					ready_gates.push_back(topo_copy[i]);
					topo_copy.erase(topo_copy.begin() + i);
					i--;
				}
			}
		}

		//assert(new_graph.size() == no_of_gates);
	}
};
