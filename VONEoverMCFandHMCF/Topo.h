//This file declares class topology and graph.
//------------------------------------------------
//File Name: Topo.h
//Author: Qihan Zhang
//Email: lengkudaodi@outlook.com
//Last Modified: Aug. 3rd 2021
//------------------------------------------------

#pragma once
#include <iostream>
#include <vector>

using namespace std;

namespace VONETopo
{
	//The set of Vertexes, each vertex includes two properties: ID and Computing Capacity
	struct Vertex
	{
		int _vertex_id;
		int _vertex_computing_capacity;
	};

	typedef vector<Vertex> NodeList;
	typedef NodeList::const_iterator NodeListIT;

	//An auxiliary class to be used in the function FIND_IF in STL
	class Vertex_finder
	{
	public:
		Vertex_finder(const int Vertex_ID);

		bool operator ()(const vector<Vertex>::value_type& Vertex_Struct);
		//Precondition: The struct Vertex_Struct has been initialized
		//Postcondition: Return the comparison results between _vertex_id and that in the struct Vertex_Struct

	private:
		int _vertex_id;
	};

	//The set of Edges, each edges includes four properties: ID, Source, Destination and Bandwidth(Frequency Slot)
	struct Edge
	{
		int _edge_id;
		Vertex _edge_source;
		Vertex _edge_destination;
		int _edge_bandwidth;
	};

	typedef vector<Edge> LinkList;
	typedef LinkList::const_iterator LinkListIT;

	//Another auxiliary class to be used in the function FIND_IF in STL
	class Edge_finder
	{
	public:
		Edge_finder(const int Source, const int Destination);

		bool operator ()(const LinkList::value_type& Edge_Struct);
		//Precondition: The struct Edge_Struct has been initialized
		//Postcondition: Return the comparison results between _edge_source and _edge_destination 
		//    and those in the struct Edge_Struct

	private:
		int _edge_source;
		int _edge_destination;
	};

	//Create the Topology of substrate network or virtual network
	class Topo
	{
	public:
		Topo();

		void AddVertex(const int VertexID, const int Computing_Capacity);
		//Postcondition: Put the Vertex with the properties of VertexID and Computing _Capacity in _vertex 

		void AddEdge(const int Source, const int Destination, const int Bandwidth = 320);	//50 for test, 320 for real network
		//Postcondition: Put the Edge with the properties of the addition order, Source, Destination and Bandwidth in _edge

		friend istream& operator >>(istream& ins, Topo& Topology);
		//Precondition: ins should be connected to the screen or a file
		//Postcondition: the topology has been put in _edge

		friend ostream& operator <<(ostream& outs, const Topo& Topology);
		//Precondition: outs should be connected to the screen or a file
		//Postcondition: the topology will be showed

		void get_vertex(NodeList& VertexesSet) const;
		//Precondition: the topology has been assigned
		//Postcondition: return the topology _vertex

		void get_edge(LinkList& EdgesSet) const;
		//Precondition: the topology has been assigned
		//Postcondition: return the topology _edge

		~Topo();

	private:
		NodeList _vertex;
		int _vertex_num;
		int _edge_num;
		LinkList _edge;
	};

	//Create weighted directed Graph of the Topology resource
	class Graph
	{
	public:
		Graph();

		Graph(const int Vertex_Num, const LinkList& EdgesSet);
		//Precondition: the topology has been assigned
		//Postcondition: the adjacent matrix of the topology has been built and put in _adjacency_matrix

		void Dijkstra(const int Source_ID, const int Destination_ID);
		//Precondition: the _adjacency_matrix has been assigned
		//Postcondition: the shortest path from Source_ID to Destination_ID has been found and put in _shortest_path,
		//    its distance has been put in _distance, too

		void get_shortest_path(vector<int>& Shortest_Path) const;
		//Precondition: the function Dijkstra has been executed, and _shortest_path has been assigned
		//Postcondition: return the shortest path _shortest_path

		int get_distance() const;
		//Precondition: the function Dijkstra has been executed, and _distance has been assigned
		//Postcondition: return the distance of the shortest path _distance

		~Graph();

	private:
		int _vertex_num;
		vector<vector<int> > _adjacency_matrix;
		int _distance;
		vector<int> _shortest_path;
	};
}