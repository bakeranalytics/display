/*  ResidueGrouper.cpp
 *  ResidueGrouper
 *  Created by brian on 8/27/09. */

#include "Alignment.h"
#include <dirent.h>
#include <errno.h>

int main (int argc, char * const argv[]) {	
	if(argv[1] == '\0'){//if no alignment file was provided, explain usage
		cout << "Residue Grouper::\n";
		cout << "Usage:: Residue Grouper <ALIGNMENT FILE> <POSITION{optional}>\n";
		cout <<"USAGE:: to compute pchem-phylo relationships across multiple alignments: ResidueGrouper -d <<export file name>>"<<endl;
		cout <<"USAGE:: to compute Valdar conservation for one alignment: ResidueGrouper -v <<alignment file name>> <<position>>   ..  processes all .afa files to vldr.txt if no file specified and runs RxR motif search"<<endl;
		cout <<"USAGE:: to compute Valdar conservation for one alignment: ResidueGrouper -va   ..  processes all .afa files to vldr.txt and runs RxR motif search writing all sequences to the file"<<endl;
		return 1;
	}
	
	
	if(argv[1][0] == '!'){//test
		//Phylogeny tree;
		//tree.printTree();
		//cout << "Residue Grouper test mode::" <<"\n";
		//cout << INFINITY <<"\n";
		
		Alignment alignment("SRC.afa");
		return 2;
	}
	
	
	////////////////// VALDAR METRIC, single file  ///////////////////
	if(argv[1][0] == '-' && argv[1][1] == 'v'){//flag of -v will run the Valdar conservation analysis
		int r=0;//sequence position
		
		if(argv[2] == '\0'){//no input file name was provided -> run on all .afa files in the directory
			string exportFileName = "full";
			string vldrFileName (exportFileName);
			vldrFileName += ".vld";
			remove( vldrFileName.c_str() );//remove old copy
			string mtfFileName (exportFileName);
			mtfFileName += ".RxRxxS-T.mtf";
			remove( mtfFileName.c_str() );//remove old copy 
			
			DIR *dp;
			struct dirent *dirp;
			string file;
			
			if((dp  = opendir("./")) == NULL) {
				cout << "Error(" << errno << ") reading file list." << endl;
				return errno;
			}
			
			while ((dirp = readdir(dp)) != NULL) {
				file = dirp->d_name;
				string::size_type flag;
				flag = file.find(".afa");//grab all files with .afa
				if(flag != string::npos){
					flag = file.find(".afa.");
					if(flag == string::npos){//skip all files with .afa.
						Alignment a(file);
						//cout<< dirp->d_name<<endl;
						a.conservationScores(exportFileName);
						//a.motifSearch(1);//motif scores to file
						if(argv[1][2] == 'a'){//writes all sequences
							a.motifSearch("full", 1);				 
						}else{
							a.motifSearch("full", 0);			
						}
					}
				}
			}
			closedir(dp);	
		}else{//single file run
			Alignment alignment(argv[2]);
			if(argv[3] != '\0'){//position was provided - analyze only that position
				r = atoi(argv[3]) - 1;//the sequence position being queried, index 0 corresponds to position 1
				cout <<"Position: "<< r+1 <<"\n";
				cout << "score at "<< r << ": "<<alignment.conservationScore(r)<<endl;
				cout << "score at "<< r << ": "<<alignment.conservationScoreDigit(r)<<endl;//test
			}else{//analyze all positions
				string vldrFileName (argv[2]);
				vldrFileName += ".vld";
				remove( vldrFileName.c_str() );//remove old copy
				string mtfFileName (argv[2]);
				mtfFileName += ".RxRxxS-T.mtf";
				remove( mtfFileName.c_str() );//remove old copy 
				
				alignment.conservationScores(argv[2]);
				//alignment.motifSearch(argv[2], 1);
				if(argv[1][2] == 'a'){//writes all sequences & rounds scores
					alignment.motifSearch(argv[2], 1);				 
				}else{//srite human sequence only & unrounded scores
					alignment.motifSearch(argv[2], 0);				
				}
				
				//alignment.printAlignment();//test
			}
		}
	}
	
	/////////////////////////  MUTIPLE ALIGNMENT FILES   ///////////////////////
	// writes the pchem - phylo coordinates of all .afa file in the directory into a file named argv[2].pvp
	// the file is NOT truncated before it begins appending data
	else if(argv[1][0] == '-' && argv[1][1] == 'd'){//flag of -d will analyze all sequences in the directory
		if(argv[2] == '\0'){//no output file name was provided 
			cout <<"USAGE:: to compute pchem-phylo relationships across multiple alignments: ResidueGrouper -d <<export file name>>"<<endl;
		}
		string exportFileName = argv[2];
		DIR *dp;
		struct dirent *dirp;
		string file;
		
		if((dp  = opendir("./")) == NULL) {
			cout << "Error(" << errno << ") reading file list." << endl;
			return errno;
		}
		
		while ((dirp = readdir(dp)) != NULL) {
			file = dirp->d_name;
			string::size_type flag;
			flag = file.find(".afa");
			if(flag != string::npos){
				flag = file.find(".afa.");
				if(flag == string::npos){
					Alignment a(file);//this creates a new alignment & triggers recompute of properties & phylogeny for each alignment file.  the alignment class could be modified to accept a properties & phylogeny class as constructor arguments to improve performance.
					cout<< dirp->d_name<<endl;
					a.exportPchemPhylCoordinates(exportFileName);
				}
			}
		}
		closedir(dp);
	}else{/////////////////////////  SINGLE ALIGNMENT FILE   ///////////////////////
		// writes the pchem - phylo coordinates of a specified .afa file (argv[1]) in the directory into a file named argv[1].pvp
		string fileName(argv[1]);
		Alignment alignment(argv[1]);
		alignment.exportPchemPhylCoordinates();
		
		if(argv[2] != '\0'){//position was provided - analyze only that position
			int r = atoi(argv[2]) - 1;//the sequence position being queried, index 0 corresponds to position 1
			cout <<"Position: "<< r+1 <<"\n";
			alignment.printPchemDistanceMatrix(r);
			alignment.printPhylogenicDistanceMatrix(r);
			alignment.printPchemPhylCoordinates(r);
			
			//alignment.computeSD(r);
			//alignment.printPropMap(r);
		}else{//analyze entire alignment
			//cout <<"Sequence scan " <<"\n";
			//alignment.spreadScan();
			
			//alignment.computeComparisonMatrixes();
		}
		
		//alignment.printSummary();
		//alignment.printAlignment();
	}
		
	
    return 0;
}

