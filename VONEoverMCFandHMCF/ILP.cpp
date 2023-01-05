//This file defines class VONECplex.
//------------------------------------------------
//File Name: ILP.cpp
//Author: Qihan Zhang
//Email: lengkudaodi@outlook.com
//Last Modified: Aug. 3rd 2021
//------------------------------------------------

#include "ILP.h"
#include "Parameters.h"

#include <numeric>
#include <string>

namespace
{
	void getILPVertexInputParameter
	(const VONETopo::Topo& Substrate_Network,
		const vector<VONETopo::Topo>& Virtual_Network_List,
		int& Request_Number,
		int& Substrate_Network_Vertex_Number,
		vector<int>& Virtual_Networks_Vertex_Number,
		vector<int>& Accumulate_Virtual_Networks_Vertex_Number,
		vector<int>& Computing_Capacity_in_Substrate_Vertexes,
		vector<vector<int> >& Computing_Capacity_in_Virtual_Vertexes);
	//Precondition: The topology Substrate_Network and topology vector Virtual_Network_List have been assigned, and other
	//    parameters have been defined.
	//Postcondition: The information from the topologys will be assigned to Request_Number, Substrate_Network_Vertex_Number, 
	//    Virtual_Networks_Vertex_Number, Computing_Capacity_in_Substrate_Vertexes and Computing_Capacity_in_Virtual_Vertexes.

	void constraints1
	(IloBoolVarArray x,
		IloRangeArray c,
		const int Request_Number,
		const int Substrate_Network_Vertex_Number,
		const vector<int> Virtual_Networks_Vertex_Number,
		const vector<int> Accumulate_Virtual_Networks_Vertex_Number);
	//Precondition: The function getILPVertexInputParameter has been excuted, so Request_Number, Substrate_Network_Vertex_Number,
	//    Virtual_Networks_Vertex_Number and Computing_Capacity_in_Substrate_Vertexes were assigned
	//Postcondition: The constraint of that each vertex of every request must be embedded in only one vertex of the substrate has
	//    been added into constraint c. And new variables of x will be added.

	void constraints2
	(IloBoolVarArray x,
		IloRangeArray c,
		const int Request_Number,
		const int Substrate_Network_Vertex_Number,
		const vector<int> Virtual_Networks_Vertex_Number,
		const vector<int> Accumulate_Virtual_Networks_Vertex_Number);
	//Precondition: The function getILPVertexInputParameter has been excuted, so Request_Number, Substrate_Network_Vertex_Number,
	//    Virtual_Networks_Vertex_Number and Computing_Capacity_in_Substrate_Vertexes were assigned. And x must be added into the 
	//    array.
	//Postcondition: The constraint of that each substrate vertex can accept at most one vertex in each request has been added 
	//    into constraint c.

	void constraints3
	(IloBoolVarArray x,
		IloRangeArray c,
		const int Request_Number,
		const int Substrate_Network_Vertex_Number,
		const vector<int> Virtual_Networks_Vertex_Number,
		const vector<int> Accumulate_Virtual_Networks_Vertex_Number,
		const vector<int> Computing_Capacity_in_Substrate_Vertexes,
		const vector<vector<int> > Computing_Capacity_in_Virtual_Vertexes);
	//Precondition: The function getILPVertexInputParameter has been excuted, so Request_Number, Substrate_Network_Vertex_Number,
	//    Virtual_Networks_Vertex_Number and Computing_Capacity_in_Substrate_Vertexes were assigned. And x must be added into the 
	//    array.
	//Postcondition: The constraint of that the occupied computing resource of all request in one substrate vertex should not 
	//    overflow its capacity has been added into constraint c.

	void getILPEdgeInputParameter
	(const VONETopo::Topo& Substrate_Network,
		const vector<VONETopo::Topo>& Virtual_Network_List,
		VONETopo::LinkList& Substrate_Network_Edge,
		int& Substrate_Network_Edge_Number,
		vector<VONETopo::LinkList>& Virtual_Networks_Edge,
		vector<int>& Virtual_Networks_Edge_Number,
		vector<int>& Accumulate_Virtual_Networks_Edge_Number,
		vector<int>& Virtual_Networks_Bandwidth);
	//Precondition: The topology Substrate_Network and topology vector Virtual_Network_List have been assigned, and other
	//    parameters have been defined.
	//Postcondition: The information from the topologys will be assigned to Substrate_Network_Edge, Substrate_Network_Edge_Number, 
	//    Virtual_Networks_Edge, Virtual_Networks_Edge_Number and Virtual_Networks_Bandwidth.

	void constraints4
	(IloBoolVarArray x,
		IloRangeArray c,
		const int Request_Number,
		const int Substrate_Network_Edge_Number,
		const vector<int> Virtual_Networks_Edge_Number,
		const vector<int> Accumulate_Virtual_Networks_Edge_Number,
		const VONETopo::LinkList& Substrate_Network_Edge);
	//Precondition: The function getILPVertexInputParameter and getILPEdgeInputParameter have been excuted, so Request_Number, 
	//    Substrate_Network_Edge_Number and Virtual_Networks_Edge_Number were assigned. 
	//Postcondition: The constraint of that the link from s to d and d to s should be disjoint has been added to the constraint c.
	//    And new variables of x will be added.

	void constraints5
	(IloBoolVarArray x,
		IloRangeArray c,
		const IloBoolVarArray y,
		const int Request_Number,
		const int Substrate_Network_Edge_Number,
		const vector<int> Virtual_Networks_Edge_Number,
		const vector<int> Accumulate_Virtual_Networks_Edge_Number,
		const int Substrate_Network_Vertex_Number,
		const vector<int> Accumulate_Virtual_Networks_Vertex_Number,
		const VONETopo::LinkList& Substrate_Network_Edge,
		const vector<VONETopo::LinkList>& Virtual_Networks_Edge);
	//Precondition: The function getILPVertexInputParameter and getILPEdgeInputParameter have been excuted, so Request_Number, 
	//    Substrate_Network_Edge_Number, Virtual_Networks_Edge_Number, Substrate_Network_Vertex_Number, 
	//    Virtual_Networks_Vertex_Number, Substrate_Network_Edge, Virtual_Networks_Edge, were assigned. 
	//    And x must be added into the array.
	//Postcondition: The constraint of that the flow conservation of each request when it is embedded into the substrate has been 
	//    added to the constraint c.

	void constraints6
	(IloBoolVarArray x,
		IloRangeArray con,
		const IloBoolVarArray y,
		const int Request_Number,
		const int Substrate_Network_Edge_Number,
		const vector<int> Virtual_Networks_Edge_Number,
		const vector<int> Accumulate_Virtual_Networks_Edge_Number,
		const VONETopo::LinkList& Substrate_Network_Edge,
		const int C);
	//Precondition: The function getILPVertexInputParameter and getILPEdgeInputParameter have been excuted, so Request_Number, 
	//    Substrate_Network_Edge_Number, Virtual_Networks_Edge_Number, Accumulate_Virtual_Networks_Edge_Number  
	//    and Substrate_Network_Edge were assigned. And y must be added into the array. C has been initialized.
	//Postcondition: The constraint of that only one core can be selected when the link is determined has been added to the 
	//    constraint con. And new variables of x will be added. 

	void constraints6_heter
	(IloBoolVarArray x,
		IloRangeArray con,
		const IloBoolVarArray y,
		const int Request_Number,
		const int Substrate_Network_Edge_Number,
		const vector<int> Virtual_Networks_Edge_Number,
		const vector<int> Accumulate_Virtual_Networks_Edge_Number,
		const VONETopo::LinkList& Substrate_Network_Edge,
		const vector<int> Virtual_Networks_Bandwidth,
		const int C,
		const int C_Heter);
	//Precondition: The function getILPVertexInputParameter and getILPEdgeInputParameter have been excuted, so Request_Number, 
	//    Substrate_Network_Edge_Number, Virtual_Networks_Edge_Number,  Accumulate_Virtual_Networks_Edge_Number, 
	//    Substrate_Network_Edge and Virtual_Networks_Bandwidth were assigned. And y must be added into the array. 
	//    C and C_Heter have been initialized.
	//Postcondition: When the service type of current request meets the hetergeneous core transport condition, only the hetergeneous 
	//    core can be selected. For other homogeneous cores, only one core can be selected when the link is determined. The constraint
	//    describing before has been added to the constraint con. And new variables of x will be added. 

	void constraints7
	(IloIntVarArray x,
		IloRangeArray con,
		const IloIntVarArray y,
		const int Request_Number,
		const vector<int> Virtual_Networks_Bandwidth,
		const int Capacity);
	//Precondition: The function getILPVertexInputParameter and getILPEdgeInputParameter have been excuted, so Request_Number,
	//    and Virtual_Networks_Bandwidth were assigned. Capacity is the max bandwidth resource of the substrate network.
	//    And y must be added into the array. 
	//Postcondition: The constraint of spectrum continuity in optical network has been added to the constraint con. And new variable
	//    x will be addd.

	void constraints7_heter
	(IloIntVarArray x,
		IloRangeArray con,
		const IloIntVarArray y,
		const int Request_Number,
		const vector<int> Virtual_Networks_Bandwidth,
		const int Capacity);
	//Precondition: The function getILPVertexInputParameter and getILPEdgeInputParameter have been excuted, so Request_Number,
	//    and Virtual_Networks_Bandwidth were assigned. Capacity is the max bandwidth resource of the substrate network.
	//    And y must be added into the array. 
	//Postcondition: The constraint of spectrum continuity in hetergeneous cores fo the optical network has been added to the constraint con. 
	//    And new variable x will be addd.

	void constraints8
	(IloModel mod,
		const IloIntVarArray x,
		const IloIntVarArray y,
		const int Request_Number,
		const int Substrate_Network_Edge_Number,
		const vector<int> Virtual_Networks_Bandwidth,
		const int C,
		const bool Cross_Talk_Flag = false);
	//Precondition: The function getILPVertexInputParameter and getILPEdgeInputParameter have been excuted, so Request_Number,
	//    Substrate_Network_Edge_Number and Virtual_Networks_Bandwidth were assigned. And x and y must be added into the array.
	//    C has been initialized. Cross_Talk_Flag indicates that whether the cross-talk between two adjacent cores should be 
	//    taken into account or not.
	//Postcondition: The constraint of spectrum consistency in optical network has been added to the model mod.

	void constraints8_heter
	(IloModel mod,
		const IloIntVarArray x,
		const IloIntVarArray y,
		const int Request_Number,
		const int Substrate_Network_Edge_Number,
		const vector<int> Virtual_Networks_Bandwidth,
		const int C,
		const int C_Heter,
		const bool Cross_Talk_Flag = false);
	//Precondition: The function getILPVertexInputParameter and getILPEdgeInputParameter have been excuted, so Request_Number,
	//    Substrate_Network_Edge_Number and Virtual_Networks_Bandwidth were assigned. And x and y must be added into the array. 
	//    C and C_Heter have been initialized. Cross_Talk_Flag indicates that whether the cross-talk between two adjacent cores 
	//    should be taken into account or not.
	//Postcondition: The constraint of spectrum consistency in hetergeneous cores of optical network has been added to the model mod.
}
namespace VONEILP
{
	VONECplex::VONECplex()
	{
		_model = IloModel(_env);
		_variables = IloIntVarArray(_env);
		_constraints = IloRangeArray(_env);
		_cplex = IloCplex(_env);
	}

	void VONECplex::solve(const VONETopo::Topo& Substrate_Network,
		const vector<VONETopo::Topo>& Virtual_Network_List,
		int CoreNumber, int HeterogeneousCoreIndex, string CrosstalkYesorNo)
	{
		int RNum;
		int SVNum;
		vector<int> VVNum;
		vector<int> AVVNum;
		vector<int> C_s;
		vector<vector<int> > C_r_v;
		getILPVertexInputParameter(Substrate_Network, Virtual_Network_List, RNum, SVNum, VVNum, AVVNum, C_s, C_r_v);

		VONETopo::LinkList SGraph;
		int SENum;
		vector<VONETopo::LinkList> VsGraph;
		vector<int> VENum;
		vector<int> AVENum;
		vector<int> VEBw;
		getILPEdgeInputParameter(Substrate_Network, Virtual_Network_List, SGraph, SENum, VsGraph, VENum, AVENum, VEBw);

		try
		{
			IloBoolVarArray Node_r_v_s(_env);
			constraints1(Node_r_v_s, _constraints, RNum, SVNum, VVNum, AVVNum);
			_variables.add(Node_r_v_s);
			constraints2(Node_r_v_s, _constraints, RNum, SVNum, VVNum, AVVNum);
			constraints3(Node_r_v_s, _constraints, RNum, SVNum, VVNum, AVVNum, C_s, C_r_v);

			IloIntVarArray Link_r_e_s_d(_env);
			constraints4(Link_r_e_s_d, _constraints, RNum, SENum, VENum, AVENum, SGraph);
			_variables.add(Link_r_e_s_d);
			constraints5(Link_r_e_s_d, _constraints, Node_r_v_s, RNum, SENum, VENum, AVENum, SVNum, AVVNum, SGraph, VsGraph);

			IloBoolVarArray Core_r_c_s_d(_env);
			if (HeterogeneousCoreIndex == 0)
			{
				constraints6(Core_r_c_s_d, _constraints, Link_r_e_s_d, RNum, SENum, VENum, AVENum, SGraph, CoreNumber);
			}
			else
			{
				constraints6_heter(Core_r_c_s_d, _constraints, Link_r_e_s_d, RNum, SENum, VENum, AVENum, SGraph, VEBw, CoreNumber, HeterogeneousCoreIndex);
			}
			_variables.add(Core_r_c_s_d);

			IloIntVarArray f_r(_env);
			string Variable = "f_";
			for (int r = 1; r <= RNum; r++)
			{
				string Variable_Name = Variable + to_string(r);
				f_r.add(IloIntVar(_constraints.getEnv(), 1, IloIntMax, Variable_Name.c_str()));
			}
			_variables.add(f_r);

			IloIntVarArray Ms_r(_env);
			if (HeterogeneousCoreIndex == 0)
			{
				constraints7(Ms_r, _constraints, f_r, RNum, VEBw, SGraph[0]._edge_bandwidth);
			}
			else
			{
				constraints7_heter(Ms_r, _constraints, f_r, RNum, VEBw, SGraph[0]._edge_bandwidth);
			}
			_variables.add(Ms_r);

			_model.add(_variables);
			_model.add(_constraints);

			string ModelLPFileName = "";
			if (HeterogeneousCoreIndex == 0)
			{
				if ((CrosstalkYesorNo == "Yes") || (CrosstalkYesorNo == "yes"))
				{
					constraints8(_model, Core_r_c_s_d, f_r, RNum, SENum, VEBw, CoreNumber, false);
					ModelLPFileName = "VONEonMCFwithCrosstalk";
				}					
				else
				{
					constraints8(_model, Core_r_c_s_d, f_r, RNum, SENum, VEBw, CoreNumber, true);
					ModelLPFileName = "VONEonMCFwithoutCrosstalk";
				}		
			}
			else
			{
				if ((CrosstalkYesorNo == "Yes") || (CrosstalkYesorNo == "yes"))
				{
					constraints8_heter(_model, Core_r_c_s_d, f_r, RNum, SENum, VEBw, CoreNumber, HeterogeneousCoreIndex, false);
					ModelLPFileName = "VONEonHMCFwithCrosstalk";
				}					
				else
				{
					constraints8_heter(_model, Core_r_c_s_d, f_r, RNum, SENum, VEBw, CoreNumber, HeterogeneousCoreIndex, true);
					ModelLPFileName = "VONEonHMCFwithoutCrosstalk";
				}					
			}
			_model.add(IloMinimize(_env, IloMax(Ms_r)));
			_cplex.extract(_model);
			
			ModelLPFileName = ModelLPFileName + to_string(RNum) + ".lp";
			_cplex.exportModel(ModelLPFileName.c_str());

			if (!_cplex.solve())
			{
				_env.error() << "Failed to optimize the VONE ILP.\n";
				throw(-1);
			}
		}
		catch (IloException& e)
		{
			cerr << "Concert exception caught: " << e << endl;
		}
		catch (...)
		{
			cerr << "Unknown exception caught!\n";
		}
	}

	ostream& operator <<(ostream& outs, VONECplex& VONEILP)
	{
		IloNumArray _values(VONEILP._env);
		VONEILP._cplex.getValues(_values, VONEILP._variables);
		VONEILP._env.setOut(outs);

		VONEILP._env.out() << "Solution Status = " << VONEILP._cplex.getStatus() << endl;
		VONEILP._env.out() << "Solution Objective Value Maximum Ms_r = " << VONEILP._cplex.getObjValue() << endl;
		VONEILP._env.out() << "Solution Variables Value are following:\n";
		VONEILP._env.out() << left << setw(30) << "Variable Name" << "Value" << endl;

		for (int i = 0; i < _values.getSize(); i++)
		{
			if (_values[i] != 0)
			{
				string VarName = VONEILP._variables[i].getName();
				if (VarName[0] != 'M')
				{
					VONEILP._env.out() << left << setw(30) << VarName
						<< _values[i] << endl;
				}
			}
		}

		return outs;
	}

	VONECplex::~VONECplex()
	{
		_env.end();
	}
}
namespace
{
	void getILPVertexInputParameter
	(const VONETopo::Topo& Substrate_Network,
		const vector<VONETopo::Topo>& Virtual_Network_List,
		int& Request_Number,
		int& Substrate_Network_Vertex_Number,
		vector<int>& Virtual_Networks_Vertex_Number,
		vector<int>& Accumulate_Virtual_Networks_Vertex_Number,
		vector<int>& Computing_Capacity_in_Substrate_Vertexes,
		vector<vector<int> >& Computing_Capacity_in_Virtual_Vertexes)
	{
		VONETopo::NodeList Substrate_Network_NodeList;
		Substrate_Network.get_vertex(Substrate_Network_NodeList);

		Substrate_Network_Vertex_Number = static_cast<int>(Substrate_Network_NodeList.size());

		for (VONETopo::NodeListIT SNTopoIT = Substrate_Network_NodeList.begin();
			SNTopoIT != Substrate_Network_NodeList.end(); SNTopoIT++)
		{
			Computing_Capacity_in_Substrate_Vertexes.push_back(SNTopoIT->_vertex_computing_capacity);
		}

		Request_Number = static_cast<int>(Virtual_Network_List.size());

		Computing_Capacity_in_Virtual_Vertexes.resize(Request_Number);

		for (vector<VONETopo::Topo>::const_iterator TopoIT = Virtual_Network_List.begin();
			TopoIT != Virtual_Network_List.end(); TopoIT++)
		{
			VONETopo::NodeList Virtual_Network_NodeList;
			TopoIT->get_vertex(Virtual_Network_NodeList);
			Virtual_Networks_Vertex_Number.push_back(static_cast<int>(Virtual_Network_NodeList.size()));

			int request_index = static_cast<int>(TopoIT - Virtual_Network_List.begin() + 1);

			if (request_index == 1)
			{
				Accumulate_Virtual_Networks_Vertex_Number.push_back(0);
			}
			else
			{

				int Sum = accumulate(Virtual_Networks_Vertex_Number.begin(),
					Virtual_Networks_Vertex_Number.begin() + request_index - 1, 0);
				Accumulate_Virtual_Networks_Vertex_Number.push_back(Sum);
			}

			for (VONETopo::NodeListIT VNTopoIT = Virtual_Network_NodeList.begin();
				VNTopoIT != Virtual_Network_NodeList.end(); VNTopoIT++)
			{
				Computing_Capacity_in_Virtual_Vertexes[request_index - 1].push_back(VNTopoIT->_vertex_computing_capacity);
			}
		}
	}

	void constraints1
	(IloBoolVarArray x,
		IloRangeArray c,
		const int Request_Number,
		const int Substrate_Network_Vertex_Number,
		const vector<int> Virtual_Networks_Vertex_Number,
		const vector<int> Accumulate_Virtual_Networks_Vertex_Number)
	{
		IloEnv env = c.getEnv();
		string Variable = "y_";
		for (int r = 1; r <= Request_Number; r++)
		{
			for (int v = 1; v <= Virtual_Networks_Vertex_Number[r - 1]; v++)
			{
				IloExpr con1(env);
				for (int s = 1; s <= Substrate_Network_Vertex_Number; s++)
				{
					string Variable_Name = Variable + to_string(r) + "_" + to_string(v) + "_" + to_string(s);
					x.add(IloBoolVar(env, Variable_Name.c_str()));
					con1 += x[(s - 1)
						+ (v - 1) * Substrate_Network_Vertex_Number
						+ Substrate_Network_Vertex_Number * Accumulate_Virtual_Networks_Vertex_Number[r - 1]];
				}
				c.add(con1 == 1);
			}
		}
	}

	void constraints2
	(IloBoolVarArray x,
		IloRangeArray c,
		const int Request_Number,
		const int Substrate_Network_Vertex_Number,
		const vector<int> Virtual_Networks_Vertex_Number,
		const vector<int> Accumulate_Virtual_Networks_Vertex_Number)
	{
		IloEnv env = c.getEnv();
		for (int r = 1; r <= Request_Number; r++)
		{
			for (int s = 1; s <= Substrate_Network_Vertex_Number; s++)
			{
				IloExpr con2(env);
				for (int v = 1; v <= Virtual_Networks_Vertex_Number[r - 1]; v++)
				{
					con2 += x[(s - 1)
						+ (v - 1) * Substrate_Network_Vertex_Number
						+ Substrate_Network_Vertex_Number * Accumulate_Virtual_Networks_Vertex_Number[r - 1]];
				}
				c.add(con2 <= 1);
			}
		}
	}

	void constraints3
	(IloBoolVarArray x,
		IloRangeArray c,
		const int Request_Number,
		const int Substrate_Network_Vertex_Number,
		const vector<int> Virtual_Networks_Vertex_Number,
		const vector<int> Accumulate_Virtual_Networks_Vertex_Number,
		const vector<int> Computing_Capacity_in_Substrate_Vertexes,
		const vector<vector<int> > Computing_Capacity_in_Virtual_Vertexes)
	{
		IloEnv env = c.getEnv();
		for (int s = 1; s <= Substrate_Network_Vertex_Number; s++)
		{
			IloExpr con3(env);
			for (int r = 1; r <= Request_Number; r++)
			{
				for (int v = 1; v <= Virtual_Networks_Vertex_Number[r - 1]; v++)
				{
					con3 += x[(s - 1)
						+ (v - 1) * Substrate_Network_Vertex_Number
						+ Substrate_Network_Vertex_Number * Accumulate_Virtual_Networks_Vertex_Number[r - 1]]
						* static_cast<int> (Computing_Capacity_in_Virtual_Vertexes[r - 1][v - 1]);
				}
			}
			c.add(con3 <= static_cast<int> (Computing_Capacity_in_Substrate_Vertexes[s - 1]));
		}
	}

	void getILPEdgeInputParameter
	(const VONETopo::Topo& Substrate_Network,
		const vector<VONETopo::Topo>& Virtual_Network_List,
		VONETopo::LinkList& Substrate_Network_Edge,
		int& Substrate_Network_Edge_Number,
		vector<VONETopo::LinkList>& Virtual_Networks_Edge,
		vector<int>& Virtual_Networks_Edge_Number,
		vector<int>& Accumulate_Virtual_Networks_Edge_Number,
		vector<int>& Virtual_Networks_Bandwidth)
	{
		Substrate_Network.get_edge(Substrate_Network_Edge);
		Substrate_Network_Edge_Number = static_cast<int>(Substrate_Network_Edge.size());

		for (vector<VONETopo::Topo>::const_iterator VNTopoIT = Virtual_Network_List.begin();
			VNTopoIT != Virtual_Network_List.end(); VNTopoIT++)
		{
			VONETopo::LinkList Virtual_Network_EdgeList;
			VNTopoIT->get_edge(Virtual_Network_EdgeList);
			Virtual_Networks_Edge.push_back(Virtual_Network_EdgeList);
			Virtual_Networks_Edge_Number.push_back(static_cast<int>(Virtual_Network_EdgeList.size()));
			Virtual_Networks_Bandwidth.push_back(Virtual_Network_EdgeList[0]._edge_bandwidth);

			int request_index = static_cast<int>(VNTopoIT - Virtual_Network_List.begin() + 1);

			if (request_index == 1)
			{
				Accumulate_Virtual_Networks_Edge_Number.push_back(0);
			}
			else
			{

				int Sum = accumulate(Virtual_Networks_Edge_Number.begin(),
					Virtual_Networks_Edge_Number.begin() + request_index - 1, 0);
				Accumulate_Virtual_Networks_Edge_Number.push_back(Sum);
			}
		}
	}

	void constraints4
	(IloBoolVarArray x,
		IloRangeArray c,
		const int Request_Number,
		const int Substrate_Network_Edge_Number,
		const vector<int> Virtual_Networks_Edge_Number,
		const vector<int> Accumulate_Virtual_Networks_Edge_Number,
		const VONETopo::LinkList& Substrate_Network_Edge)
	{
		IloEnv env = c.getEnv();
		string Variable = "x_";
		for (int r = 1; r <= Request_Number; r++)
		{
			for (int sd = 1; sd <= Substrate_Network_Edge_Number; sd++)
			{
				for (int e = 1; e <= Virtual_Networks_Edge_Number[r - 1]; e++)
				{
					IloExpr con4(env);
					string Variable_Name = Variable + to_string(r) + "_" + to_string(e) + "_"
						+ to_string(Substrate_Network_Edge[sd - 1]._edge_source._vertex_id) + "_"
						+ to_string(Substrate_Network_Edge[sd - 1]._edge_destination._vertex_id);
					x.add(IloBoolVar(env, Variable_Name.c_str()));
					Variable_Name = Variable + to_string(r) + "_" + to_string(e) + "_"
						+ to_string(Substrate_Network_Edge[sd - 1]._edge_destination._vertex_id) + "_"
						+ to_string(Substrate_Network_Edge[sd - 1]._edge_source._vertex_id);
					x.add(IloBoolVar(env, Variable_Name.c_str()));
					con4 += x[2 * (e - 1)
						+ (sd - 1) * Virtual_Networks_Edge_Number[r - 1] * 2
						+ Accumulate_Virtual_Networks_Edge_Number[r - 1] * 2 * Substrate_Network_Edge_Number]
						+ x[(2 * e - 1)
						+ (sd - 1) * Virtual_Networks_Edge_Number[r - 1] * 2
						+ Accumulate_Virtual_Networks_Edge_Number[r - 1] * 2 * Substrate_Network_Edge_Number];
					c.add(con4 <= 1);
				}
			}
		}
	}

	void constraints5
	(IloBoolVarArray x,
		IloRangeArray c,
		const IloBoolVarArray y,
		const int Request_Number,
		const int Substrate_Network_Edge_Number,
		const vector<int> Virtual_Networks_Edge_Number,
		const vector<int> Accumulate_Virtual_Networks_Edge_Number,
		const int Substrate_Network_Vertex_Number,
		const vector<int> Accumulate_Virtual_Networks_Vertex_Number,
		const VONETopo::LinkList& Substrate_Network_Edge,
		const vector<VONETopo::LinkList>& Virtual_Networks_Edge)
	{
		IloEnv env = c.getEnv();
		for (int r = 1; r <= Request_Number; r++)
		{
			for (int e = 1; e <= Virtual_Networks_Edge_Number[r - 1]; e++)
			{
				for (int s = 1; s <= Substrate_Network_Vertex_Number; s++)
				{
					IloExpr con5(env);
					for (int sd = 1; sd <= Substrate_Network_Edge_Number; sd++)
					{
						if (Substrate_Network_Edge[sd - 1]._edge_source._vertex_id == s)
						{
							con5 += x[2 * (e - 1)
								+ (sd - 1) * Virtual_Networks_Edge_Number[r - 1] * 2
								+ Accumulate_Virtual_Networks_Edge_Number[r - 1] * 2 * Substrate_Network_Edge_Number]
								- x[(2 * e - 1)
								+ (sd - 1) * Virtual_Networks_Edge_Number[r - 1] * 2
								+ Accumulate_Virtual_Networks_Edge_Number[r - 1] * 2 * Substrate_Network_Edge_Number];
						}

						if (Substrate_Network_Edge[sd - 1]._edge_destination._vertex_id == s)
						{
							con5 += x[(2 * e - 1)
								+ (sd - 1) * Virtual_Networks_Edge_Number[r - 1] * 2
								+ Accumulate_Virtual_Networks_Edge_Number[r - 1] * 2 * Substrate_Network_Edge_Number]
								- x[2 * (e - 1)
								+ (sd - 1) * Virtual_Networks_Edge_Number[r - 1] * 2
								+ Accumulate_Virtual_Networks_Edge_Number[r - 1] * 2 * Substrate_Network_Edge_Number];
						}
					}
					c.add(con5 - y[(s - 1)
						+ (Virtual_Networks_Edge[r - 1][e - 1]._edge_source._vertex_id - 1) * Substrate_Network_Vertex_Number
						+ Substrate_Network_Vertex_Number * Accumulate_Virtual_Networks_Vertex_Number[r - 1]] + y[(s - 1)
						+ (Virtual_Networks_Edge[r - 1][e - 1]._edge_destination._vertex_id - 1) * Substrate_Network_Vertex_Number
						+ Substrate_Network_Vertex_Number * Accumulate_Virtual_Networks_Vertex_Number[r - 1]] == 0);
				}
			}
		}
	}

	void constraints6
	(IloBoolVarArray x,
		IloRangeArray con,
		const IloBoolVarArray y,
		const int Request_Number,
		const int Substrate_Network_Edge_Number,
		const vector<int> Virtual_Networks_Edge_Number,
		const vector<int> Accumulate_Virtual_Networks_Edge_Number,
		const VONETopo::LinkList& Substrate_Network_Edge,
		const int C)
	{
		IloEnv env = con.getEnv();
		string Variable = "X_";
		for (int r = 1; r <= Request_Number; r++)
		{
			for (int sd = 1; sd <= Substrate_Network_Edge_Number; sd++)
			{
				IloExpr con6_1(env), con6_2(env);
				for (int c = 1; c <= C; c++)
				{
					string Variable_Name = Variable + to_string(r) + "_" + to_string(c) + "_"
						+ to_string(Substrate_Network_Edge[sd - 1]._edge_source._vertex_id) + "_"
						+ to_string(Substrate_Network_Edge[sd - 1]._edge_destination._vertex_id);
					x.add(IloBoolVar(env, Variable_Name.c_str()));
					Variable_Name = Variable + to_string(r) + "_" + to_string(c) + "_"
						+ to_string(Substrate_Network_Edge[sd - 1]._edge_destination._vertex_id) + "_"
						+ to_string(Substrate_Network_Edge[sd - 1]._edge_source._vertex_id);
					x.add(IloBoolVar(env, Variable_Name.c_str()));
					con6_1 += x[2 * (c - 1)
						+ (sd - 1) * C * 2
						+ (r - 1) * C * 2 * Substrate_Network_Edge_Number];
					con6_2 += x[(2 * c - 1)
						+ (sd - 1) * C * 2
						+ (r - 1) * C * 2 * Substrate_Network_Edge_Number];
				}

				IloExpr con6_1_right(env), con6_2_right(env);
				for (int e = 1; e <= Virtual_Networks_Edge_Number[r - 1]; e++)
				{
					con6_1_right += y[2 * (e - 1)
						+ (sd - 1) *  Virtual_Networks_Edge_Number[r - 1] * 2
						+ Accumulate_Virtual_Networks_Edge_Number[r - 1] * 2 * Substrate_Network_Edge_Number];
					con6_2_right += y[(2 * e - 1)
						+ (sd - 1) * Virtual_Networks_Edge_Number[r - 1] * 2
						+ Accumulate_Virtual_Networks_Edge_Number[r - 1] * 2 * Substrate_Network_Edge_Number];
				}

				con.add(con6_1 - con6_1_right == 0);
				con.add(con6_2 - con6_2_right == 0);
			}
		}
	}

	void constraints6_heter
	(IloBoolVarArray x,
		IloRangeArray con,
		const IloBoolVarArray y,
		const int Request_Number,
		const int Substrate_Network_Edge_Number,
		const vector<int> Virtual_Networks_Edge_Number,
		const vector<int> Accumulate_Virtual_Networks_Edge_Number,
		const VONETopo::LinkList& Substrate_Network_Edge,
		const vector<int> Virtual_Networks_Bandwidth,
		const int C,
		const int C_Heter)
	{
		IloEnv env = con.getEnv();
		string Variable = "X_";
		for (int r = 1; r <= Request_Number; r++)
		{
			int Omega_r = Virtual_Networks_Bandwidth[r - 1];
			for (int sd = 1; sd <= Substrate_Network_Edge_Number; sd++)
			{
				IloExpr con6_1(env), con6_2(env);
				for (int c = 1; c <= C; c++)
				{
					string Variable_Name = Variable + to_string(r) + "_" + to_string(c) + "_"
						+ to_string(Substrate_Network_Edge[sd - 1]._edge_source._vertex_id) + "_"
						+ to_string(Substrate_Network_Edge[sd - 1]._edge_destination._vertex_id);
					x.add(IloBoolVar(env, Variable_Name.c_str()));
					Variable_Name = Variable + to_string(r) + "_" + to_string(c) + "_"
						+ to_string(Substrate_Network_Edge[sd - 1]._edge_destination._vertex_id) + "_"
						+ to_string(Substrate_Network_Edge[sd - 1]._edge_source._vertex_id);
					x.add(IloBoolVar(env, Variable_Name.c_str()));

					int c_tmp = 0;
					if (Omega_r == N)
					{
						if (c == C_Heter)
							c_tmp = c;
						else
							continue;
					}
					else
					{
						if (c == C_Heter)
							continue;
						else
							c_tmp = c;
					}

					con6_1 += x[2 * (c_tmp - 1)
						+ (sd - 1) * C * 2
						+ (r - 1) * C * 2 * Substrate_Network_Edge_Number];
					con6_2 += x[(2 * c_tmp - 1)
						+ (sd - 1) * C * 2
						+ (r - 1) * C * 2 * Substrate_Network_Edge_Number];
				}

				IloExpr con6_1_right(env), con6_2_right(env);
				for (int e = 1; e <= Virtual_Networks_Edge_Number[r - 1]; e++)
				{
					con6_1_right += y[2 * (e - 1)
						+ (sd - 1) *  Virtual_Networks_Edge_Number[r - 1] * 2
						+ Accumulate_Virtual_Networks_Edge_Number[r - 1] * 2 * Substrate_Network_Edge_Number];
					con6_2_right += y[(2 * e - 1)
						+ (sd - 1) * Virtual_Networks_Edge_Number[r - 1] * 2
						+ Accumulate_Virtual_Networks_Edge_Number[r - 1] * 2 * Substrate_Network_Edge_Number];
				}

				con.add(con6_1 - con6_1_right == 0);
				con.add(con6_2 - con6_2_right == 0);
			}
		}
	}

	void constraints7
	(IloIntVarArray x,
		IloRangeArray con,
		const IloIntVarArray y,
		const int Request_Number,
		const vector<int> Virtual_Networks_Bandwidth,
		const int Capacity)
	{
		IloEnv env = con.getEnv();
		string Variable = "Ms_";
		for (int r = 1; r <= Request_Number; r++)
		{
			string Variable_Name = Variable + to_string(r);
			x.add(IloIntVar(env, 1, Capacity, Variable_Name.c_str()));
			int Omega_r = Virtual_Networks_Bandwidth[r - 1];
			con.add(y[r - 1] - x[r - 1] == 1 - Omega_r);
		}
	}

	void constraints7_heter
	(IloIntVarArray x,
		IloRangeArray con,
		const IloIntVarArray y,
		const int Request_Number,
		const vector<int> Virtual_Networks_Bandwidth,
		const int Capacity)
	{
		IloEnv env = con.getEnv();
		string Variable = "Ms_";
		for (int r = 1; r <= Request_Number; r++)
		{
			string Variable_Name = Variable + to_string(r);
			x.add(IloIntVar(env, 1, Capacity, Variable_Name.c_str()));
			int Omega_r = Virtual_Networks_Bandwidth[r - 1];
			if (Omega_r == N)
				con.add(y[r - 1] - x[r - 1] == 1 - Omega_r / M);
			else
				con.add(y[r - 1] - x[r - 1] == 1 - Omega_r);
		}
	}

	void constraints8
	(IloModel mod,
		const IloIntVarArray x,
		const IloIntVarArray y,
		const int Request_Number,
		const int Substrate_Network_Edge_Number,
		const vector<int> Virtual_Networks_Bandwidth,
		const int C,
		const bool Cross_Talk_Flag)
	{
		IloEnv env = mod.getEnv();

		for (int r1 = 1; r1 <= Request_Number - 1; r1++)
		{
			for (int r2 = r1 + 1; r2 <= Request_Number; r2++)
			{
				int Omega_r1 = Virtual_Networks_Bandwidth[r1 - 1];
				int Omega_r2 = Virtual_Networks_Bandwidth[r2 - 1];
				for (int sd = 1; sd <= Substrate_Network_Edge_Number; sd++)
				{
					for (int c = 1; c <= C; c++)
					{
						IloExpr same_core_con1(env), same_core_con2(env),
							spectrum_consistency_con(env);

						same_core_con1 = x[2 * (c - 1)
							+ (sd - 1) * C * 2
							+ (r1 - 1) * C * 2 * Substrate_Network_Edge_Number] +
							x[2 * (c - 1)
							+ (sd - 1) * C * 2
							+ (r2 - 1) * C * 2 * Substrate_Network_Edge_Number];

						same_core_con2 = x[(2 * c - 1)
							+ (sd - 1) * C * 2
							+ (r1 - 1) * C * 2 * Substrate_Network_Edge_Number] +
							x[(2 * c - 1)
							+ (sd - 1) * C * 2
							+ (r2 - 1) * C * 2 * Substrate_Network_Edge_Number];

						spectrum_consistency_con = (y[r1 - 1] - y[r2 - 1] >= Omega_r2)
							+ (y[r2 - 1] - y[r1 - 1] >= Omega_r1);

						if (Cross_Talk_Flag == true && Omega_r1 == Omega_r2 && C > 1)
						{
							//Taking the cross-talk between two adjacent cores into account:
							IloExpr	font_adjacent_core_con1(env), back_adjacent_core_con1(env),
								font_adjacent_core_con2(env), back_adjacent_core_con2(env);

							font_adjacent_core_con1 = x[2 * (c - 1)
								+ (sd - 1) * C * 2
								+ (r1 - 1) * C * 2 * Substrate_Network_Edge_Number] +
								x[2 * ((c + C) % C + 1 - 1) //font adjacent core index equals to the back
								+ (sd - 1) * C * 2
								+ (r2 - 1) * C * 2 * Substrate_Network_Edge_Number];

							font_adjacent_core_con2 = x[(2 * c - 1)
								+ (sd - 1) * C * 2
								+ (r1 - 1) * C * 2 * Substrate_Network_Edge_Number] +
								x[(2 * ((c + C) % C + 1) - 1) //font adjacent core index equals to the back
								+ (sd - 1) * C * 2
								+ (r2 - 1) * C * 2 * Substrate_Network_Edge_Number];

							back_adjacent_core_con1 = x[2 * (c - 1)
								+ (sd - 1) * C * 2
								+ (r1 - 1) * C * 2 * Substrate_Network_Edge_Number] +
								x[2 * ((c + C - 2) % C + 1 - 1) //back adjacent core index
								+ (sd - 1) * C * 2
								+ (r2 - 1) * C * 2 * Substrate_Network_Edge_Number];

							back_adjacent_core_con2 = x[(2 * c - 1)
								+ (sd - 1) * C * 2
								+ (r1 - 1) * C * 2 * Substrate_Network_Edge_Number] +
								x[(2 * ((c + C - 2) % C + 1) - 1) //back adjacent core index
								+ (sd - 1) * C * 2
								+ (r2 - 1) * C * 2 * Substrate_Network_Edge_Number];

							if (C == 2)
							{
								mod.add(IloIfThen(env,
									(same_core_con1 == 2) + (font_adjacent_core_con1 == 2) == 1,
									spectrum_consistency_con == 1)); //font adjacent core index equals to the back

								mod.add(IloIfThen(env,
									(same_core_con2 == 2) + (font_adjacent_core_con2 == 2) == 1,
									spectrum_consistency_con == 1)); //font adjacent core index equals to the back
							}
							else
							{
								mod.add(IloIfThen(env,
									(same_core_con1 == 2)
									+ (font_adjacent_core_con1 == 2) //font adjacent core index
									+ (back_adjacent_core_con1 == 2) == 1, //back adjacent core index
									spectrum_consistency_con == 1));

								mod.add(IloIfThen(env,
									(same_core_con2 == 2)
									+ (font_adjacent_core_con2 == 2) //font adjacent core index
									+ (back_adjacent_core_con2 == 2) == 1, //back adjacent core index
									spectrum_consistency_con == 1));
							}
						}
						else
						{
							//Without taking the cross-talk between two adjacent cores into account:
							mod.add(IloIfThen(env,
								same_core_con1 == 2, spectrum_consistency_con == 1));

							mod.add(IloIfThen(env,
								same_core_con2 == 2, spectrum_consistency_con == 1));
						}
					}
				}
			}
		}
	}

	void constraints8_heter
	(IloModel mod,
		const IloIntVarArray x,
		const IloIntVarArray y,
		const int Request_Number,
		const int Substrate_Network_Edge_Number,
		const vector<int> Virtual_Networks_Bandwidth,
		const int C,
		const int C_Heter,
		const bool Cross_Talk_Flag)
	{
		IloEnv env = mod.getEnv();

		for (int r1 = 1; r1 <= Request_Number - 1; r1++)
		{
			for (int r2 = r1 + 1; r2 <= Request_Number; r2++)
			{
				int Omega_r1 = Virtual_Networks_Bandwidth[r1 - 1];
				int Omega_r2 = Virtual_Networks_Bandwidth[r2 - 1];
				for (int sd = 1; sd <= Substrate_Network_Edge_Number; sd++)
				{
					for (int c = 1; c <= C; c++)
					{
						IloExpr same_core_con1(env), same_core_con2(env),
							spectrum_consistency_con1(env), spectrum_consistency_con2(env);

						same_core_con1 = x[2 * (c - 1)
							+ (sd - 1) * C * 2
							+ (r1 - 1) * C * 2 * Substrate_Network_Edge_Number] +
							x[2 * (c - 1)
							+ (sd - 1) * C * 2
							+ (r2 - 1) * C * 2 * Substrate_Network_Edge_Number];

						same_core_con2 = x[(2 * c - 1)
							+ (sd - 1) * C * 2
							+ (r1 - 1) * C * 2 * Substrate_Network_Edge_Number] +
							x[(2 * c - 1)
							+ (sd - 1) * C * 2
							+ (r2 - 1) * C * 2 * Substrate_Network_Edge_Number];

						spectrum_consistency_con1 = (y[r1 - 1] - y[r2 - 1] >= Omega_r2)
							+ (y[r2 - 1] - y[r1 - 1] >= Omega_r1);

						spectrum_consistency_con2 = (y[r1 - 1] - y[r2 - 1] >= Omega_r2 / M)
							+ (y[r2 - 1] - y[r1 - 1] >= Omega_r1 / M);

						if (Cross_Talk_Flag == true && Omega_r1 == Omega_r2 && C > 2)
						{
							//Taking the cross-talk between two adjacent cores into account:
							IloExpr	font_adjacent_core_con1(env), back_adjacent_core_con1(env),
								font_adjacent_core_con2(env), back_adjacent_core_con2(env);

							font_adjacent_core_con1 = x[2 * (c - 1)
								+ (sd - 1) * C * 2
								+ (r1 - 1) * C * 2 * Substrate_Network_Edge_Number] +
								x[2 * ((c + C) % C + 1 - 1) //font adjacent core index equals to the back
								+ (sd - 1) * C * 2
								+ (r2 - 1) * C * 2 * Substrate_Network_Edge_Number];

							font_adjacent_core_con2 = x[(2 * c - 1)
								+ (sd - 1) * C * 2
								+ (r1 - 1) * C * 2 * Substrate_Network_Edge_Number] +
								x[(2 * ((c + C) % C + 1) - 1) //font adjacent core index equals to the back
								+ (sd - 1) * C * 2
								+ (r2 - 1) * C * 2 * Substrate_Network_Edge_Number];

							back_adjacent_core_con1 = x[2 * (c - 1)
								+ (sd - 1) * C * 2
								+ (r1 - 1) * C * 2 * Substrate_Network_Edge_Number] +
								x[2 * ((c + C - 2) % C + 1 - 1) //back adjacent core index
								+ (sd - 1) * C * 2
								+ (r2 - 1) * C * 2 * Substrate_Network_Edge_Number];

							back_adjacent_core_con2 = x[(2 * c - 1)
								+ (sd - 1) * C * 2
								+ (r1 - 1) * C * 2 * Substrate_Network_Edge_Number] +
								x[(2 * ((c + C - 2) % C + 1) - 1) //back adjacent core index
								+ (sd - 1) * C * 2
								+ (r2 - 1) * C * 2 * Substrate_Network_Edge_Number];

							if (C == 3)
							{
								if (c == C_Heter)
								{
									if (Omega_r1 == N)
									{
										mod.add(IloIfThen(env,
											same_core_con1 == 2, spectrum_consistency_con2 == 1));

										mod.add(IloIfThen(env,
											same_core_con2 == 2, spectrum_consistency_con2 == 1));
									}
								}
								else
								{
									if (Omega_r1 != N)
									{
										mod.add(IloIfThen(env,
											(same_core_con1 == 2) +
											(x[2 * (c - 1)
												+ (sd - 1) * C * 2
												+ (r1 - 1) * C * 2 * Substrate_Network_Edge_Number] +
												x[2 * ((((c + C) % C + 1) == C_Heter ? (C_Heter + C) % C + 1 : (c + C) % C + 1) - 1) //font adjacent core index equals to the back
												+ (sd - 1) * C * 2
												+ (r2 - 1) * C * 2 * Substrate_Network_Edge_Number] == 2) == 1,
											spectrum_consistency_con1 == 1));

										mod.add(IloIfThen(env,
											(same_core_con2 == 2) +
											(x[(2 * c - 1)
												+ (sd - 1) * C * 2
												+ (r1 - 1) * C * 2 * Substrate_Network_Edge_Number] +
												x[(2 * (((c + C) % C + 1) == C_Heter ? (C_Heter + C) % C + 1 : (c + C) % C + 1) - 1) //font adjacent core index equals to the back
												+ (sd - 1) * C * 2
												+ (r2 - 1) * C * 2 * Substrate_Network_Edge_Number] == 2) == 1,
											spectrum_consistency_con1 == 1));
									}
								}
							}
							else
							{
								if (c == C_Heter)
								{
									if (Omega_r1 == N)
									{
										mod.add(IloIfThen(env,
											same_core_con1 == 2, spectrum_consistency_con2 == 1));

										mod.add(IloIfThen(env,
											same_core_con2 == 2, spectrum_consistency_con2 == 1));
									}
								}
								else
								{
									if (Omega_r1 != N)
									{
										if (((c + C - 2) % C + 1) == C_Heter)
										{
											mod.add(IloIfThen(env,
												(same_core_con1 == 2) + (font_adjacent_core_con1 == 2) == 1, spectrum_consistency_con1 == 1));

											mod.add(IloIfThen(env,
												(same_core_con2 == 2) + (font_adjacent_core_con2 == 2) == 1, spectrum_consistency_con1 == 1));
										}
										else if (((c + C) % C + 1) == C_Heter)
										{
											mod.add(IloIfThen(env,
												(same_core_con1 == 2) + (back_adjacent_core_con1 == 2) == 1, spectrum_consistency_con1 == 1));

											mod.add(IloIfThen(env,
												(same_core_con2 == 2) + (back_adjacent_core_con2 == 2) == 1, spectrum_consistency_con1 == 1));
										}
										else
										{
											mod.add(IloIfThen(env,
												(font_adjacent_core_con1 == 2) +
												(same_core_con1 == 2) +
												(back_adjacent_core_con1 == 2) == 1, spectrum_consistency_con1 == 1));

											mod.add(IloIfThen(env,
												(font_adjacent_core_con2 == 2) +
												(same_core_con2 == 2) +
												(back_adjacent_core_con2 == 2) == 1, spectrum_consistency_con1 == 1));
										}
									}
								}
							}
						}
						else
						{
							//Without taking the cross-talk between two adjacent cores into account:
							if (c == C_Heter)
							{
								if (Omega_r1 == N && Omega_r2 == N)
								{
									mod.add(IloIfThen(env,
										same_core_con1 == 2, spectrum_consistency_con2 == 1));

									mod.add(IloIfThen(env,
										same_core_con2 == 2, spectrum_consistency_con2 == 1));
								}
							}
							else
							{
								if (Omega_r1 != N && Omega_r2 != N)
								{
									mod.add(IloIfThen(env,
										same_core_con1 == 2, spectrum_consistency_con1 == 1));

									mod.add(IloIfThen(env,
										same_core_con2 == 2, spectrum_consistency_con1 == 1));
								}
							}
						}
					}
				}
			}
		}
	}
}