//This file defines class topology and graph.
//------------------------------------------------
//File Name: Topo.cpp
//Author: Qihan Zhang
//Email: lengkudaodi@outlook.com
//Last Modified: Aug. 3rd 2021
//------------------------------------------------

#include "Topo.h"

#include <iomanip>
#include <algorithm>

namespace
{
	bool less_edge_id(const VONETopo::Edge& FirstEdge, const VONETopo::Edge& SecondEdge);
}

namespace VONETopo
{
	Vertex_finder::Vertex_finder(const int Vertex_ID) :_vertex_id(Vertex_ID)
	{

	}

	bool Vertex_finder::operator ()(const vector<Vertex>::value_type& Vertex_Struct)
	{
		return Vertex_Struct._vertex_id == _vertex_id;
	}

	Edge_finder::Edge_finder(const int Source, const int Destination) :_edge_source(Source), _edge_destination(Destination)
	{

	}

	bool Edge_finder::operator ()(const LinkList::value_type& Edge_Struct)
	{
		if (_edge_source < _edge_destination)
			return ((Edge_Struct._edge_source._vertex_id == _edge_source)
				&& (Edge_Struct._edge_destination._vertex_id == _edge_destination));
		else
			return ((Edge_Struct._edge_source._vertex_id == _edge_destination)
				&& (Edge_Struct._edge_destination._vertex_id == _edge_source));
	}

	Topo::Topo() :_vertex_num(0), _edge_num(0)
	{

	}

	void Topo::AddVertex(const int VertexID, const int Computing_Capacity)
	{
		Vertex vertex_tmp = { VertexID, Computing_Capacity };
		_vertex.push_back(vertex_tmp);
		_vertex_num = static_cast<int>(_vertex.size());
	}

	void Topo::AddEdge(const int Source, const int Destination, const int Bandwidth)
	{
		NodeListIT Niter_Source, Niter_Destination;
		if (Source != Destination)
		{
			NodeListIT Niter = find_if(_vertex.begin(), _vertex.end(), Vertex_finder(Source));
			if (Niter != _vertex.end())
			{
				Niter_Source = Niter;
				Niter = find_if(_vertex.begin(), _vertex.end(), Vertex_finder(Destination));
				if (Niter != _vertex.end())
				{
					Niter_Destination = Niter;
					LinkListIT Giter = find_if(_edge.begin(), _edge.end(), Edge_finder(Source, Destination));
					if (Giter == _edge.end())
					{
						Edge edge;
						edge._edge_id = _edge_num + 1;
						edge._edge_bandwidth = Bandwidth;

						if (Niter_Source->_vertex_id < Niter_Destination->_vertex_id)
						{
							edge._edge_source = *Niter_Source;
							edge._edge_destination = *Niter_Destination;
						}
						else
						{
							edge._edge_source = *Niter_Destination;
							edge._edge_destination = *Niter_Source;
						}

						_edge.push_back(edge);
					}
					else
					{
						cout << "The Edge from " << Source << " to " << Destination << " has been already existed!\n"
							<< "Please do not add the same Edge!\n";
						exit(1);
					}
				}
				else
				{
					cout << "The Vertex " << Destination << " are not defined!\n"
						<< "Please add it to the Topology firstly.\n";
					exit(1);
				}
			}
			else
			{
				cout << "The Vertex " << Source << " are not defined!\n"
					<< "Please add it to the Topology firstly.\n";
				exit(1);
			}
		}
		else
		{
			cout << "The Source " << Source << " and Destination " << Destination << " should be different!\n";
			exit(1);
		}

		_edge_num = static_cast<int>(_edge.size());
	}

	istream& operator >>(istream& ins, Topo& Topology)
	{
		int Link_ID, Start, End, Start_Capacity, End_Capacity, Bandwidth;
		while (ins >> Link_ID >> Start >> End >> Start_Capacity >> End_Capacity >> Bandwidth)
		{
			if (Start > End)
			{
				swap(Start, End);
				swap(Start_Capacity, End_Capacity);
			}

			NodeListIT Niter = find_if(Topology._vertex.begin(), Topology._vertex.end(), Vertex_finder(Start));
			if (Niter == Topology._vertex.end())
			{
				Topology.AddVertex(Start, Start_Capacity);
			}
			Niter = find_if(Topology._vertex.begin(), Topology._vertex.end(), Vertex_finder(End));
			if (Niter == Topology._vertex.end())
			{
				Topology.AddVertex(End, End_Capacity);
			}
			Topology.AddEdge(Start, End, Bandwidth);
			(Topology._edge.end() - 1)->_edge_id = Link_ID;
		}
		sort(Topology._edge.begin(), Topology._edge.end(), less_edge_id);

		return ins;
	}

	ostream& operator <<(ostream& outs, const Topo& Topology)
	{
		outs << left << setw(10) << "Link ID" << setw(10) << "Start" << setw(10) << "End" << setw(15)
			<< "Start Capacity" << setw(15) << "End Capacity" << setw(15) << "Bandwidth" << endl;

		for (LinkListIT iter = Topology._edge.begin(); iter != Topology._edge.end(); iter++)
		{
			outs << left << setw(10) << iter->_edge_id << setw(10) << iter->_edge_source._vertex_id << setw(10)
				<< iter->_edge_destination._vertex_id << setw(15) << iter->_edge_source._vertex_computing_capacity
				<< setw(15) << iter->_edge_destination._vertex_computing_capacity << setw(15) << iter->_edge_bandwidth << endl;
		}

		outs << "There are " << Topology._vertex_num << " Vertexes and " << Topology._edge_num << " Edges in this topology!\n";

		return outs;
	}

	void Topo::get_vertex(NodeList& VertexesSet) const
	{
		VertexesSet = _vertex;
	}

	void Topo::get_edge(LinkList& EdgesSet) const
	{
		EdgesSet = _edge;
	}

	Topo::~Topo()
	{

	}

	Graph::Graph() : _vertex_num(0)
	{

	}

	Graph::Graph(const int Vertex_Num, const LinkList& EdgesSet)
	{
		_vertex_num = Vertex_Num;
		for (int i = 1; i <= _vertex_num; i++)
		{
			vector<int> row_tmp(_vertex_num, INT_MAX);
			for (LinkListIT iter = EdgesSet.begin(); iter != EdgesSet.end(); iter++)
			{
				if ((iter->_edge_source)._vertex_id == i)
				{
					row_tmp[(iter->_edge_destination)._vertex_id - 1] = iter->_edge_bandwidth;
				}
				else if ((iter->_edge_destination)._vertex_id == i)
				{
					row_tmp[(iter->_edge_source)._vertex_id - 1] = iter->_edge_bandwidth;
				}
			}
			row_tmp[i - 1] = 0;
			_adjacency_matrix.push_back(row_tmp);
		}
	}

	void Graph::Dijkstra(const int Source_ID, const int Destination_ID)
	{
		vector <bool> s;
		vector <int> distance;
		vector <int> shortest_path_set;
		for (int i = 1; i <= _vertex_num; i++)
		{
			distance.push_back(_adjacency_matrix[Source_ID - 1][i - 1]);
			s.push_back(false);
			if (i != Source_ID && distance[i - 1] < INT_MAX)
				shortest_path_set.push_back(Source_ID);
			else
				shortest_path_set.push_back(-1);
		}

		s[Source_ID - 1] = true;
		distance[Source_ID - 1] = 0;
		for (int i = 1; i <= _vertex_num; i++)
		{
			int min = INT_MAX, u = Source_ID;
			for (int j = 1; j <= _vertex_num; j++)
			{
				if (!s[j - 1] && distance[j - 1] < min)
				{
					u = j;
					min = distance[j - 1];
				}
			}
			s[u - 1] = true;
			for (int k = 1; k <= _vertex_num; k++)
			{
				if (!s[k - 1] && _adjacency_matrix[u - 1][k - 1] < INT_MAX
					&& distance[u - 1] + _adjacency_matrix[u - 1][k - 1] < distance[k - 1])
				{
					distance[k - 1] = distance[u - 1] + _adjacency_matrix[u - 1][k - 1];
					shortest_path_set[k - 1] = u;
				}
			}
		}

		_shortest_path.clear();
		_distance = distance[Destination_ID - 1];
		_shortest_path.push_back(Destination_ID);
		for (int i = 1; shortest_path_set[_shortest_path[i - 1] - 1] != -1; i++)
			_shortest_path.push_back(shortest_path_set[_shortest_path[i - 1] - 1]);
		reverse(_shortest_path.begin(), _shortest_path.end());
	}

	void Graph::get_shortest_path(vector<int>& Shortest_Path) const
	{
		Shortest_Path = _shortest_path;
	}

	int Graph::get_distance() const
	{
		return _distance;
	}

	Graph::~Graph()
	{

	}
}

namespace
{
	bool less_edge_id(const VONETopo::Edge& FirstEdge, const VONETopo::Edge& SecondEdge)
	{
		return FirstEdge._edge_id < SecondEdge._edge_id;
	}
}