//This file is the main function of VONE on MCF and HMCF.
//------------------------------------------------
//File Name: VONEoverMCFandHMCF.cpp
//Author: Qihan Zhang
//Email: lengkudaodi@outlook.com
//Last Modified: Aug. 3rd 2021
//------------------------------------------------

#include "ILP.h"
#include "Heuristic.h"

namespace
{
	static void Usage(const char* ProgrameName);
}

//***************Program entry***************//
//MainError Code which is the retrun integer value of the main function can indicate the mistake details
//when debug the program:
//0: There are no errors during the executing;
//-1: The number of program arguments is wrong, may be not enough or too much; 
//-2: One of the name of the arguments is wrong;
//-3: The topology input file fails to read when the argument "Input" has been set, check the file opened code;
//-4: The argument "HeterogeneousCoreIndex" should be a non-negetive integer, check the input parameter;
//-5: The argument "CrosstalkYesorNo" should be one of the strings as following: Yes, yes No or no;
//-6: The argument "Solver" should be one of the strings as following: ILP, Heuristic or ILPHeuristic;
//-7: The result file failed to open.

int main(int argc, char **argv)
{
	int ServiceRequestNumber, CoreNumber, HeterogeneousCoreIndex, MainError;
	string TypeofTopology, TopoFilename, CrosstalkYesorNo, Solver;
	switch (argc)
	{
	case 1:
		Usage(argv[0]);
		MainError = -1;
		break;
	case 2:
		if ((strcmp(argv[1], "-h") == 0) || (strcmp(argv[1], "-help") == 0))
		{
			Usage(argv[0]);
			MainError = 0;
			exit(1);
		}
		else
		{
			cerr << "Wrong argument name!\n";
			MainError = -2;
		}
		break;
	case 3:
	case 4:
	case 5:
	case 6:
	case 7:
	case 8:
	case 9:
	case 10:
	case 11:
	case 12:
		Usage(argv[0]);
		MainError = -1;
		break;
	case 13:
		MainError = 0;
		for (int i = 2; i <= argc; i = i + 2)
		{
			if ((strcmp(argv[i - 1], "-t") == 0) || (strcmp(argv[i - 1], "-topo") == 0))
			{
				if (strcmp(argv[i + 1 - 1], "Input") == 0)
				{
					cerr << "Wrong argument name!\n";
					MainError = -2;
					break;
				}
				else
					TypeofTopology = argv[i + 1 - 1];
			}
			else if ((strcmp(argv[i - 1], "-r") == 0) || (strcmp(argv[i - 1], "-rnum") == 0))
				ServiceRequestNumber = stoi(argv[i + 1 - 1]);
			else if ((strcmp(argv[i - 1], "-c") == 0) || (strcmp(argv[i - 1], "-cnum") == 0))
				CoreNumber = stoi(argv[i + 1 - 1]);
			else if ((strcmp(argv[i - 1], "-h") == 0) || (strcmp(argv[i - 1], "-hetercindex") == 0))
				HeterogeneousCoreIndex = stoi(argv[i + 1 - 1]);
			else if ((strcmp(argv[i - 1], "-x") == 0) || (strcmp(argv[i - 1], "-crosstalk") == 0))
				CrosstalkYesorNo = argv[i + 1 - 1];
			else if ((strcmp(argv[i - 1], "-s") == 0) || (strcmp(argv[i - 1], "-solver") == 0))
				Solver = argv[i + 1 - 1];
			else
			{
				cerr << "Wrong argument name!\n";
				MainError = -2;
				break;
			}
		}
		break;
	case 14:
		MainError = 0;
		for (int i = 2; i <= argc; i++)
		{
			if ((strcmp(argv[i - 1], "-t") == 0) || (strcmp(argv[i - 1], "-topo") == 0))
			{
				if (strcmp(argv[i + 1 - 1], "Input") == 0)
				{
					TypeofTopology = argv[i + 1 - 1];
					TopoFilename = argv[i + 2 - 1];
				}
				else
				{
					cerr << "Wrong argument name!\n";
					MainError = -2;
					break;
				}
			}
			else if ((strcmp(argv[i - 1], "-r") == 0) || (strcmp(argv[i - 1], "-rnum") == 0))
				ServiceRequestNumber = stoi(argv[i + 1 - 1]);
			else if ((strcmp(argv[i - 1], "-c") == 0) || (strcmp(argv[i - 1], "-cnum") == 0))
				CoreNumber = stoi(argv[i + 1 - 1]);
			else if ((strcmp(argv[i - 1], "-h") == 0) || (strcmp(argv[i - 1], "-hetercindex") == 0))
				HeterogeneousCoreIndex = stoi(argv[i + 1 - 1]);
			else if ((strcmp(argv[i - 1], "-x") == 0) || (strcmp(argv[i - 1], "-crosstalk") == 0))
				CrosstalkYesorNo = argv[i + 1 - 1];
			else if ((strcmp(argv[i - 1], "-s") == 0) || (strcmp(argv[i - 1], "-solver") == 0))
				Solver = argv[i + 1 - 1];
			else
			{
				cerr << "Wrong argument name!\n";
				MainError = -2;
				break;
			}
		}
		break;
	default:
		Usage(argv[0]);
		MainError = -1;
	}

	//Input the Substrate Network:
	VONETopo::Topo Substrate_Network;

	if (TypeofTopology == "SixNode")
	{
		//Six Node Topology:
		Substrate_Network.AddVertex(1, 50);
		Substrate_Network.AddVertex(2, 100);
		Substrate_Network.AddVertex(3, 100);
		Substrate_Network.AddVertex(4, 100);
		Substrate_Network.AddVertex(5, 100);
		Substrate_Network.AddVertex(6, 50);

		Substrate_Network.AddEdge(2, 1, 50);
		Substrate_Network.AddEdge(1, 3, 50);
		Substrate_Network.AddEdge(2, 4, 50);
		Substrate_Network.AddEdge(3, 4, 50);
		Substrate_Network.AddEdge(4, 5, 50);
		Substrate_Network.AddEdge(5, 6, 50);
		Substrate_Network.AddEdge(2, 3, 50);
		Substrate_Network.AddEdge(5, 3, 50);
		Substrate_Network.AddEdge(4, 6, 50);
	}
	else if (TypeofTopology == "FITI")
	{
		//FITI Topology:
		Substrate_Network.AddVertex(1, 150);	//Harbin
		Substrate_Network.AddVertex(2, 150);	//Changchun
		Substrate_Network.AddVertex(3, 1500);	//Shenyang
		Substrate_Network.AddVertex(4, 1500);	//Beijing
		Substrate_Network.AddVertex(5, 150);	//Dalian
		Substrate_Network.AddVertex(6, 750);	//Tianjin
		Substrate_Network.AddVertex(7, 150);	//Jinan
		Substrate_Network.AddVertex(8, 150);	//Lanzhou
		Substrate_Network.AddVertex(9, 150);	//Zhengzhou
		Substrate_Network.AddVertex(10, 1500);	//Xi'an
		Substrate_Network.AddVertex(11, 750);	//Nanjing
		Substrate_Network.AddVertex(12, 150);	//Hefei
		Substrate_Network.AddVertex(13, 1500);	//Shanghai
		Substrate_Network.AddVertex(14, 750);	//Hangzhou
		Substrate_Network.AddVertex(15, 1500);	//Chengdu
		Substrate_Network.AddVertex(16, 1500);	//Wuhan
		Substrate_Network.AddVertex(17, 750);	//Chongqing
		Substrate_Network.AddVertex(18, 150);	//Changsha
		Substrate_Network.AddVertex(19, 150);	//Xiamen
		Substrate_Network.AddVertex(20, 1500);	//Guangzhou
		Substrate_Network.AddVertex(21, 750);	//Shenzhen

		Substrate_Network.AddEdge(1, 2);	//1:Harbin<->Changchun
		Substrate_Network.AddEdge(2, 3);	//2:Changchun<->Shenyang
		Substrate_Network.AddEdge(3, 4);	//3:Shenyang<->Beijing
		Substrate_Network.AddEdge(3, 5);	//4:Shenyang<->Dalian
		Substrate_Network.AddEdge(4, 6);	//5:Beijing<->Tianjin
		Substrate_Network.AddEdge(6, 7);	//6:Tianjin<->Jinan
		Substrate_Network.AddEdge(4, 9);	//7:Beijing<->Zhengzhou
		Substrate_Network.AddEdge(4, 10);	//8:Beijing<->Xi'an
		Substrate_Network.AddEdge(8, 10);	//9:Lanzhou<->Xi'an
		Substrate_Network.AddEdge(7, 13);	//10:Jinan<->Shanghai
		Substrate_Network.AddEdge(11, 12);	//11:Nanjing<->Hefei
		Substrate_Network.AddEdge(11, 13);	//12:Nanjing<->Shanghai
		Substrate_Network.AddEdge(9, 16);	//13:Zhengzhou<->Wuhan
		Substrate_Network.AddEdge(10, 15);	//14:Xi'an<->Chengdu
		Substrate_Network.AddEdge(13, 14);	//15:Shanghai<->Hangzhou
		Substrate_Network.AddEdge(12, 16);	//16:Hefei<->Wuhan
		Substrate_Network.AddEdge(15, 17);	//17:Chengdu<->Chongqing
		Substrate_Network.AddEdge(16, 17);	//18:Wuhan<->Chongqing
		Substrate_Network.AddEdge(16, 18);	//19:Wuhan<->Changsha
		Substrate_Network.AddEdge(14, 19);	//20:Hangzhou<->Xiamen
		Substrate_Network.AddEdge(18, 20);	//21:Changsha<->Guangzhou
		Substrate_Network.AddEdge(19, 20);	//22:Xiamen<->Guangzhou
		Substrate_Network.AddEdge(20, 21);	//23:Guangzhou<->Shenzhen
	}
	else if (TypeofTopology == "Input")
	{
		ifstream topoinput;
		topoinput.open(TopoFilename.c_str());
		if (topoinput.fail())
		{
			cerr << "Topo input file opened failed.\n";
			MainError = -3;
			exit(1);
		}

		topoinput >> Substrate_Network;
		topoinput.close();
	}
	else
	{
		cerr << "Wrong argument name!\n";
		MainError = -2;
		exit(1);
	}

	string ResultFileName = "VONEover";
	if (HeterogeneousCoreIndex == 0)
		ResultFileName = ResultFileName + "MCF";
	else if (HeterogeneousCoreIndex > 0)
		ResultFileName = ResultFileName + "HMCF";
	else
	{
		cerr << "Index of Heterogeneous Core should be a possitive integer!\n";
		MainError = -4;
	}

	if ((CrosstalkYesorNo == "Yes") || (CrosstalkYesorNo == "yes"))
		ResultFileName = ResultFileName + "withIC-XT";
	else if ((CrosstalkYesorNo == "No") || (CrosstalkYesorNo == "no"))
		ResultFileName = ResultFileName + "withoutIC-XT";
	else
	{
		cerr << "Wrong CrosstalkYesorNo string, right string should be Yes, yes, No or no!\n";
		MainError = -5;
	}

	if (Solver == "ILP")
		ResultFileName = ResultFileName + "ILP";
	else if (Solver == "Heuristic")
		ResultFileName = ResultFileName + "Heuristic";
	else if (Solver == "ILPHeuristic")
		ResultFileName = ResultFileName + "ILPHeuristic";
	else
	{
		cerr << "Wrong Solver has been given, right Solver should be ILP, Heuristic or ILPHeuristic!\n";
		MainError = -6;
	}

	ofstream fout;
	ResultFileName = ResultFileName + to_string(ServiceRequestNumber) + "Result.txt";
	fout.open(ResultFileName.c_str()); //Use result file to record some information
	if (fout.fail())
	{
		cerr << "File " << ResultFileName << " opening failed.\n";
		MainError = -7;
		exit(1);
	}

	cout << Substrate_Network << endl;
	fout << Substrate_Network << endl;

	//Input the Virtual Network:
	vector<VONETopo::Topo> Virtual_Network_List;
	for (int i = 1; i <= ServiceRequestNumber; i++)
	{
		VONETopo::Topo VN_tmp;

		//int VN_Vertex = 2 + rand() % 6;
		int VN_Vertex = 2 + rand() % 3;	//4 Node
		for (int j = 1; j <= VN_Vertex; j++)
		{
			int VN_Vertex_CPU = 1 + rand() % 5;
			VN_tmp.AddVertex(j, VN_Vertex_CPU);
		}

		int VN_Demand = 3 + rand() % 3;
		int VN_Edge = VN_Vertex - 1 + rand() % (VN_Vertex * (VN_Vertex - 1) / 2 - (VN_Vertex - 1) + 1);
		int Used_Edge_Num = 1, Edge_Step = 1;
		bool end_flag = false;
		while (true)
		{
			int Vertex_Row = 1;
			int Vertex_Column = Vertex_Row + Edge_Step;
			while (Vertex_Column <= VN_Vertex)
			{
				VN_tmp.AddEdge(Vertex_Row, Vertex_Column, VN_Demand);
				Vertex_Column++;
				Vertex_Row++;
				Used_Edge_Num++;
				if (Used_Edge_Num > VN_Edge)
				{
					end_flag = true;
					break;
				}
			}
			if (end_flag)
				break;
			Edge_Step++;
		}

		Virtual_Network_List.push_back(VN_tmp);
	}

	for (int i = 1; i <= ServiceRequestNumber; i++)
	{
		cout << "The Virtual Request Topology " << i << " are following:\n";
		cout << Virtual_Network_List[i - 1] << endl;
		fout << "The Virtual Request Topology " << i << " are following:\n";
		fout << Virtual_Network_List[i - 1] << endl;
	}

	if (Solver == "ILP")
	{
		//Solve the ILP of VONE:
		VONEILP::VONECplex Solve_VONE_on_MCF_of_ILP;
		//Time count:
		clock_t start = clock();
		Solve_VONE_on_MCF_of_ILP.solve(Substrate_Network, Virtual_Network_List, 
			CoreNumber, HeterogeneousCoreIndex, CrosstalkYesorNo);
		clock_t end = clock();
		fout << "\nThe running time of ILP is: " << static_cast<double>(end - start) / CLOCKS_PER_SEC * 1000 << "ms\n\n";
		cout << "\nThe running time of ILP is: " << static_cast<double>(end - start) / CLOCKS_PER_SEC * 1000 << "ms\n\n";
		fout << Solve_VONE_on_MCF_of_ILP << endl;
		cout << Solve_VONE_on_MCF_of_ILP << endl;
	}		
	else if (Solver == "Heuristic")
	{
		//Solve the Heuristic of VONE:
		VONEHeuristic::VONEHeuristic Solve_VONE_on_MCF_of_Heuristic;
		//Time count:
		clock_t start = clock();
		Solve_VONE_on_MCF_of_Heuristic.solve(Substrate_Network, Virtual_Network_List, 
			CoreNumber, HeterogeneousCoreIndex, CrosstalkYesorNo);
		clock_t end = clock();
		fout << "The running time of Heuristic is: " << static_cast<double>(end - start) / CLOCKS_PER_SEC * 1000 << "ms\n\n";
		cout << "The running time of Heuristic is: " << static_cast<double>(end - start) / CLOCKS_PER_SEC * 1000 << "ms\n\n";
		fout << Solve_VONE_on_MCF_of_Heuristic << endl;
		cout << Solve_VONE_on_MCF_of_Heuristic << endl;		
	}
	else if (Solver == "ILPHeuristic")
	{
		//The process above will take a lot time to solve VONE, but the heuristic will sort the
		//    request in some kind of order, this should not be taken into account in ILP nodel.
		//Solve the ILP of VONE:
		VONEILP::VONECplex Solve_VONE_on_MCF_of_ILP;
		//Time count:
		clock_t ILP_start = clock();
		Solve_VONE_on_MCF_of_ILP.solve(Substrate_Network, Virtual_Network_List,
			CoreNumber, HeterogeneousCoreIndex, CrosstalkYesorNo);
		clock_t ILP_end = clock();
		fout << "\nThe running time of ILP is: " << static_cast<double>(ILP_end - ILP_start) / CLOCKS_PER_SEC * 1000 << "ms\n\n";
		cout << "\nThe running time of ILP is: " << static_cast<double>(ILP_end - ILP_start) / CLOCKS_PER_SEC * 1000 << "ms\n\n";
		fout << Solve_VONE_on_MCF_of_ILP << endl;
		cout << Solve_VONE_on_MCF_of_ILP << endl;

		//Solve the Heuristic of VONE:
		VONEHeuristic::VONEHeuristic Solve_VONE_on_MCF_of_Heuristic;
		//Time count:
		clock_t Heuristic_start = clock();
		Solve_VONE_on_MCF_of_Heuristic.solve(Substrate_Network, Virtual_Network_List,
			CoreNumber, HeterogeneousCoreIndex, CrosstalkYesorNo);
		clock_t Heuristic_end = clock();
		fout << "The running time of Heuristic is: " << static_cast<double>(Heuristic_end - Heuristic_start) / CLOCKS_PER_SEC * 1000 << "ms\n\n";
		cout << "The running time of Heuristic is: " << static_cast<double>(Heuristic_end - Heuristic_start) / CLOCKS_PER_SEC * 1000 << "ms\n\n";
		fout << Solve_VONE_on_MCF_of_Heuristic << endl;
		cout << Solve_VONE_on_MCF_of_Heuristic << endl;
	}
	else
	{
		cerr << "Wrong \n";
		MainError = -6;
	}

	fout.close();
	return MainError;
}

namespace
{
	static void Usage(const char* ProgramName)
	{
		cerr << "\nUsage: " << ProgramName << " -help/-h to elaborate the reference of the program. \n";

		cerr << "\nRight usage of " << ProgramName << " is like the following: \n";
		cerr << ProgramName << " -topo/-t 'SixNode' <Name of the Topology File> -rnum/-r 30 -cnum/-c 6 "
			 << "-hetercindex/-h 0 -crosstalk/-x 'No' -solver/-s 'ILP'";

		cerr << "\nWhere argument -topo/-t <Type of the Topology> <Name of the Topology File> is a string "
			 << "to describe the file of the substrate topology. \n";
		cerr << "<Type of the Topology> could be one of the strings named 'SixNode', 'FITI' or 'Input', \n"
			 << "where 'SixNode' describes the simple six-node experimental topology often using for solving ILP models "
			 << " and then comparing with the heuristic algorithms, \n"
			 << "where 'FITI' describes the modified real FITI topology built in China to test some metrics of heuristic "
			 << "algorithms over a large scale topology, \n"
			 << "where 'Input' indicates that the topology is described by a topology input file <Name of the Topology File>, \n"
			 << "<Name of the Topology File> is the file name path of the topology. When <Type of the Topology> is 'Input', "
			 << "the argument works, ortherwise, it will be ignored. \n";

		cerr << "\nWhere -rnum/-r <Number of Service Requests> is an integer to indicate the number of servcie requests. \n";
		
		cerr << "\nWhere -cnum/-c <Number of Cores> is an integer to indicate the total number of cores in a fiber. \n";

		cerr << "\nWhere -hetercindex/-h <Index of Heterogeneous Core> is an integer to indicate the index of the heterogeneous core, "
			<< "0 represents that all cores are homogeneous. \n";

		cerr << "\nWhere -crosstalk/-x <Name of the Traffic File> is a string to describe wether the crosstalk should be taken into account or not.\n";
		
		cerr << "\nWhere -solver/-s <Solving Method of the VONE problem> is a string to indicate the solving method of the VONE problem. \n"
			 << "<Solving Method of the VONE problem> could be one of the strings called 'ILP', 'Heuristic' or 'ILPHeuristic', \n"
			 << "where 'ILP' solves an ILP model by branch and bround to get the mathematical accurate solutions, \n"
			 << "where 'Heuristic' executes the proposed heuristic algorithms to get approximate solutions, \n"
			 << "where 'ILPHeuristic' implements the ILP method firstly then the heuristic method because the heuristic method may "
			 << "change the order of service requests. \n"
			 << "Note that the ILP method may take along time to solve, this argument is suitable for small topology and a few number of service requests. \n";
		
		cerr << "\nThe arguments above must be enough and not too much, or the program will be exit with error code -1." << endl;
	}
}