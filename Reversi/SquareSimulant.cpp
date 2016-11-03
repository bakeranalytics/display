#include "StdAfx.h"
#include "SquareSimulant.h"
#include <stdlib.h>

using namespace System;
using namespace System::IO;
//using namespace System::String;


SquareSimulant::SquareSimulant(void)
{
	id = -1;//uninitialized flag
	uniformWeights = false;//defaults to indiviual weight values
	//Console::WriteLine(L"new sim: "+polarity);//test//
}

SquareSimulant::~SquareSimulant(void)
{
}


//randomly changes the values of each scoringMatrix value according to the following algorithm:
//randomly choose one square to deemphasize and one to emphasize, the rest will be normally adjusted
//about the same average weight.
//-normal adjustment: set the new value n to a randomly selected value, Gaussianly distributed around the old value
//-emphasized: set the new value to n+(1-n)/a (n as defined above); this sets the value to 1/a of the way between n annd 1
//-deemphasized: set the new value to n/b (n as defined above)
// the parameters a & b are randomly selected between ...
void SquareSimulant::newBrainMutation(Simulant & const sim, int seed){

	//replace the code below (from copied from Simulant) with the new mutaion form described above

	srand(seed);
	double r;//temp random number used for deciding when to mutate
	double m = 0.1;//10% mutation rate
	//Console::WriteLine("new brain from mutation of "+sim.getID()+" from seed "+seed+":");

	for (int t=0; t<60; t++){
		for (int i=0; i<8; i++){
			for (int j=0; j<8; j++){
				r = (double) (rand() % 2);//values between 0 and 1
				if(r < m){//mutation rate
					scoringMatrix[t][j][i] = (double) (rand() % 2);//values between 0 and 1;
				}else{
					scoringMatrix[t][j][i] = sim.scoringMatrix[t][j][i];
				}
				
			}
		}
	}
}






///////////  score board machinery   ////////////////////////
