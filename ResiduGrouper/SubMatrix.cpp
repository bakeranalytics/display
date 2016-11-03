/*
 *  SubMatrix.cpp
 *  ResidueGrouper
 *
 *  Created by brian on 1/1/10.
 *  Copyright 2010 Salk. All rights reserved.
 *
 */

#include "SubMatrix.h"



SubMatrix::SubMatrix(){
	string blosumFile = "blosum62.txt";//(http://www.ncbi.nlm.nih.gov/Class/FieldGuide/BLOSUM62.txt)
	//string petFile = "pet91.txt"; //The rapid generation of mutation data matrices from protein sequence, Jones, et. al
	string petFile = "pet91_ST.txt"; //The rapid generation of mutation data matrices from protein sequence, Jones, et. al but modified to make S<>T substitutions conserved (used for RxRxxS/T motif)
	
	loadMatrix(petFile);
	normalize();//used with valdar score
	//printMatrixValues();//test
}


double SubMatrix::distance(const string r1, const string r2){//returns the blosum distance between the two sp << endlecified amino acids
	string pair = r1;
	pair.append(r2);
	return pmapSubMatrix[pair];
}



/** Returns the largest matrix value.*/
double SubMatrix::getMax(){
	double biggest = -INFINITY;
	for(int i=0; i<aa.size(); i++){
		for(int j=0; j<aa.size(); j++){
			residueName = aa[i];
			residueName.append(aa[j]);
			if (biggest < pmapSubMatrix[residueName]){
				biggest = pmapSubMatrix[residueName];
			}
		}
	}
	return biggest;
}


/** Returns the smallest matrix value.*/
double SubMatrix::getMin(){
	double smallest = INFINITY;
	for(int i=0; i<aa.size(); i++){
		for(int j=0; j<aa.size(); j++){
			residueName = aa[i];
			residueName.append(aa[j]);
			if (smallest > pmapSubMatrix[residueName]){
				smallest = pmapSubMatrix[residueName];
			}
		}
	}	
	return smallest;
}


void SubMatrix::loadMatrix(const string file){
	char line[1000];//temp - input line from file
	int c = 0, r=0;//counter
	int firstLine = 1;
	string temp;
	string tempa;
	char *p;
	int value;
	string splitter = "\t ";//delimetrs for tokenizing
	
	cout << "import substitution matrix...\n";
	pmapSubMatrix.clear();
	
	//TODO genralize this to an input string taken from RG
	ifstream in(file.c_str(), ios::in);//The rapid generation of mutation data matrices from protein sequence, Jones, et. al
	if( ! in ){
		cerr << "Unable to open " << file;
		exit(4);
	}
	while(!(in.eof())){ 
		in.getline(line, 1000, '\n');
		
		if(firstLine){//get residue codes		
			stringstream residues(line);
			while (residues >> temp){
				aa.push_back(temp);
			}
			c = 0;
			firstLine = 0;			
		}else{//data line
			//cout << r <<":  ";//test
			c = 0;
			p=strtok (line,splitter.c_str());
			p=strtok(NULL,splitter.c_str());//skip first column
			while (p!=NULL){			
				residueName = aa[r]; 
				tempa = aa[c++];
				residueName.append(tempa);
				//cout << p << " ";
				value = atoi(p);
				p=strtok(NULL,splitter.c_str());				
				pmapSubMatrix[residueName] = value;
			}
			r++;
		}	
	}
}


/** Normalized according to Valdar's metric.  (a[i][j] - min) / (max - min)*/
void SubMatrix::normalize(){
	//get substitution matrix max, min, range
	double matMax = getMax();
	double matMin = getMin();
	double matRange = matMax - matMin;
	
	//replace matrix values with normalized values
	for(int i=0; i<aa.size(); i++){
		for(int j=0; j<aa.size(); j++){
			residueName = aa[i];
			residueName.append(aa[j]);
			pmapSubMatrix[residueName] = (pmapSubMatrix[residueName] - matMin) / matRange;		
		}
	}
	
}


void SubMatrix::printMatrixValues(){	
	cout << "Substitution matrix: " << endl;
	int aaLength = aa.size();
	
	//print header row
	cout  << "\t";
	for(int i=0; i<aaLength; i++){
		cout << aa[i] << "\t\t";
	}
	cout <<endl;
	
	for(int i=0; i<aaLength; i++){
		cout << aa[i] << "\t";
		for(int j=0; j<aaLength; j++){
			residueName = aa[i];
			residueName.append(aa[j]);
			cout  << pmapSubMatrix[residueName] << "\t";
			if( pmapSubMatrix[residueName] == 1){
				cout  << "\t";
			}
		}
		cout << endl;
	}
}
