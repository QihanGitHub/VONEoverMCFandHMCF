//This file defines class VONEHeuristic.
//------------------------------------------------
//File Name: Heuristic.cpp
//Author: Qihan Zhang
//Email: lengkudaodi@outlook.com
//Last Modified: Aug. 6th 2021
//------------------------------------------------

#include "Heuristic.h"

#include <algorithm>
#include <numeric>
#include <cstdlib>
#include <iterator>
#include <cmath>
#include <fstream>

namespace
{
	bool VNcompare(const VONETopo::Topo& VNA, const VONETopo::Topo& VNB);
	//Precondition: The Topo VNA and VNB has been initialized
	//Postcondition: Return the comparison results between VNA and VNB according to vertex number first and then edge number

	bool VNVcompare(const VONETopo::Vertex& VA, const VONETopo::Vertex& VB);
	//Precondition: The Vertex VA and VB has been initialized
	//Postcondition: Return the comparison results between VA and VB according to remaining computing resource

	bool SNUVcompare(const VONEHeuristic::Utilited_Vertex& UVA, const VONEHeuristic::Utilited_Vertex& UVB);
	//Precondition: The Vertex UVA and UVB has been initialized
	//Postcondition: Return the comparison results between UVA and UVB according to remaining computing resource

	int Request_Type(const int Bandwidth);
	//Precondition: Bandwidth has been assignment
	//Postcondition: Return the request type of that the edge_width equals Bandwidth

	int Core_Distance(const int Core1_index, const int Core2_index, const int C);
	//Precondition: Core1_index and Core2_index have been assigned
	//Postcondition: Return the distance of two core index, which is an auxiliary function for function Nearest_Core_with_Same_Cost

	int Nearest_Core_with_Same_Cost(const vector<int>& Core_Cost, const vector<int>::iterator& Initial_index, const int Last_Priority_Core, const int CoreNumber);
	//Precondition: Core_Cost, Initial_index and Last_Priority_Core have been assigned
	//Postcondition: Return the nearest core of Last_Priority_Core in the same cost core, which is an auxiliary function for function Core_Priority_Define

	void Core_Priority_Define(vector<int>& Core_Priority, const int Hetergeneous_Core, const int CoreNumber, ofstream& outs);
	//Precondition: Core_Priority has been assignment, Hetergeneous indicates the index of the hetergeneous core, outs has been connected to a file stream
	//Postcondition: Core Priority of all cores has been defined into Core_Priority, and log file has been recorded

	void Core_Classification_Define(vector<int>& Core_Classification, const vector<int> Core_Priority, const vector<int> Service_Type, const int CoreNumber, ofstream& outs);
	//Precondition: Core_Priority_Define has been excuted, and Core_Classification has been assignment, Service_Type has been listed, 
	//    and outs has been connected to a file stream
	//Postcondition: Core Classification has been defined into Core_Classification, and log file has been recorded

	void Vertexes_Embedding(VONETopo::NodeList& Virtual_Network_NodeList, vector<VONEHeuristic::Utilited_Vertex>& SN_Utilited_Vertex,
		vector<int>& vertex_embedding_result, ofstream& outs);
	//Precondition: Virtual_Network_NodeList, SN_Utilited_Vertex and vertex_embedding_result has been assignment, and outs has been connected to a file stream
	//Postcondition: The result of vertexes embedding has been put into vertex_embedding_result, and log file has been recorded

	void get_SN_EdgeList(VONETopo::LinkList& SN_Edge, const vector<VONEHeuristic::Utilited_Edge>& SN_Utilited_Edge);
	//Precondition: SN_Edge, SN_Utilited_Edge has been assignment
	//Postcondition: _edge in SN_Utilited_Edge has been put into SN_Edge, which is an auxiliary function for function Edges_Embedding, Core_Assignment and Frequency_Allocating

	void Edges_Embedding(const VONETopo::LinkList& Virtual_Network_EdgeList, const int Number_of_SN_Vertex,
		const vector<VONEHeuristic::Utilited_Edge>& SN_Utilited_Edge, const vector<int>& vertex_embedding_result, vector<vector<int> >& edge_embedding_result, ofstream& outs);
	//Precondition: Virtual_Network_NodeList, Number_of_SN_Vertex, SN_Utilited_Edge, vertex_embedding_result and edge_embedding_result has been assignment, 
	//    and outs has been connected to a file stream
	//Postcondition: The result of edges embedding has been put into edge_embedding_result, and log file has been recorded

	void Frequency_Recovery(const vector<vector<int> >& edge_embedding_result, vector<VONEHeuristic::Utilited_Edge>& SN_Utilited_Edge, const int CoreNumber);
	//Precondition: edge_embedding_result and SN_Utilited_Edge have been assignment
	//Postcondition: Clear the pre-allocating frequency slots in SN_Utilited_Edge

	int Core_Index(const int node1_id, const int node2_id, const int current_core);
	//Precondition: node1_id, node2_id and current_core have been assignment
	//Postcondition: Return the core index of source node1_id to destination node2_id

	void Core_Assignment(const vector<vector<int> >& edge_embedding_result, const vector<int> Core_Priority, const int Hetergeneous_Core,
		vector<VONEHeuristic::Utilited_Edge>& SN_Utilited_Edge, vector<int>& Core_Classification, int VN_Bandwidth,
		vector<int>& core_assignment_result, int& start_frequency, const int CoreNumber, ofstream& outs);
	//Precondition: edge_embedding_result, Core_Priority, Hetergeneous_Core, SN_Utilited_Edge, Core_Classification, VN_Bandwidth, core_assignment_result 
	//    and start_frequency have been assignment, and outs has been connnected to a file stream
	//Postcondition: The result of core assignment and start frequency have been stored into core_assignment_result and start_frequency, 
	//    and log file has been recorded

	void Frequency_Allocating(const vector<vector<int> >& edge_embedding_result, const vector<int> core_assignment_result, const int start_frequency,
		const int Hetergeneous_Core, vector<VONEHeuristic::Utilited_Edge>& SN_Utilited_Edge, int VN_Bandwidth);
	//Precondition: edge_embedding_result, core_assignment_result, start_frequency, Hetergeneous_Core, SN_Utilited_Edge, and VN_Bandwidth have been assignment
	//Postcondition: The result of frequency allocating has been stored into SN_Utilited_Edge
}

namespace VONEHeuristic
{
	VONEHeuristic::VONEHeuristic() : _block_num(0)
	{

	}

	void VONEHeuristic::solve(const VONETopo::Topo& Substrate_Network,
		vector<VONETopo::Topo>& Virtual_Network_List, int CoreNumber, 
		int HeterogeneousCoreIndex, string CrosstalkYesorNo)
	{
		ofstream fout;
		fout.open("log.dat"); //Use log file to record some information
		if (fout.fail())
		{
			cerr << "File log.dat opening failed.\n";
			exit(1);
		}

		//Initialize the utilited vertex of the substrate network:
		vector<Utilited_Vertex> SN_Utilited_Vertex;
		VONETopo::NodeList SN_Vertex;
		Substrate_Network.get_vertex(SN_Vertex);
		for (VONETopo::NodeListIT iter = SN_Vertex.begin(); iter != SN_Vertex.end(); iter++)
		{
			Utilited_Vertex tmp;
			tmp._vertex = *iter;
			tmp._remaining_computing_resource = iter->_vertex_computing_capacity;
			SN_Utilited_Vertex.push_back(tmp);
		}

		//Initialize the utilited edge of the substrate network:
		vector<Utilited_Edge> SN_Utilited_Edge;
		VONETopo::LinkList SN_Edge;
		Substrate_Network.get_edge(SN_Edge);
		for (VONETopo::LinkList::iterator iter = SN_Edge.begin(); iter != SN_Edge.end(); iter++)
		{
			Utilited_Edge tmp;
			tmp._edge = *iter;

			for (int i = 1; i <= CoreNumber; i++)
			{
				vector<int> frequency_tmp(iter->_edge_bandwidth);
				tmp._frequency.push_back(frequency_tmp); //Link smaller source to bigger destination
				tmp._frequency.push_back(frequency_tmp); //Link bigger source to smaller destination
				tmp._end_slot.push_back(1);
			}
			tmp._path_distance = 1;

			SN_Utilited_Edge.push_back(tmp);
		}

		//Sort the virtual networks according to the vertex size and edge size:
		sort(Virtual_Network_List.begin(), Virtual_Network_List.end(), VNcompare);

		for (vector<VONETopo::Topo>::iterator iter = Virtual_Network_List.begin(); iter != Virtual_Network_List.end(); iter++)
		{
			fout << "Sorted Virtual Request Topology " << iter - Virtual_Network_List.begin() + 1 << " are following:\n";
			fout << *iter << endl;
		}

		//Core Priority:
		vector<int> Core_Priority(CoreNumber);
		if ((CrosstalkYesorNo == "Yes") || (CrosstalkYesorNo == "yes"))
		{
			for (int i = 1; i <= Core_Priority.size(); i++)
			{
				Core_Priority[i - 1] = i;
			}
		}
		else
			Core_Priority_Define(Core_Priority, HeterogeneousCoreIndex, CoreNumber, fout);

		//Core Classfication:
		vector<int> Core_Classification(CoreNumber);
		if (HeterogeneousCoreIndex == 0)
		{			
			if ((CrosstalkYesorNo == "No") || (CrosstalkYesorNo == "no"))
			{
				vector<int> Service_Type = { 1, 2, 3 };
				Core_Classification_Define(Core_Classification, Core_Priority, Service_Type, CoreNumber, fout);
			}				
		}
		else
		{			
			if ((CrosstalkYesorNo == "No") || (CrosstalkYesorNo == "no"))
			{
				vector<int> Service_Type = { 1, 3 };
				Core_Classification_Define(Core_Classification, Core_Priority, Service_Type, CoreNumber, fout);
			}			
		}

		//Embedding start:
		for (vector<VONETopo::Topo>::iterator titer = Virtual_Network_List.begin();
			titer != Virtual_Network_List.end(); titer++)
		{
			//Vertexes embedding:	
			VONETopo::NodeList VNNode;
			titer->get_vertex(VNNode);
			vector<int> vertex_embedding_result;
			Vertexes_Embedding(VNNode, SN_Utilited_Vertex, vertex_embedding_result, fout);
			_vertex_embedding_result.push_back(vertex_embedding_result);
			if (vertex_embedding_result.empty())
			{
				_block_num++;
				continue;
			}

			//Edges embedding:
			VONETopo::LinkList VNLink;
			titer->get_edge(VNLink);
			vector<vector<int> > edge_embedding_result;
			Edges_Embedding(VNLink, static_cast<const int>(SN_Utilited_Vertex.size()), SN_Utilited_Edge, vertex_embedding_result, edge_embedding_result, fout);
			_edge_embedding_result.push_back(edge_embedding_result);
			if (edge_embedding_result.empty())
			{
				_block_num++;
				_vertex_embedding_result[titer - Virtual_Network_List.begin()].clear();
				continue;
			}

			//Core Assignment:
			vector<int> core_assignment_result;
			int start_frequency_result = 0;
			int VN_Bandwidth = VNLink[0]._edge_bandwidth;
			Core_Assignment(edge_embedding_result, Core_Priority, HeterogeneousCoreIndex,
				SN_Utilited_Edge, Core_Classification, VN_Bandwidth,
				core_assignment_result, start_frequency_result, CoreNumber, fout);
			_core_assignment_result.push_back(core_assignment_result);
			_start_frequency_result.push_back(start_frequency_result);
			if (core_assignment_result.empty())
			{
				_block_num++;
				_vertex_embedding_result[titer - Virtual_Network_List.begin()].clear();
				_edge_embedding_result[titer - Virtual_Network_List.begin()].clear();
				continue;
			}

			//Frequency Allocating:
			Frequency_Allocating(edge_embedding_result, core_assignment_result, 
				start_frequency_result, HeterogeneousCoreIndex, SN_Utilited_Edge, VN_Bandwidth);
		}

		//Storing the frequency status of each substrate network link:
		for (vector<Utilited_Edge>::iterator iter = SN_Utilited_Edge.begin();
			iter != SN_Utilited_Edge.end(); iter++)
		{
			_frequency_allocating_result.push_back(iter->_frequency);
		}

		//Counting the maximum frequency slot and fragmetation of each core in each substrate network link:
		for (unsigned int i = 1; i <= _frequency_allocating_result.size(); i++)
		{
			vector<int> tmp1;
			vector<int> tmp2;
			int crosstalk = 0;
			for (unsigned int j = 1; j <= _frequency_allocating_result[i-1].size(); j++)
			{
				for (int k = static_cast<int>(_frequency_allocating_result[i - 1][j - 1].size()); k >= 1; k--)
				{
					if (_frequency_allocating_result[i - 1][j - 1][k - 1] != 0)
					{
						tmp1.push_back(k);
						break;
					}
					else if (k == 1)
					{
						tmp1.push_back(0);
					}
					else
						continue;
				}

				int fragmentation = 0;
				for (int k = 1; k <= tmp1[j-1]; k++)
				{
					if (_frequency_allocating_result[i - 1][j - 1][k - 1] == 0)
					{
						fragmentation += 1;
					}
					else if ((j != 2* HeterogeneousCoreIndex-1) && (j != 2* HeterogeneousCoreIndex) &&
						((j % 2 == 0 ? j % (2 * CoreNumber) + 2 : (j + 1) % (2 * CoreNumber) + 1) != 2* HeterogeneousCoreIndex -1) &&
						((j % 2 == 0 ? j % (2 * CoreNumber) + 2 : (j + 1) % (2 * CoreNumber) + 1) != 2 * HeterogeneousCoreIndex) &&
						_frequency_allocating_result[i - 1][j - 1][k - 1] == 
						_frequency_allocating_result[i - 1][(j%2==0 ? j%(2*CoreNumber)+2 : (j+1)%(2*CoreNumber)+1)- 1][k - 1])
					{
						crosstalk += 1;
					}
					else
						continue;
				}
				tmp2.push_back(fragmentation);
			}
			_maximum_frequency_slot_index.push_back(tmp1);
			_fragmetation.push_back(tmp2);
			_crosstalk.push_back(crosstalk);
		}

		fout.close();
	}

	ostream& operator <<(ostream& outs, const VONEHeuristic& VONEHeuristic)
	{
		//Output the result of vertex embedding:
		outs << "----------Vertexes Embedding Result----------\n\n";
		for (unsigned int i = 1; i <= VONEHeuristic._vertex_embedding_result.size(); i++)
		{
			if (!VONEHeuristic._vertex_embedding_result[i - 1].empty())
			{
				for (unsigned int j = 1; j <= VONEHeuristic._vertex_embedding_result[i - 1].size(); j++)
				{
					outs << "The node " << j << " in VN " << i << " is embedded in SN " << VONEHeuristic._vertex_embedding_result[i - 1][j - 1] << endl;
				}
				outs << endl;
			}
			else
				outs << "Sorted Virtual Network " << i << " will be blocked!\n\n";

		}
		outs << "---------------------------------------------\n\n";

		//Output the result of edge embedding:
		outs << "----------Edges Embedding Result----------\n\n";
		for (unsigned int i = 1; i <= VONEHeuristic._edge_embedding_result.size(); i++)
		{
			if (!VONEHeuristic._edge_embedding_result[i - 1].empty())
			{
				for (unsigned int j = 1; j <= VONEHeuristic._edge_embedding_result[i - 1].size(); j++)
				{
					outs << "The link " << j << " in VN " << i << " is embedding in SN link: ";
					unsigned int k = 1;
					for (; k < VONEHeuristic._edge_embedding_result[i - 1][j - 1].size()-1; k++)
					{
						outs << VONEHeuristic._edge_embedding_result[i - 1][j - 1][k - 1] << "->";
					}
					outs << VONEHeuristic._edge_embedding_result[i - 1][j - 1][k - 1] << ", with the cost: " 
						<< VONEHeuristic._edge_embedding_result[i - 1][j - 1][k] << endl;
				}
				outs << endl;
			}
			else
				outs << "Sorted Virtual Network " << i << " will be blocked!\n\n";
		}
		outs << "------------------------------------------\n\n";

		//Output the result of core assignment:
		outs << "----------Core Assignment Result----------\n\n";
		for (unsigned int i = 1; i <= VONEHeuristic._core_assignment_result.size(); i++)
		{
			if (!VONEHeuristic._core_assignment_result[i - 1].empty())
			{
				outs << "The Core Assignment in VN " << i << " is: ";
				for (unsigned int j = 1; j <= VONEHeuristic._core_assignment_result[i - 1].size(); j++)
				{
					if (j != VONEHeuristic._core_assignment_result[i - 1].size())
						outs << VONEHeuristic._core_assignment_result[i - 1][j - 1] / 2 + 1 << "-";
					else
						outs << VONEHeuristic._core_assignment_result[i - 1][j - 1] / 2 + 1 << endl;
				}
				outs << endl;
			}
			else
				outs << "Sorted Virtual Network " << i << " will be blocked!\n\n";
		}
		outs << "------------------------------------------\n\n";

		//Output the result of start frequency:
		outs << "----------Start Frequency Result----------\n\n";
		for (unsigned int i = 1; i <= VONEHeuristic._start_frequency_result.size(); i++)
		{
			if (VONEHeuristic._start_frequency_result[i - 1] != 0)
			{
				outs << "The Start Frequency in VN " << i << " is: " << VONEHeuristic._start_frequency_result[i - 1] << endl;
				outs << endl;
			}
			else
				outs << "Sorted Virtual Network " << i << " will be blocked!\n\n";
		}
		outs << "------------------------------------------\n\n";

		//Output the frequency allocating in all substrate edges:
		outs << "----------Frequency Allocating Result----------\n\n";
		for (unsigned int i = 1; i <= VONEHeuristic._frequency_allocating_result.size(); i++)
		{
			outs << "The Frequency Allocating of the smaller source to bigger destination in No." << i << " edge is following: \n";
			for (unsigned int j = 1; j <= VONEHeuristic._frequency_allocating_result[i - 1].size(); j = j + 2)
			{
				for (unsigned int k = 1; k <= VONEHeuristic._frequency_allocating_result[i - 1][j - 1].size(); k++)
				{
					outs << VONEHeuristic._frequency_allocating_result[i - 1][j - 1][k - 1] << "\t";
				}
				outs << endl;
			}
			outs << endl;

			outs << "The Frequency Allocating of the bigger source to smaller destination in No." << i << " edge is following: \n";
			for (unsigned int j = 2; j <= VONEHeuristic._frequency_allocating_result[i - 1].size(); j = j + 2)
			{
				for (unsigned int k = 1; k <= VONEHeuristic._frequency_allocating_result[i - 1][j - 1].size(); k++)
				{
					outs << VONEHeuristic._frequency_allocating_result[i - 1][j - 1][k - 1] << "\t";
				}
				outs << endl;
			}
			outs << endl;
		}
		outs << "-----------------------------------------------\n\n";
		

		//Output the maximum frequency slot index:
		outs << "----------Maximum Frequency Slot Index----------\n\n";
		outs << "The Maximum Frequency Slot Index of the smaller source to bigger destination is following: \n";
		for (unsigned int j = 1; j <= VONEHeuristic._maximum_frequency_slot_index[0].size(); j = j + 2)
		{
			for (unsigned int i = 1; i <= VONEHeuristic._maximum_frequency_slot_index.size(); i++)
			{
				outs << VONEHeuristic._maximum_frequency_slot_index[i - 1][j - 1] << "\t";
			}
			outs << endl;
		}
		outs << endl;

		outs << "The Maximum Frequency Slot Index of the bigger source to smaller destination is following: \n";
		for (unsigned int j = 2; j <= VONEHeuristic._maximum_frequency_slot_index[0].size(); j = j + 2)
		{
			for (unsigned int i = 1; i <= VONEHeuristic._maximum_frequency_slot_index.size(); i++)
			{
				outs << VONEHeuristic._maximum_frequency_slot_index[i - 1][j - 1] << "\t";
			}
			outs << endl;
		}
		outs << "\n------------------------------------------------\n\n";

		//Output the fragmentation:
		outs << "----------Fragmetation----------\n\n";
		outs << "The Fragmetation of the smaller source to bigger destination is following: \n";
		for (unsigned int j = 1; j <= VONEHeuristic._fragmetation[0].size(); j = j + 2)
		{
			for (unsigned int i = 1; i <= VONEHeuristic._fragmetation.size(); i++)
			{
				outs << VONEHeuristic._fragmetation[i - 1][j - 1] << "\t";
			}
			outs << endl;
		}
		outs << endl;

		outs << "The Fragmetation of the bigger source to smaller destination is following: \n";
		for (unsigned int j = 2; j <= VONEHeuristic._fragmetation[0].size(); j = j + 2)
		{
			for (unsigned int i = 1; i <= VONEHeuristic._fragmetation.size(); i++)
			{
				outs << VONEHeuristic._fragmetation[i - 1][j - 1] << "\t";
			}
			outs << endl;
		}
		outs << "\n--------------------------------\n\n";

		//Output the crosstalk:
		outs << "----------Crosstalk----------\n\n";
		outs << "The Crosstalk is following: \n";
		for (unsigned int i = 1; i <= VONEHeuristic._crosstalk.size(); i++)
		{
			outs << VONEHeuristic._crosstalk[i - 1] << "\t";
		}
		outs << "\n\n-----------------------------\n\n";

		//Output the block number:
		outs << "----------Block Result----------\n\n";
		outs << "There are " << VONEHeuristic._block_num << " requests will be blocked!\n\n";
		outs << "--------------------------------\n\n";

		return outs;
	}

	VONEHeuristic::~VONEHeuristic()
	{

	}
}

namespace
{
	bool VNcompare(const VONETopo::Topo& VNA, const VONETopo::Topo& VNB)
	{
		VONETopo::NodeList VNAVertex, VNBVertex;
		VONETopo::LinkList VNAEdge, VNBEdge;
		VNA.get_vertex(VNAVertex);
		VNB.get_vertex(VNBVertex);
		VNA.get_edge(VNAEdge);
		VNB.get_edge(VNBEdge);

		if (VNAVertex.size() > VNBVertex.size())
			return true;
		else if ((VNAVertex.size() == VNBVertex.size()) &&
			(VNAEdge.size() > VNBEdge.size()))
			return true;
		else
			return false;
	}

	bool VNVcompare(const VONETopo::Vertex& VA, const VONETopo::Vertex& VB)
	{
		return (VA._vertex_computing_capacity > VB._vertex_computing_capacity);
	}

	bool SNUVcompare(const VONEHeuristic::Utilited_Vertex& UVA, const VONEHeuristic::Utilited_Vertex& UVB)
	{
		return (UVA._remaining_computing_resource > UVB._remaining_computing_resource);
	}

	int Request_Type(const int Bandwidth)
	{
		switch (Bandwidth)
		{
		case 3:
			return 1;
		case 4:
			return 2;
		case 5:
			return 3;
		default:
		{
			cerr << "Something wrong in Request Type!\n";
			exit(1);
		}
		}
	}

	int Core_Distance(const int Core1_index, const int Core2_index, const int C)
	{
		int distance = abs(Core1_index - Core2_index);

		return distance > C / 2 ? C - distance : distance;
	}

	int Nearest_Core_with_Same_Cost(const vector<int>& Core_Cost, const vector<int>::iterator& Initial_index, const int Last_Priority_Core, const int CoreNumber)
	{
		int priority_index = static_cast<int>(Initial_index - Core_Cost.begin() + 1);
		int distance = Core_Distance(priority_index, Last_Priority_Core, CoreNumber);
		for (vector<int>::const_iterator Citer = Core_Cost.begin(); Citer != Core_Cost.end(); Citer++)
		{
			int priority_index_tmp = static_cast<int>(Citer - Core_Cost.begin() + 1);
			if (*Citer == *Initial_index)
			{
				int distance_tmp = Core_Distance(priority_index_tmp, Last_Priority_Core, CoreNumber);
				if (distance_tmp < distance)
				{
					priority_index = priority_index_tmp;
					distance = distance_tmp;
				}
			}
		}
		return priority_index;
	}

	void Core_Priority_Define(vector<int>& Core_Priority, const int Hetergeneous_Core, const int CoreNumber, ofstream& outs)
	{
		vector<int> Core_Cost(CoreNumber);
		outs << "Core priority is: ";
		for (vector<int>::iterator iter = Core_Priority.begin(); iter != Core_Priority.end(); iter++)
		{
			int priority_index = 0;
			if (iter == Core_Priority.begin())
			{
				if (Hetergeneous_Core != 0)
					priority_index = Hetergeneous_Core; //For Hetergenous
				else
					priority_index = 1; //For Homogeneous
			}
			else
			{
				//For Hetergeneous:
				vector<int>::iterator zeroexist = find(Core_Cost.begin(), Core_Cost.end(), 0);
				if (zeroexist == Core_Cost.end())
				{
					vector<int>::iterator Min_Cost_index = min_element(Core_Cost.begin(), Core_Cost.end());
					priority_index = Nearest_Core_with_Same_Cost(Core_Cost, Min_Cost_index, *prev(iter), CoreNumber);
				}
				else
				{
					priority_index = Nearest_Core_with_Same_Cost(Core_Cost, zeroexist, *prev(iter), CoreNumber);
				}
			}

			*iter = priority_index;
			if (*iter == Hetergeneous_Core)
			{
				Core_Cost[(*iter + CoreNumber) % CoreNumber + 1 - 1]--;
				Core_Cost[(*iter + CoreNumber - 2) % CoreNumber + 1 - 1]--;
			}
			else
			{
				Core_Cost[(*iter + CoreNumber) % CoreNumber + 1 - 1]++;
				Core_Cost[(*iter + CoreNumber - 2) % CoreNumber + 1 - 1]++;
			}
			Core_Cost[*iter - 1] = INT_MAX / 2; //Notice the INT_MAX may cause overflow, here just choose a big value instead of infinity

			//Print the Core Priority:
			if (iter == prev(Core_Priority.end()))
				outs << *iter << endl;
			else
				outs << *iter << "->";
		}
	}

	void Core_Classification_Define(vector<int>& Core_Classification, const vector<int> Core_Priority, const vector<int> Service_Type, const int CoreNumber, ofstream& outs)
	{
		int Type_Num = static_cast<int>(Service_Type.size());
		int Sindex = 0;
		for (vector<int>::const_iterator iter = Core_Priority.begin(); iter != Core_Priority.end(); iter++)
		{
			int frontcore = (*iter + CoreNumber) % CoreNumber + 1;
			int backcore = (*iter + CoreNumber - 2) % CoreNumber + 1;

			//For Homogeneous:
			if (Type_Num == 3)
			{
				if (Core_Classification[frontcore - 1] != Service_Type[Sindex % Type_Num])
				{
					if (Core_Classification[backcore - 1] != Service_Type[Sindex % Type_Num])
					{
						Core_Classification[*iter - 1] = Service_Type[Sindex % Type_Num];
						Sindex++;
						continue;
					}
					else
					{
						Sindex++;
						if (Core_Classification[frontcore - 1] != Service_Type[Sindex % Type_Num])
						{
							Core_Classification[*iter - 1] = Service_Type[Sindex % Type_Num];
							Sindex++;
							continue;
						}
						else
						{
							Sindex++;
							Core_Classification[*iter - 1] = Service_Type[Sindex % Type_Num];
							Sindex++;
							continue;
						}
					}
				}
				else
				{
					Sindex++;
					if (Core_Classification[backcore - 1] != Service_Type[Sindex % Type_Num])
					{
						Core_Classification[*iter - 1] = Service_Type[Sindex % Type_Num];
						Sindex++;
						continue;
					}
					else
					{
						Sindex++;
						Core_Classification[*iter - 1] = Service_Type[Sindex % Type_Num];
						Sindex++;
						continue;
					}
				}
			}

			//For Hetergeneous:
			if (Type_Num == 2)
			{
				if (iter == Core_Priority.begin())
				{
					Core_Classification[*iter - 1] = 2;
					continue;
				}
				else
				{
					if (Core_Classification[frontcore - 1] != Service_Type[Sindex % Type_Num])
					{
						if (Core_Classification[backcore - 1] != Service_Type[Sindex % Type_Num])
						{
							Core_Classification[*iter - 1] = Service_Type[Sindex % Type_Num];
							Sindex++;
							continue;
						}
						else
						{
							Sindex++;
							if (Core_Classification[frontcore - 1] != Service_Type[Sindex % Type_Num])
							{
								Core_Classification[*iter - 1] = Service_Type[Sindex % Type_Num];
								Sindex++;
								continue;
							}
							else
							{
								Core_Classification[*iter - 1] = 2;
								Sindex++;
								continue;
							}
						}
					}
					else
					{
						Sindex++;
						if (Core_Classification[backcore - 1] != Service_Type[Sindex % Type_Num])
						{
							Core_Classification[*iter - 1] = Service_Type[Sindex % Type_Num];
							Sindex++;
							continue;
						}
						else
						{
							Core_Classification[*iter - 1] = 2;
							Sindex++;
							continue;
						}
					}
				}
			}
		}

		int service_type1 = 0, service_type3 = 0;
		for (vector<int>::iterator iter = Core_Classification.begin(); iter != Core_Classification.end(); iter++)
		{
			if (*iter == 1)
				service_type1++;

			if (*iter == 3)
				service_type3++;
		}
		if (service_type1 > service_type3)
		{
			for (vector<int>::iterator iter = Core_Classification.begin(); iter != Core_Classification.end(); iter++)
			{
				if (*iter == 1)
				{
					*iter = 3;
					continue;
				}

				if (*iter == 3)
				{
					*iter = 1;
					continue;
				}
			}
		}

		outs << "Core classification is: ";
		for (vector<int>::iterator iter = Core_Classification.begin(); iter != prev(Core_Classification.end()); iter++)
		{
			outs << *iter << "->";
		}
		outs << *prev(Core_Classification.end()) << endl;
	}

	void Vertexes_Embedding(VONETopo::NodeList& Virtual_Network_NodeList, vector<VONEHeuristic::Utilited_Vertex>& SN_Utilited_Vertex,
		vector<int>& vertex_embedding_result, ofstream& outs)
	{
		vertex_embedding_result.resize(Virtual_Network_NodeList.size());
		sort(Virtual_Network_NodeList.begin(), Virtual_Network_NodeList.end(), VNVcompare);
		sort(SN_Utilited_Vertex.begin(), SN_Utilited_Vertex.end(), SNUVcompare);
		vector<VONEHeuristic::Utilited_Vertex>::iterator iter = SN_Utilited_Vertex.begin();

		for (VONETopo::NodeListIT viter = Virtual_Network_NodeList.begin(); viter != Virtual_Network_NodeList.end(); viter++)
		{
			if (iter->_remaining_computing_resource >= viter->_vertex_computing_capacity)
			{
				vertex_embedding_result[viter->_vertex_id - 1] = (iter->_vertex)._vertex_id;
				iter->_remaining_computing_resource -= viter->_vertex_computing_capacity;
				iter++;
			}
			else
			{
				vertex_embedding_result.clear();
				outs << "Due to the Vertex computing capacity limit, current Virtual Network will be blocked!\n";
				break;
			}
		}
		outs << "Current virtual network vertexes embedding result is: \n";
		for (vector<int>::const_iterator iter = vertex_embedding_result.begin(); iter != vertex_embedding_result.end(); iter++)
		{
			outs << "No." << iter - vertex_embedding_result.begin() + 1 << " Node in VN was embedded in SN Node No." << *iter << endl;
		}
		outs << endl;
	}

	void get_SN_EdgeList(VONETopo::LinkList& SN_Edge, const vector<VONEHeuristic::Utilited_Edge>& SN_Utilited_Edge)
	{
		for (vector<VONEHeuristic::Utilited_Edge>::const_iterator iter = SN_Utilited_Edge.begin();
			iter != SN_Utilited_Edge.end(); iter++)
		{
			VONETopo::Edge SN_Edge_tmp;
			SN_Edge_tmp = iter->_edge;
			SN_Edge_tmp._edge_bandwidth = iter->_path_distance;
			SN_Edge.push_back(SN_Edge_tmp);
		}
	}

	void Edges_Embedding(const VONETopo::LinkList& Virtual_Network_EdgeList, const int Number_of_SN_Vertex,
		const vector<VONEHeuristic::Utilited_Edge>& SN_Utilited_Edge, const vector<int>& vertex_embedding_result, vector<vector<int> >& edge_embedding_result, ofstream& outs)
	{
		VONETopo::LinkList SN_Edge;
		get_SN_EdgeList(SN_Edge, SN_Utilited_Edge);

		for (VONETopo::LinkListIT liter = Virtual_Network_EdgeList.begin(); liter != Virtual_Network_EdgeList.end(); liter++)
		{
			//Find the shortest path:
			VONETopo::Graph SN_Resource(Number_of_SN_Vertex, SN_Edge);
			SN_Resource.Dijkstra(vertex_embedding_result[(liter->_edge_source)._vertex_id - 1],
				vertex_embedding_result[(liter->_edge_destination)._vertex_id - 1]);
			vector<int> Shortest_Path;
			SN_Resource.get_shortest_path(Shortest_Path);
			if (Shortest_Path.size() >= 2)
			{
				Shortest_Path.push_back(SN_Resource.get_distance());
				edge_embedding_result.push_back(Shortest_Path);
			}
			else
			{
				edge_embedding_result.clear();
				outs << "Due to the Edge frequency capacity limit, current Virtual Network will be blocked!\n";
				break;
			}
		}
		outs << "Current virtual network edges embedding result is: \n";
		for (vector<vector<int> >::const_iterator iter1 = edge_embedding_result.begin(); iter1 != edge_embedding_result.end(); iter1++)
		{
			outs << "No." << iter1 - edge_embedding_result.begin() + 1 << " Link in VN was embedded in SN Link ";
			for (vector<int>::const_iterator iter2 = iter1->begin(); iter2 != prev(iter1->end()); iter2++)
			{
				if (iter2 != prev(prev(iter1->end())))
					outs << *iter2 << "->";
				else
					outs << *iter2 << ", ";
			}
			outs << "with distance: " << *prev(iter1->end()) << endl;
		}
		outs << endl;
	}

	void Frequency_Recovery(const vector<vector<int> >& edge_embedding_result, vector<VONEHeuristic::Utilited_Edge>& SN_Utilited_Edge, const int CoreNumber)
	{
		VONETopo::LinkList SN_Edge;
		get_SN_EdgeList(SN_Edge, SN_Utilited_Edge);

		for (vector<vector<int> >::const_iterator liter = edge_embedding_result.begin(); liter != edge_embedding_result.end(); liter++)
		{
			for (vector<int>::const_iterator siter = liter->begin() + 1; siter != prev(liter->end()); siter++)
			{
				VONETopo::LinkListIT edge_index = find_if(SN_Edge.begin(), SN_Edge.end(),
					VONETopo::Edge_finder(*prev(siter), *siter));

				//Frequency resource recovery:
				for (int j = 1; j <= CoreNumber; j++)
				{
					for (int i = 1; i <= SN_Utilited_Edge[0]._frequency[0].size(); i++)
					{
						if (SN_Utilited_Edge[edge_index - SN_Edge.begin()]._frequency[2 * (j - 1)][i - 1] < 0)
							SN_Utilited_Edge[edge_index - SN_Edge.begin()]._frequency[2 * (j - 1)][i - 1] = 0;
						else if (SN_Utilited_Edge[edge_index - SN_Edge.begin()]._frequency[2 * j - 1][i - 1] < 0)
							SN_Utilited_Edge[edge_index - SN_Edge.begin()]._frequency[2 * j - 1][i - 1] = 0;
						else
							continue;
					}
				}
			}
		}
	}

	int Core_Index(const int node1_id, const int node2_id, const int current_core)
	{
		return (node1_id < node2_id) ? (2 * (current_core - 1)) : (2 * current_core - 1);
	}

	void Core_Assignment(const vector<vector<int> >& edge_embedding_result, const vector<int> Core_Priority, const int Hetergeneous_Core,
		vector<VONEHeuristic::Utilited_Edge>& SN_Utilited_Edge, vector<int>& Core_Classification, int VN_Bandwidth,
		vector<int>& core_assignment_result, int& start_frequency, const int CoreNumber, ofstream& outs)
	{
		VONETopo::LinkList SN_Edge;
		get_SN_EdgeList(SN_Edge, SN_Utilited_Edge);

		int Service_Type = Request_Type(VN_Bandwidth);
		if (Hetergeneous_Core != 0)
		{
			Core_Classification[Hetergeneous_Core - 1] = 2;
			for (vector<int>::iterator iter = Core_Classification.begin(); iter != Core_Classification.end(); iter++)
			{
				if (*iter == 0)
					*iter = -1;
			}			
			VN_Bandwidth = Service_Type == 2 ? VN_Bandwidth / 2 : VN_Bandwidth;
		}

		vector<vector<int> > Aux_Core_Classification(CoreNumber);
		for (vector<int>::iterator iter = Core_Classification.begin(); iter != Core_Classification.end(); iter++)
		{
			if (*iter == 0)
			{
				vector<int> tmp = {1, 2, 3};
				Aux_Core_Classification[iter - Core_Classification.begin()] = tmp;
			}
			else if	(*iter == -1)
			{
				vector<int> tmp = {1, 3};
				Aux_Core_Classification[iter - Core_Classification.begin()] = tmp;
			}
			else
			{
				vector<int> tmp = {*iter};
				Aux_Core_Classification[iter - Core_Classification.begin()] = tmp;
			}
		}

		vector<int> Start_Frequency(CoreNumber);
		vector<vector<int> > Core_Record;
		for (int i = 1; i <= CoreNumber; i++)
		{
			int core = Core_Priority[i - 1];
			if (find(Aux_Core_Classification[core - 1].begin(), Aux_Core_Classification[core - 1].end(), Service_Type) != Aux_Core_Classification[core - 1].end())
			{
				vector<int> Core_Record_tmp;
				for (int start = 1; start <= SN_Utilited_Edge[0]._frequency[0].size() - VN_Bandwidth + 1; start++)
				{
					//Clear pre-assignment:
					if (!Core_Record_tmp.empty())
					{
						Frequency_Recovery(edge_embedding_result, SN_Utilited_Edge, CoreNumber);
						Core_Record_tmp.clear();
					}
					//Get the first link:
					vector<vector<int> >::const_iterator eiter = edge_embedding_result.begin();
					vector<int>::const_iterator siter = eiter->begin() + 1;
					VONETopo::LinkListIT edge_index = find_if(SN_Edge.begin(), SN_Edge.end(),
						VONETopo::Edge_finder(*prev(siter), *siter));
					VONEHeuristic::Utilited_Edge SN_Utilited_Edge_tmp = SN_Utilited_Edge[edge_index - SN_Edge.begin()];

					//First Link frequency slots finding:
					int core_index = Core_Index(*prev(siter), *siter, core);
					if (SN_Utilited_Edge_tmp._frequency[core_index][start - 1] == 0)
					{
						bool first_start_flag = true;
						for (int start_tmp = start + 1; start_tmp <= start + VN_Bandwidth - 1; start_tmp++)
						{
							if (SN_Utilited_Edge_tmp._frequency[core_index][start_tmp - 1] == 0)
							{
								SN_Utilited_Edge[edge_index - SN_Edge.begin()]._frequency[core_index][start_tmp - 1] = -1;
								continue;
							}
							else
							{
								first_start_flag = false;
								break;
							}
						}
						if (first_start_flag == true)
						{
							Core_Record_tmp.push_back(core_index);
							bool following_link_flag = true;
							for (vector<vector<int> >::const_iterator eiter = edge_embedding_result.begin(); eiter != edge_embedding_result.end(); eiter++)
							{
								for (vector<int>::const_iterator siter = eiter->begin() + 1; siter != prev(eiter->end()); siter++)
								{
									if (eiter == edge_embedding_result.begin() && siter == eiter->begin() + 1)
									{
										continue;
									}
									else
									{
										edge_index = find_if(SN_Edge.begin(), SN_Edge.end(),
											VONETopo::Edge_finder(*prev(siter), *siter));
										SN_Utilited_Edge_tmp = SN_Utilited_Edge[edge_index - SN_Edge.begin()];

										bool core_flag = false;
										for (int j = 1; j <= CoreNumber; j++)
										{
											int core_tmp = Core_Priority[j - 1];
											if (find(Aux_Core_Classification[core_tmp - 1].begin(), Aux_Core_Classification[core_tmp - 1].end(), Service_Type) != Aux_Core_Classification[core_tmp - 1].end())
											{
												bool following_start_flag = true;
												core_index = Core_Index(*prev(siter), *siter, core_tmp);
												for (int start_tmp = start; start_tmp <= start + VN_Bandwidth - 1; start_tmp++)
												{
													if (SN_Utilited_Edge_tmp._frequency[core_index][start_tmp - 1] == 0)
													{
														SN_Utilited_Edge[edge_index - SN_Edge.begin()]._frequency[core_index][start_tmp - 1] = -1;
														continue;
													}
													else
													{
														following_start_flag = false;
														break;
													}
												}

												if (following_start_flag == true)
												{
													Core_Record_tmp.push_back(core_index);
													core_flag = true;
													break;
												}
												else
													continue;
											}
										}
										if (core_flag == true)
											continue;
										else
										{
											following_link_flag = false;
											break;
										}
									}
								}
								if (following_link_flag == false)
									break;
							}
							if (following_link_flag == false)
								continue;
							else
							{
								Start_Frequency[core - 1] = start;
								Core_Record.push_back(Core_Record_tmp);
								Frequency_Recovery(edge_embedding_result, SN_Utilited_Edge, CoreNumber);
								break;
							}
						}
						else
							continue;
					}
				}
			}
		}

		if (Core_Record.empty())
		{
			outs << "Due to the Core Assignment limit, current Virtual Network will be blocked!\n";
		}
		else
		{
			outs << "Core Record is: \n";
			for (int i = 1; i <= Core_Record.size(); i++)
			{
				for (int j = 1; j <= Core_Record[i - 1].size(); j++)
				{
					outs << Core_Record[i - 1][j - 1] << "\t";
				}
				outs << endl;
			}
			outs << endl;

			outs << "Start Frequency is: ";
			for (int i = 1; i <= CoreNumber; i++)
			{
				if (i != CoreNumber)
					outs << Start_Frequency[i - 1] << ", ";
				else
					outs << Start_Frequency[i - 1] << endl;
			}
			outs << endl;

			int core_assignment_index = 1;
			int min_Start = Start_Frequency[Core_Record[0][0] / 2 + 1 - 1];
			for (vector<vector<int> >::const_iterator Riter = Core_Record.begin(); Riter != Core_Record.end(); Riter++)
			{
				if (Start_Frequency[(*Riter)[0] / 2 + 1 - 1] < min_Start)
				{
					min_Start = Start_Frequency[(*Riter)[0] / 2 + 1 - 1];
					core_assignment_index = static_cast<int>(Riter - Core_Record.begin() + 1);
				}
			}
			start_frequency = Start_Frequency[Core_Record[core_assignment_index - 1][0] / 2 + 1 - 1];
			core_assignment_result = Core_Record[core_assignment_index - 1];
		}

		outs << "Core Priority is: ";
		for (int i = 1; i <= CoreNumber; i++)
		{
			if (i != CoreNumber)
				outs << Core_Priority[i - 1] << ", ";
			else
				outs << Core_Priority[i - 1] << endl;
		}
		outs << endl;

		outs << "Core Classification is: ";
		for (int i = 1; i <= CoreNumber; i++)
		{
			if (i != CoreNumber)
				outs << Core_Classification[i - 1] << ", ";
			else
				outs << Core_Classification[i - 1] << endl;
		}
		outs << endl;

		outs << "Current Virtual Network Cores Assignment Result is: ";
		for (vector<int>::const_iterator iter = core_assignment_result.begin(); iter != core_assignment_result.end(); iter++)
		{
			if (iter != prev(core_assignment_result.end()))
				outs << "Core " << (*iter) / 2 + 1 << "--";
			else
				outs << "Core " << (*iter) / 2 + 1 << ", ";
		}
		outs << "with start frequency: " << start_frequency << endl;
		outs << endl;
	}

	void Frequency_Allocating(const vector<vector<int> >& edge_embedding_result, const vector<int> core_assignment_result, const int start_frequency,
		const int Hetergeneous_Core, vector<VONEHeuristic::Utilited_Edge>& SN_Utilited_Edge, int VN_Bandwidth)
	{
		VONETopo::LinkList SN_Edge;
		get_SN_EdgeList(SN_Edge, SN_Utilited_Edge);

		int Service_Type = Request_Type(VN_Bandwidth);
		if (Hetergeneous_Core != 0)
		{
			VN_Bandwidth = Service_Type == 2 ? VN_Bandwidth / 2 : VN_Bandwidth;
		}

		int index = 1;
		for (vector<vector<int> >::const_iterator liter = edge_embedding_result.begin(); liter != edge_embedding_result.end(); liter++)
		{
			for (vector<int>::const_iterator siter = liter->begin() + 1; siter != prev(liter->end()); siter++)
			{
				VONETopo::LinkListIT edge_index = find_if(SN_Edge.begin(), SN_Edge.end(),
					VONETopo::Edge_finder(*prev(siter), *siter));
				//Frequency resource allocated:

				for (int j = 1; j <= VN_Bandwidth; j++)
				{
					SN_Utilited_Edge[edge_index - SN_Edge.begin()]._frequency[core_assignment_result[index - 1]][start_frequency - 1 + j - 1] = Service_Type;
				}

				for (int i = static_cast<int>(SN_Utilited_Edge[edge_index - SN_Edge.begin()]._frequency[core_assignment_result[index - 1]].size()); i >= 1; i--)
				{
					if (SN_Utilited_Edge[edge_index - SN_Edge.begin()]._frequency[core_assignment_result[index - 1]][i - 1] != 0)
					{
						SN_Utilited_Edge[edge_index - SN_Edge.begin()]._end_slot[core_assignment_result[index - 1] / 2 + 1 - 1] = i;
						break;
					}
				}
				vector<int>::iterator max_iter = max_element(SN_Utilited_Edge[edge_index - SN_Edge.begin()]._end_slot.begin(),
					SN_Utilited_Edge[edge_index - SN_Edge.begin()]._end_slot.end());
				SN_Utilited_Edge[edge_index - SN_Edge.begin()]._path_distance = *max_iter;
				index++;
			}
		}
	}
}