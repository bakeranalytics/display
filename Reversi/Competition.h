#pragma once
#include "Simulant.h"
#include "Game.h"
#include <vector>
#include <string>

using namespace std;

class Competition{
public:
	Competition(void);
	virtual ~Competition(void);

	void competeSims(const string* simNames);
	void competeSimSets(const string* simDirs);
	void competeSimSetToMaxFlip(const string* simDirs);
	void competeSimSetToUniformWeights(const string* simDirs);

private:
	bool quiet;//if true output is 'production' only; if false, output gives more tracking info

};
