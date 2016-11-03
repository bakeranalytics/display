/*
 *  Distribution.cpp
 *  ResidueGrouper
 *
 *  Created by brian on 2/23/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */


#include "Distribution.h"


Distribution::Distribution(){
	aaType.resize(20);
	//aaCount.resize(20);
	
	//load initial set of amino acids
	string aa[] = {"A","C","D","E","F","G","H","I","K","L","M","N","P","Q","R","S","T","V","W","Y"};	for(int i=0; i<20; i++){
		aaType[i] = aa[i];
		//cout << aatype[i];//test
	}
}


/** Adds a count of type s at position p. */
void Distribution::addCount(const string &s, int p){
	
}