#pragma once
#include <string>
#include <fstream>
#include <stdlib.h>
#include <iostream>
using namespace std;

// a simulated player
class Simulant
{
public:	Simulant(void);
public:	virtual ~Simulant(void);	
	 int getMove(int const board[8][8], int move[3]);
	 double getSmartMove(int const board[8][8], int move[3], int turnCount);
	 void copyBoard(int const board[8][8], int cboard[8][8]);
	 int computePutativeFlips(int move[3], int const board[8][8]);
	 double computePutativeBoardScore(int move[3], int const board[8][8]);
	 void setPolarity(int p);
	 void newRandomBrain(int seed);
	 void newBrainSingleCrossover(Simulant const &sim1, Simulant const &sim2, int seed);
	 void newBrainDoubleCrossover(Simulant const &sim1, Simulant const &sim2, int seed);
	 void newBrainMutation(Simulant const &sim, int seed);
	 void writeBrain(const string name);
	 void writeSeedBrain(const string name);
	 bool readBrain(const string name);
	 void setID(int i);
	 int getID();
	 void showBrain(int b=4);//60*64 for full
	 bool uniformWeights;//true makes all the wieghts in the brain equal to one

//protected:
	double scoringMatrix[60][8][8];//brain values: between 0 - 1

protected:
	int turn;
	int id;
	int polarity;
	
	double scoreBoard(int const pboard[8][8]);

private: 

};
