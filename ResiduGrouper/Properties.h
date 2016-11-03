/*
 *  Properties.h
 *  ResidueGrouper
 *  Created by brian on 9/2/09. 
 *	Assigns physiochemical properties to a residue type.
 */

#include <iostream>
#include <vector>
#include <map>
#include <cstring>
#include <cmath>
#include <cfloat>
using namespace std;

class Properties{
public:		
	Properties();
	Properties(int norm);
	~Properties(){};
	//Properties(const Properties& p){};//  - default copy constructor is fine for now
	
	string residueName;
	int normalized;//1 to use normalized metrics, 0 to use natural metrics - normalized is default
	
	vector<double> fingerprint(const string r);//returns the properties of the specified amino acid
	void initMetrics(int norm);//sets the normalized flag to the input value and repopulates the property maps accordingly
	
	vector<double> getPropertyValues();
	vector<string> getPropertyNames();
	int getPropertyCount();//returns length of propertyValues matrix
	void print();//unsupported for now
	void printPropertyValues();
	void printPropertyNames();
	
private:
	void populatePropertyValues();
	
	//property maps
	void initPropertyMaps();
	void initNormalizedPropertyMaps();
	map <string, double> pmapCharge;
	map <string, double> pmapVolume;
	map <string, double> pmapPolarity;
		
	/////// these functions are used only to poplulate the property maps
	double readCharge();
	double readVolume();
	double readPolarity();
	
};
