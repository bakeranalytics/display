/*
 *  Distribution.h
 *  ResidueGrouper
 *
 *  Created by brian on 2/23/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include <iostream>
#include <vector>
using namespace std;

class Distribution{
public:		
	Distribution();
	~Distribution(){}
	
	void addCount(const string &s, int p);
	
private:
	vector<string> aaType;//amino acid symbols; loaded with initial 20 up front, additional symbols added as encountered
	//vector<vector<int>> aaCount;//the number of amino acids of the corresponding type found at a given position
	
};

//-- what is the best way to build the matrix?? - vector of vectors???