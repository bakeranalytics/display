#pragma once

#include "Simulant.h"

using namespace System;


// a simulated player that trains in a square-centric manner.  
// a 'square' is the set of rows & columns a given distance from the board center
// example: square 0 is filled before play begins, it holds only 4 places, and 
// square 7 represents the board edges, it holds 28 places
class SquareSimulant : public Simulant{
public:	SquareSimulant(void);
public:	virtual ~SquareSimulant(void);

	 void newBrainMutation(Simulant & const sim, int seed);//new mutation procedure

protected:
private: 

};
