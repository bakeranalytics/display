#pragma once
#include "Simulant.h"

class Game
{
public: Game(void);
 virtual ~Game(void);

 Simulant s1, s2;

 void play();
 void playSim(array<System::String ^> ^simNames);
 void playMaxFlip();	
 void playMaxCoverage();	
 void showBoard();
 void setPlayers(Simulant s1);
 int compete(Simulant &s1, Simulant &s2);
 int competeMaxFlip(Simulant &s1, Simulant &s2, bool reversed);
 int competeUniformWeights(Simulant &s1, Simulant &s2, bool reversed);
//public: void setPlayers(Simulant s1, Simulant s2);

	bool supressOutput;

private:
  int board[8][8];
  int turnCount;
  
  void setupBoard();
 void getPersonsMove(int move[3]);
 int makeMove(int move[3]);
 int declareWinner();
 bool winCheck();
 bool boardFull();
};

