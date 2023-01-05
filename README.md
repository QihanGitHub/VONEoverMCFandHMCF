# VONEoverMCFandHMCF
In the paper in **Documentation** folder, the authors developed four Integer Linear Programming (ILP) models and heuristic algorithms to minimize the index of frequency slots occupied when the virtual optical networks are embedded over an elastic optical network with homogeneous or heterogeneous multi-core fibers. The four ILP models of the paper have been implemented in C++ language in the source codes **ILP.h** and **ILP.cpp** in **VONEoverMCFandHMCF** folder. The optimization solver software employed here is IBM ILOG CPLEX Optimization Studio 12.8 (CPLEX). Since CPLEX is a commercial software with the copyright of IBM, although the open source codes include some classes or functions from CPLEX, the usage of them and the compilation and link method rather than any head files or libraries of CPLEX are only given. For a free edition of CPLEX or more details information about CPLEX, please visit their official site <https://www.ibm.com/products/ilog-cplex-optimization-studio>.

The four heuristic algorithms of the paper have also been implemented in the same programming language in the source codes **Heuristic.h** and **Heuristic.cpp** in **VONEoverMCFandHMCF** folder.

An executable version named **VONEonMCFandHMCF.exe** in **VONEoverMCFandHMCF** folder for the 64-bit Windows 10 platform has been built. Followers can execute it directly in the Windows "cmd" or "PowerShell" terminal. The usage of the arguments for the executable file could be obtained by running the command: 
`VONEonMCFandHMCF.exe -h`
or `VONEonMCFandHMCF.exe -help` in the terminal.

When the argument `-topo/-t` is `'Input'`, the running of the executable file will need an input file that describes the topology of the substrate network. The file is a txt file with the same format as the document in <https://github.com/QihanGitHub/TDMWDMOFDMoverMCF/tree/master/Documentation> lists. "xDMN6S8.txt" or "xDMNSFNET.txt" in <https://github.com/QihanGitHub/TDMWDMOFDMoverMCF/tree/master/TDMWDMOFDMonMCF> could be taken as an example.

After the execution, if the argument `-solver/-s` is `'ILP'` or `'ILPHeuristic'`, the result files will include the model "\*.lp" file called "VONEoverMCF\<MCF/HMCF\>\<with/without\>\<The number of service requests\>.lp" and the result "\*.txt" file called "VONEover\<MCF/HMCF\>\<with/without\>IC-XT\<ILP/Heuristic/ILPHeuristic\>\<The number of service requests\>Result.txt" will be saved in the same folder of the executable file. If the argument `-solver/-s` is `'Heuristic'`, the result file will only include the result "\*.txt" file with the filename abovementioned. In addition, solving ILP is very time-consuming, it may take a long time to wait for a big number of service requests, even encountering the out-of-memory error.

Finally, when rebuilding the open source codes in VSCode, the CPLEX include paths or library paths in **tasks.json** file in **VONEoverMCFandHMCF/.vscode** folder may be different because of the different options during the software installation. The file gives the default CPLEX installation paths and they should be carefully checked and replaced by the right paths when rebuilding.

For more information about this work, please refer to the corresponding folder.

## Citation
If this work helps you, please cite our paper *Crosstalk-Avoid Virtual Optical Network Embedding Over Elastic Optical Networks With Heterogeneous Multi-Core Fibers*. The *bib* file format is following:
```
@article{Q.Zhang2022,
   author = {Q. Zhang and X. Zhang and X. Gong and L. Guo},
   title = {Crosstalk-Avoid Virtual Optical Network Embedding Over Elastic Optical Networks With Heterogeneous Multi-Core Fibers},
   journal = {Journal of Lightwave Technology},
   volume = {40},
   number = {24},
   pages = {7687-7700},
   year = {2022},
   doi = {10.1109/JLT.2022.3203861}
}
```