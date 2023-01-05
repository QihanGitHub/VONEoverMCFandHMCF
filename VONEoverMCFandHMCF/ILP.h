//This file declares class VONECplex.
//------------------------------------------------
//File Name: ILP.h
//Author: Qihan Zhang
//Email: lengkudaodi@outlook.com
//Last Modified: Aug. 3rd 2021
//------------------------------------------------

#pragma once
#include <ilcplex/ilocplex.h>
#include "Topo.h"
ILOSTLBEGIN

namespace VONEILP
{
	//Create CPLEX class to solve ILP of VONE
	class VONECplex
	{
	public:
		VONECplex();

		void solve(const VONETopo::Topo& Substrate_Network, const vector<VONETopo::Topo>& Virtual_Network_List, 
			int CoreNumber, int HeterogeneousCoreIndex, string CrosstalkYesorNo);
		//Precondition: Topologies of substrate netwrok and virtual networks have been assigned, CoreNumber, HeterogeneousCoreIndex,
		//	and CrosstalkYesorNo have been initialized
		//Postcondition: Solve the integer linear programming of VONE based on the Substrate_Network and Virtual_Network_List

		friend ostream& operator <<(ostream& outs, VONECplex& VONEILP);
		//Precondition: outs has been connected to an out stream
		//Postcondition: Print the solution information of the ILP problem to outs

		~VONECplex();

	private:
		IloEnv _env;

		IloModel _model;
		IloIntVarArray _variables;
		IloObjective _objective;
		IloRangeArray _constraints;

		IloCplex _cplex;
	};
}
