/*
 *  Phylogeny.h
 *  ResidueGrouper
 *
 *  Created by brian on 10/9/09.
 *  Copyright 2009 __Salk Institute__. All rights reserved.
 *
 */


#include <vector>
#include <map>
#include <cmath>
#include "tnt.h"
using namespace std;
using namespace TNT;
#define dim 14//the number of species in the tree

class Phylogeny{
public:	
	Phylogeny();
	~Phylogeny(){};
	
	vector<double> treeAverages;//Average distance from all other species
	vector<int> excludedSpecies;//index corresponds to tree species index; value of 0 means do not exclude from average computation, other values flags exclusion of species
	double valdarNorm;//normalization factor used in Valdar conservation score - uses all species in the tree -> partial species alignments should use Alignment.valdarNorm

	
	/** 
		@return the distance between the two species mapped to the input parameters.
		@param r maps to one of the two species whose relationship is being queried.
 		@param s maps to one of the two species whose relationship is being queried.
	 */
	double getDistance(int r, int s);
	void printTree();//print the tree matrix to the command line
	
	int getTreeNumber(const string& s);
	void excludeSpecies(int i);//flags a species for exclusion from average computation & triggers recalculation of averages
	int includeSpecies(const string& s);//flags species for inclusion in averaging; returns treeNum index
	void computeTreeAverages();//must be called by Alignment after including &/or excluding species

	
private:
	vector<string> treeNumberVec;//Numeric ID for each species corresponding to its position in the tree distance matrix	
	void loadTree();//original, degenerate values
	void loadModifiedTree();//use this for simulated annealing
	void loadTreeVector();//mapping of species name to order in the data
	void shiftTree();//shifts the phonogenic distances by a specified amount for each species	
	double phylogenicDistance[dim][dim];//(3, 3, -1);//distance between all points, initialized to -1 as flag for 'not set'
	//To use the phylogenic distances the sequence file must be edited to replace the names by these associate numbers.  
	//Unmatched sequences must be removed.
	//0 human
	//1 chimp
	//2 monkey, rhe
	//3 mouse
	//4 rat
	//5 rabit
	//6 dog
	//7 cow
	//8 opossum
	//9 chicken
	//10 frog
	//11 zebrafish
	//12 pufferfish
	//13 tretraodo
	
	
};

