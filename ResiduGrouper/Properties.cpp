/*  Properties.cpp
 *  ResidueGrouper
 *  Created by brian on 9/2/09.
 *
 *  Properties are taken from references noted below.  The property profile follows that used in the MAPP application,
 *  Stone & Sidow, Genome Research, 15:978-986, 2005.  More detailed reference info can be found there.
 *
 * USAGE: Initalization to normalized mode is triggered from constructor (metrics are normalized from -1 to 1).  
 * Natural metric values can be used by calling initMetrics() with an argument of 0.
 * Calling the fingerprint function with a specified residue string (single letter amino acid abbreviation, capitalized) provides the residues property values.
 */

#include "Properties.h"

const int propCount = 3;//the number of properties in the propertyMatrix - UPGRADE to allow dyanmic metric count
vector<string> propertyNames;
vector<double> propertyValues;


/** Initializes property metric maps.  Uses normalized metrics by default. */
Properties::Properties(){
	initMetrics(1);//normalized
}


/** Initializes property metric maps. */
Properties::Properties(int norm){
	initMetrics(norm);
}


////////////// PUBLIC FUNCTIONS  ////////////////

/** Returns the property values assigned to the residue. */
vector<double> Properties::fingerprint(string r){
	residueName = r;
	populatePropertyValues();
	return propertyValues;
}


/** Sets the normalized flag and delegates to set/reset the property maps. */
void Properties::initMetrics(int norm){
	normalized = norm;
	if(normalized == 1){
		initNormalizedPropertyMaps();
		cout << "Init Properties: normalized mode\n";
	}else{
		initPropertyMaps();
		cout << "Init Properties: natural mode\n";
	}
}


/** Print the metric values to the command line. */
void Properties::printPropertyValues(){
	cout << residueName << "\t" << pmapCharge[residueName] << "\t" << pmapVolume[residueName] << "\t" << pmapPolarity[residueName] << endl;
}


/** Print the metric names to the command line. */
void Properties::printPropertyNames(){
	cout << "\t\t\t\tCharge \t Volume \t Polarity" << endl;
}


/** Returns the number of properties held for each residue. */
int Properties::getPropertyCount(){
	return propCount;
	//return propertyValues.size();
}


/** Returns the property values assigned to the residue. */
vector<double> Properties::getPropertyValues(){
	return propertyValues;
}


/** Returns the names of the properties assigned to each residue. */
vector<string> Properties::getPropertyNames(){
	return propertyNames;
}


/* Print property data to command line.  UNSUPPORTED*/
void Properties::print(){
	cout << "\nPhysiochemical properties for: "<< residueName <<" \n";
	//for(int i=0; i<seqNames.size(); i++){
	//	cout << seqNames[i] << "\n";
	//	for(int j=0; j<seqMatrix[i].size(); j++){
	//		cout<< seqMatrix[i][j];			
	//	}
	//	cout << "\n";
	//}
}



//////// PRIVATE FUNCTIONS  ////////////
/** Assign properties to the propertyValues vector. */
void Properties::populatePropertyValues(){
	propertyValues.clear();
	propertyValues.push_back(pmapCharge[residueName]);
	propertyValues.push_back(pmapVolume[residueName]);
	propertyValues.push_back(pmapPolarity[residueName]);
}


/** Read natural property values into the property maps. */
void Properties::initPropertyMaps(){
	string aa[] = {"A","C","D","E","F","G","H","I","K","L","M","N","P","Q","R","S","T","V","W","Y"};
	int aaLength = sizeof(aa)/sizeof(aa[0]);//20 - unless something unexpected changes...
	
	//charge
	pmapCharge.clear();
	for(int i=0; i<aaLength; i++){//read values into the map
		residueName = aa[i];
		pmapCharge[residueName] = readCharge();
	}
	
	//volume
	pmapVolume.clear();
	for(int i=0; i<aaLength; i++){//read values into the map
		residueName = aa[i];
		pmapVolume[residueName] = readVolume();
	}
	
	//polarity
	pmapPolarity.clear();
	for(int i=0; i<aaLength; i++){//read values into the map
		residueName = aa[i];
		pmapPolarity[residueName] = readPolarity();
	}
}


/** Compute normalized property values from -1 to 1 and put them in the property maps. */
void Properties::initNormalizedPropertyMaps(){
	string aa[] = {"A","C","D","E","F","G","H","I","K","L","M","N","P","Q","R","S","T","V","W","Y"};
	int aaLength = sizeof(aa)/sizeof(aa[0]);//20 - unless something unexpected changes...
	double max = DBL_MIN, min = DBL_MAX, range, mean;
	
	//charge
	max = DBL_MIN, min = DBL_MAX;
	pmapCharge.clear();
	for(int i=0; i<aaLength; i++){//get min & max and map raw charge
		residueName = aa[i];
		pmapCharge[residueName] = readCharge();
		if (readCharge() > max) max = readCharge();
		if (readCharge() < min) min = readCharge();
	}
	range = max - min;
	mean = min + range / 2;
	for(int i=0; i<aaLength; i++){//compute and map normalized charge
		residueName = aa[i];
		pmapCharge[residueName] = ( pmapCharge[residueName] - mean ) * 2 / range;		
	}
	
	//volume
	max = DBL_MIN, min = DBL_MAX;
	pmapVolume.clear();
	for(int i=0; i<aaLength; i++){//get min & max and map raw volume
		residueName = aa[i];
		pmapVolume[residueName] = readVolume();
		if (readVolume() > max) max = readVolume();
		if (readVolume() < min) min = readVolume();
	}
	range = max - min;
	mean = min + range / 2;	
	for(int i=0; i<aaLength; i++){//compute and map normalized volume
		residueName = aa[i];
		pmapVolume[residueName] = ( pmapVolume[residueName] - mean ) * 2 / range;		
	}
	
	//polarity
	max = DBL_MIN, min = DBL_MAX;
	pmapPolarity.clear();
	for(int i=0; i<aaLength; i++){//get min & max and map raw polarity
		residueName = aa[i];
		pmapPolarity[residueName] = readPolarity();
		if (readPolarity() > max) max = readPolarity();
		if (readPolarity() < min) min = readPolarity();	
	}
	range = max - min;
	mean = min + range / 2;
	for(int i=0; i<aaLength; i++){//compute and map normalized polarity
		residueName = aa[i];
		pmapPolarity[residueName] = ( pmapPolarity[residueName] - mean ) * 2 / range;		
	}	
}


///////////// natural property values taken from literature - functions used only to initiate the property maps  ////////////////////
/** pI - isoelectric point - from wikipedia (need different version of Stryer to match MAPP ref) */
double Properties::readCharge(){	
	if (residueName=="A") {return 6.01;
	}else if (residueName=="C") {return 5.05;
	}else if (residueName=="D") {return 2.85;
	}else if (residueName=="E") {return 3.15;
	}else if (residueName=="F") {return 5.49;
	}else if (residueName=="G") {return 6.06;
	}else if (residueName=="H") {return 7.60;
	}else if (residueName=="I") {return 6.05;
	}else if (residueName=="K") {return 9.60;
	}else if (residueName=="L") {return 6.01;
	}else if (residueName=="M") {return 5.74;
	}else if (residueName=="N") {return 5.41;
	}else if (residueName=="P") {return 6.30;
	}else if (residueName=="Q") {return 5.65;
	}else if (residueName=="R") {return 10.76;
	}else if (residueName=="S") {return 5.68;
	}else if (residueName=="T") {return 5.60;
	}else if (residueName=="V") {return 6.00;
	}else if (residueName=="W") {return 5.89;
	}else if (residueName=="Y") {return 5.64;
	}else{return INFINITY;}//NONSENSE pI
}



/** side chain volume - Zamyatnin, 1972 - experimental molar volume (ml/mol) */
double Properties::readVolume(){
	if (residueName=="G") {return 43.5;
	}else if (residueName=="A") {return 60.6;
	}else if (residueName=="V") {return 91.3;
	}else if (residueName=="L"){return 107.5;
	}else if (residueName=="I"){return 107.5;
	}else if (residueName=="F") {return 121.3;
	}else if (residueName=="P") {return 81.0;
	}else if (residueName=="W") {return 144.1;
	}else if (residueName=="S") {return 60.8;
	}else if (residueName=="T") {return 77.1;
	}else if (residueName=="M") {return 105.1;
	}else if (residueName=="C") {return 72.5;
	}else if (residueName=="N") {return 78.0;
	}else if (residueName=="Q") {return 93.9;
	}else if (residueName=="D") {return 74.1;
	}else if (residueName=="E") {return 90.4;
	}else if (residueName=="Y") {return 123.6;
	}else if (residueName=="H") {return 99.3;
	}else if (residueName=="K") {return 108.5;
	}else if (residueName=="R") {return 111.5;
	}else{return INFINITY;}//nonsense volume
	
}


/** polarity - from Stryer*/
double Properties::readPolarity(){
	if (residueName=="F") {return 3.7;	
	}else if (residueName=="M") {return 3.4;
	}else if (residueName=="I") {return 3.1;
	}else if (residueName=="L") {return 2.8;
	}else if (residueName=="V") {return 2.6;
	}else if (residueName=="C") {return 2.0;
	}else if (residueName=="W") {return 1.9;
	}else if (residueName=="A") {return 1.6;
	}else if (residueName=="T") {return 1.2;
	}else if (residueName=="G") {return 1.0;
	}else if (residueName=="S") {return 0.6;
	}else if (residueName=="P") {return -0.2;
	}else if (residueName=="Y") {return -0.7;
	}else if (residueName=="H") {return -3.0;
	}else if (residueName=="Q") {return -4.1;
	}else if (residueName=="N") {return -4.8;
	}else if (residueName=="E") {return -8.2;
	}else if (residueName=="K") {return -8.8;
	}else if (residueName=="D") {return -9.2;
	}else if (residueName=="R") {return -12.3;
	}else{return INFINITY;}//nonsense value
}

