/*
 *  SubMatrix.h
 *  ResidueGrouper
 *
 *  Created by brian on 1/1/10.
 */


#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <cmath>
#include <cstring>
#include <sstream> 
#include <stdio.h>
#include <stdlib.h>
using namespace std;

class SubMatrix{
public:		
	SubMatrix();
	~SubMatrix(){};
	
	double distance(const string r1, const string r2);	
	void printMatrixValues();
	
	double getMax();//returns the maximum cell value from the matrix
	double getMin();//returns the minimum cell value from the matrix

	void loadMatrix(const string file);
		
private:	
	string residueName;
	map <string, double> pmapSubMatrix;
	vector <string> aa;
	void normalize();
	
	
};