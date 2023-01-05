//This file declares class VONEHeuristic.
//------------------------------------------------
//File Name: Heuristic.h
//Author: Qihan Zhang
//Email: lengkudaodi@outlook.com
//Last Modified: Aug. 3rd 2021
//------------------------------------------------

#pragma once
#include "Topo.h"

namespace VONEHeuristic
{
	//The set of Embedded Vertexes, each vertex includes two properties: 
	//    properties in STRUCT Vertex and Remaining Computing Resource
	struct Utilited_Vertex
	{
		VONETopo::Vertex _vertex;
		int _remaining_computing_resource;
	};

	//The set of Embedded Edges, each edge includes four properties: 
	//    properties in STRUCT Edge, Remaining Bandwidth, 
	//    Frequency Status and End Frequency Slot allocating status of  each core
	struct Utilited_Edge
	{
		VONETopo::Edge _edge;
		int _path_distance;
		vector<vector<int> > _frequency;
		vector<int> _end_slot;
	};

	////Create Heuristic class to solve VONE and store the embedding solutions
	class VONEHeuristic
	{
	public:
		VONEHeuristic();

		void solve(const VONETopo::Topo& Substrate_Network, vector<VONETopo::Topo>& Virtual_Network_List,
			int CoreNumber, int HeterogeneousCoreIndex, string CrosstalkYesorNo);
		//Precondition: Topologies of substrate netwrok and virtual networks have been assigned
		//Postcondition: Solve the heuristic of VONE based on the Substrate_Network and Virtual_Network_List

		friend ostream& operator <<(ostream& outs, const VONEHeuristic& VONEHeuristic);
		//Precondition: outs has been connected to an out stream
		//Postcondition: Print the solution information of the heuristic solution to outs

		~VONEHeuristic();

	private:
		vector<vector<int> > _vertex_embedding_result;
		vector<vector<vector<int> > > _edge_embedding_result;
		vector<vector<int> > _core_assignment_result;
		vector<int> _start_frequency_result;
		vector<vector<vector<int> > > _frequency_allocating_result;
		vector<vector<int> > _maximum_frequency_slot_index;
		vector<vector<int> > _fragmetation;
		vector<int> _crosstalk;
		int _block_num;
	};
}