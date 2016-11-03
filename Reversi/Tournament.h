#pragma once
#include "Simulant.h"
#include "Game.h"
#include <vector>
#include <time.h>

using namespace System;
using namespace std;

//a training tournament
class Tournament{
public: Tournament(void);
virtual ~Tournament(void);

	void run();
	void seed();
	void initialize();


private:
	vector <float> scores;//score (number of games won) by each simulant; order shadows the sims array
	Simulant s1, s2;//the temp simulants that will play in each game
	Simulant *s;//temp simulant
	vector <int> winners;//holds the indexes of the sims with the best score
	int winnerSize;//the number of winners taken from each generation
	int setSize;//the population of each generation
	Simulant *sims;//the competing simulants
	int winner;
	Game game;
	int tournamentLength;//the number of rounds played in the tournament
	int roundCounter;//the round that is being played, or was just finished
	bool quiet;//true suppresses scoring info

	
	void playRound();
	void scoreRound();
	void writeWinningBrains();
	void buildNewBrains();
  

};

