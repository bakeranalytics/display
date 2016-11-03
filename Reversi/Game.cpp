#include "StdAfx.h"
#include "Game.h"
#include <time.h>

using namespace System;


Game::Game(void){
	turnCount = 0;
	supressOutput = true;

	setupBoard();//initialize the board
	
	//test layout
	/*
	board[0][3] = 1;
	board[1][3] = 1;
	board[2][3] = 1;
	board[3][3] = 1;
	board[3][4] = -1;
	board[4][3] = 1;
	board[5][3] = -1;
	*/
    //Console::WriteLine(L"init game");

}

Game::~Game(void)
{
}


//initialized board array to starting configuration
void Game::setupBoard(){
	turnCount = 0;

	for (int i=0; i<8; i++){
		for (int j=0; j<8; j++){
			board[j][i] = 0;
		}
	}
	board[3][3] = 1;
	board[4][4] = 1;
	board[3][4] = -1;
	board[4][3] = -1;
}

//plays a person against a Simulant running the maxFlip algorithm.  The person always goes first.
void Game::playMaxFlip(){
	int move[3];//i, j, coordinates & p for the chips polarity
    //Console::WriteLine(L"playing game");
	bool pStuck = false, sStuck = false;
	s1.newRandomBrain(time(0));//random brain
	s1.setPolarity(-1);

	while(turnCount < 60){
		showBoard();
		pStuck = false;
		sStuck = false;

		s2.setPolarity(1);//s2 is to serve as proxy for the person to check whether any moves are possible
		if(s2.getMove(board, move) == 0){//smart move not needed & takes more time to run
			Console::WriteLine("There is no where you can move...");
			pStuck = true;
		}else{
			getPersonsMove(move);
			while(move[0] == -1 || move[1] == -1 || makeMove(move) == 0) {//reprompt if the requested move is not allowed
				Console::WriteLine(" was not allowed.");
				getPersonsMove(move);
			}
			showBoard();
		}

		//do the sim's move
		s1.getMove(board, move);//uses 'max flip' algorithm
		//s1.getSmartMove(board, move, turnCount);//uses 'smart matrix' algorithm
		if(makeMove(move) == 0){//no move was possible
			Console::WriteLine("I can't move any where.");
			sStuck = true;
		}

		if(winCheck() == true) break;//check for all chips being the same polarity
		if(sStuck == true && pStuck == true) break;//check for both players being unable to move		
	}

	int winner = declareWinner();
	Console::WriteLine(" ");
	if(winner == -1){Console::WriteLine("I won.  :)");}
	if(winner == 0){Console::WriteLine("Tie.  :)");}
	if(winner == 1){Console::WriteLine("You won.  :)");}
	showBoard();
}


//plays a Simulant running the 'smart matrix' algorithm against a Simulant running the maxFlip algorithm
int Game::competeMaxFlip(Simulant &s1, Simulant &s2, bool reversed){
	setupBoard();
	s1.setPolarity(1);
	s2.setPolarity(-1);//the maxFlip sim
	int move[3];//i, j, coordinates & p for the chips polarity
	//Console::WriteLine("s1 "+s1.getID());//test
	bool s1Stuck = false, s2Stuck = false;
//jenny loves brian

	while(turnCount < 60){
		s1Stuck = false;
		s2Stuck = false;

		//do the first sim's move
		if(reversed)//s2 is smart
			s1.getMove(board, move);//uses 'max flip' statagey
		else
			s1.getSmartMove(board, move, turnCount);
		if(makeMove(move) == 0){//no move was possible
			if (! supressOutput) Console::WriteLine("s1 can't move any where.");
			s1Stuck = true;
		}
		//do the second sim's move
		if(reversed)
			s2.getSmartMove(board, move, turnCount);//uses 'smart matrix' algorithm
		else
			s2.getMove(board, move);//uses 'max flip' algorithm
		if(makeMove(move) == 0){//no move was possible
			if (! supressOutput) Console::WriteLine("s2 can't move any where.");
			s2Stuck = true;
		}

		if(winCheck() == true) break;//check for all chips being the same polarity
		if(s1Stuck == true && s2Stuck == true) break;//check for both players being unable to move		
	}

	int winner = declareWinner();

	//showBoard();
	return winner;
}



//plays a Simulant running the 'smart matrix' algorithm against a Simulant running the uniform weights algorithm
//THIS WILL PLAY THE SAME RESULT AS MAXFLIP ALGORITHM
int Game::competeUniformWeights(Simulant &s1, Simulant &s2, bool reversed){
	setupBoard();
	s1.setPolarity(1);
	s2.setPolarity(-1);//the maxFlip sim
	int move[3];//i, j, coordinates & p for the chips polarity
    //Console::WriteLine(L"playing game");
	bool s1Stuck = false, s2Stuck = false;
	if(reversed)//sim s2 is the 'smart' one
		s1.uniformWeights = true;
	else
		s2.uniformWeights = true;

	while(turnCount < 60){
		s1Stuck = false;
		s2Stuck = false;

		//do the first sim's move
		s1.getSmartMove(board, move, turnCount);
		//s1.getMove(board, move);	- uses 'max flip' statagey
		if(makeMove(move) == 0){//no move was possible
			if (! supressOutput) Console::WriteLine("s1 can't move any where.");
			s1Stuck = true;
		}
		//do the second sim's move
		s2.getSmartMove(board, move, turnCount);//uses 'smart matrix' algorithm
		//s2.getMove(board, move);//uses 'max flip' algorithm
		if(makeMove(move) == 0){//no move was possible
			if (! supressOutput) Console::WriteLine("s2 can't move any where.");
			s2Stuck = true;
		}

		if(winCheck() == true) break;//check for all chips being the same polarity
		if(s1Stuck == true && s2Stuck == true) break;//check for both players being unable to move		
	}

	int winner = declareWinner();

	//showBoard();
	return winner;
}



//plays a person against a Simulant.  The person always goes first.
void Game::play(){
	int move[3];//i, j, coordinates & p for the chips polarity
    //Console::WriteLine(L"playing game");
	bool pStuck = false, sStuck = false;
	s1.newRandomBrain(time(0));//random brain
	s1.setPolarity(-1);

	while(turnCount < 60){
		showBoard();
		pStuck = false;
		sStuck = false;

		s2.setPolarity(1);//s2 is to serve as proxy for the person to check whether any moves are possible
		if(s2.getMove(board, move) == 0){//smart move not needed & takes more time to run
			Console::WriteLine("There is no where you can move...");
			pStuck = true;
		}else{
			getPersonsMove(move);
			while(move[0] == -1 || move[1] == -1 || makeMove(move) == 0) {//reprompt if the requested move is not allowed
				Console::WriteLine(" was not allowed.");
				getPersonsMove(move);
			}
			showBoard();
		}

		//do the sim's move
		//s1.getMove(board, move);	- uses 'max flip' statagey
		s1.getSmartMove(board, move, turnCount);//getMove(board, move);
		if(makeMove(move) == 0){//no move was possible
			Console::WriteLine("I can't move any where.");
			sStuck = true;
		}

		if(winCheck() == true) break;//check for all chips being the same polarity
		if(sStuck == true && pStuck == true) break;//check for both players being unable to move		
	}

	int winner = declareWinner();
	Console::WriteLine(" ");
	if(winner == -1){Console::WriteLine("I won.  :)");}
	if(winner == 0){Console::WriteLine("Tie.  :)");}
	if(winner == 1){Console::WriteLine("You won.  :)");}
	showBoard();
}
//plays a person against a Simulant using uniform wieghts/max coverage algorithm.  The person always goes first.
void Game::playMaxCoverage(){
	int move[3];//i, j, coordinates & p for the chips polarity
    //Console::WriteLine(L"playing game");
	bool pStuck = false, sStuck = false;
	//s1.newRandomBrain(time(0));//random brain
	s1.uniformWeights = true;
	s1.setPolarity(-1);

	while(turnCount < 60){
		showBoard();
		pStuck = false;
		sStuck = false;

		s2.setPolarity(1);//s2 is to serve as proxy for the person to check whether any moves are possible
		if(s2.getMove(board, move) == 0){//smart move not needed & takes more time to run
			Console::WriteLine("There is no where you can move...");
			pStuck = true;
		}else{
			getPersonsMove(move);
			while(move[0] == -1 || move[1] == -1 || makeMove(move) == 0) {//reprompt if the requested move is not allowed
				Console::WriteLine(" was not allowed.");
				getPersonsMove(move);
			}
			showBoard();
		}

		//do the sim's move
		//s1.getMove(board, move);	- uses 'max flip' statagey
		s1.getSmartMove(board, move, turnCount);//getMove(board, move);
		if(makeMove(move) == 0){//no move was possible
			Console::WriteLine("I can't move any where.");
			sStuck = true;
		}

		if(winCheck() == true) break;//check for all chips being the same polarity
		if(sStuck == true && pStuck == true) break;//check for both players being unable to move		
	}

	int winner = declareWinner();
	Console::WriteLine(" ");
	if(winner == -1){Console::WriteLine("I won.  :)");}
	if(winner == 0){Console::WriteLine("Tie.  :)");}
	if(winner == 1){Console::WriteLine("You won.  :)");}
	showBoard();
}

//plays a person against a Simulant.  The person always goes first.
void Game::playSim(array<System::String ^> ^simNames){
	int move[3];//i, j, coordinates & p for the chips polarity
    //Console::WriteLine(L"playing game");
	bool pStuck = false, sStuck = false;

	Simulant s1;
	if ( ! s1.readBrain(""+simNames[1]+".sim") ) {
		Console::WriteLine("sim "+simNames[1]+" cannot be read.");
		return;
	}
	s1.setPolarity(-1);

	while(turnCount < 60){
		showBoard();
		pStuck = false;
		sStuck = false;

		s2.setPolarity(1);//s2 is to serve as proxy for the person to check whether any moves are possible
		if(s2.getMove(board, move) == 0){//smart move not needed & takes more time to run
			Console::WriteLine("There is no where you can move...");
			pStuck = true;
		}else{
			getPersonsMove(move);
			while(move[0] == -1 || move[1] == -1 || makeMove(move) == 0) {//reprompt if the requested move is not allowed
				Console::WriteLine(" was not allowed.");
				getPersonsMove(move);
			}
			showBoard();
		}

		//do the sim's move
		//s1.getMove(board, move);	- uses 'max flip' statagey
		s1.getSmartMove(board, move, turnCount);//getMove(board, move);
		if(makeMove(move) == 0){//no move was possible
			Console::WriteLine("I can't move any where.");
			sStuck = true;
		}

		if(winCheck() == true) break;//check for all chips being the same polarity
		if(sStuck == true && pStuck == true) break;//check for both players being unable to move		
	}

	int winner = declareWinner();
	Console::WriteLine(" ");
	if(winner == -1){Console::WriteLine("I won.  :)");}
	if(winner == 0){Console::WriteLine("Tie.  :)");}
	if(winner == 1){Console::WriteLine("You won.  :)");}
	showBoard();
}


//plays a Simulant against a Simulant
int Game::compete(Simulant &s1, Simulant &s2){
	setupBoard();
	s1.setPolarity(1);
	s2.setPolarity(-1);
	int move[3];//i, j, coordinates & p for the chips polarity
    //Console::WriteLine(L"playing game");
	bool s1Stuck = false, s2Stuck = false;

	while(turnCount < 60){
		s1Stuck = false;
		s2Stuck = false;

		//do the first sim's move
		s1.getSmartMove(board, move, turnCount);
		//s1.getMove(board, move);	- uses 'max flip' statagey
		if(makeMove(move) == 0){//no move was possible
			if (! supressOutput) Console::WriteLine("s1 can't move any where.");
			s1Stuck = true;
		}
		//do the second sim's move
		s2.getSmartMove(board, move, turnCount);
		//s2.getMove(board, move);	- uses 'max flip' statagey
		if(makeMove(move) == 0){//no move was possible
			if (! supressOutput) Console::WriteLine("s2 can't move any where.");
			s2Stuck = true;
		}

		if(winCheck() == true) break;//check for all chips being the same polarity
		if(s1Stuck == true && s2Stuck == true) break;//check for both players being unable to move		
	}

	int winner = declareWinner();

	//showBoard();
	return winner;
}


//computes who won the game and returns the polarity of the winner
int Game::declareWinner(){
	int win = 0;

	for (int i=0; i<8; i++){
		for (int j=0; j<8; j++){
			win += board[i][j];
		}
	}

	if(win > 0) return 1;
	if(win < 0) return -1;
	if(win == 0) return 0;//tie

	return win;
}

//checks whether all chips are of the same polarity
//returns true if all are of the same polarity, false otherwise
bool Game::winCheck(){
	bool finished = true;
	bool p1=false, p2=false;//flags the pressence of each polarity 1->1, 2->-1 :: true if the polarity is present on the board/

	for (int i=0; i<8; i++){
		for (int j=0; j<8; j++){
			if(board[i][j] == -1) p2 = true;
			if(board[i][j] == 1) p1 = true;
			if(p1 && p2){
				finished = false;
				break;
			}
		}
	}

	return finished;
}


//returns true if all spaces on the board are occupied; returns false if one or more are inoccupied
bool Game::boardFull(){
	bool finished = true;

	for (int i=0; i<8; i++){
		for (int j=0; j<8; j++){
			if(board[i][j] == 0){
				finished = false;
				break;
			}
		}
	}

	return finished;
}


//gets user input for its next move
void Game::getPersonsMove(int move[3]){
	//init to a 'not set' flag
	move[0] = -1;
	move[1] = -1;

	Console::Write("Your turn ("+turnCount+").  Please enter your move: ");
	String^ m = Console::ReadLine();

	//the i coordinate
	if(m->Contains("1")) move[1] = 0;
	if(m->Contains("2")) move[1] = 1;
	if(m->Contains("3")) move[1] = 2;
	if(m->Contains("4")) move[1] = 3;
	if(m->Contains("5")) move[1] = 4;
	if(m->Contains("6")) move[1] = 5;
	if(m->Contains("7")) move[1] = 6;
	if(m->Contains("8")) move[1] = 7;
	
	//the j coordinate
	if(m->Contains("a")) move[0] = 0;
	if(m->Contains("b")) move[0] = 1;
	if(m->Contains("c")) move[0] = 2;
	if(m->Contains("d")) move[0] = 3;
	if(m->Contains("e")) move[0] = 4;
	if(m->Contains("f")) move[0] = 5;
	if(m->Contains("g")) move[0] = 6;
	if(m->Contains("h")) move[0] = 7;

	move[2] = 1;//the person has polarity 1
	
	/*//test block
	Console::Write("requested move: ");//test
		Console::Write(move[0]);//test
		Console::Write(",  ");//test
		Console::Write(move[1]);//test
		Console::Write(" -> ");//test
		Console::Write(move[2]);//test
		//Console::Write(" was ");//test	
	*///end test block
}


//updates the board array with the move made
//input array: i & j are board coordinates, p is the polarity/color of chip played (-1 or 1)
//returns the number of chips flipped by the move
int Game::makeMove(int move[3]){	
	if(move[0] == -1){//check for a 'no move possible' flag
		return 0;
	}

	//get move params
	int i = move[0];//the i coordinate
	int j = move[1];//the j coordinate
	int p = move[2];//the move polarity (which player placed the chip)

	int flipCount = 0;

	/*test block
	Console::Write("moving ");//test
	Console::Write(i);//test
	Console::Write(",  ");//test
	Console::Write(j);//test
	Console::Write(" -> ");//test
	Console::Write(p);//test
	Console::Write("\n");//test
	//end test block
	*/

	////////////// initial chip
	if(board[i][j] == 0){
		board[i][j] = p;
	}else{//the square already has a chip, can't play another one there
		return 0;
	}

	//////////////// induced flips
	int x, y, xt, yt;//i corresponds to x & j to y and are used to find flip boundaries.  xt & yt are temp indexes for flipping

	//right of played square
	y = j;
	for(x=i+1; x<8; x++){ 
		if(board[x][y] == 0){//found empty square
			break;
		}
		if(board[x][y] == p){//found chip of same polarity
			for(xt=i+1; xt<x; xt++){//flip the intervening chips
				board[xt][y] = p;
				flipCount++;
				//Console::WriteLine("   flipping "+xt+", "+y+" -> "+p+" :: "+board[xt][y]);//test
			}
			break;
		}
	}

	//left of played square
	y = j;
	for(x=i-1; x>-1; x--){ 
		if(board[x][y] == 0){//found empty square
			break;
		}
		if(board[x][y] == p){//found chip of same polarity
			for(xt=i-1; x<xt; xt--){//flip the intervening chips
				board[xt][y] = p;
				flipCount++;
				//Console::WriteLine("   flipping "+xt+", "+y+" -> "+p+" :: "+board[xt][y]);//test
			}
			break;
		}
	}

	//below played square
	x = i;
	for(y=j+1; y<8; y++){ 
		if(board[x][y] == 0){//found empty square
			break;
		}
		if(board[x][y] == p){//found chip of same polarity
			for(yt=j+1; yt<y; yt++){//flip the intervening chips
				board[x][yt] = p;
				flipCount++;
				//Console::WriteLine("  flipping "+yt+", "+y+" -> "+p+" :: "+board[x][yt]);//test
			}
			break;
		}
	}

	//above played square
	x = i;
	for(y=j-1; y>-1; y--){ 
		if(board[x][y] == 0){//found empty square
			break;
		}
		if(board[x][y] == p){//found chip of same polarity
			for(yt=j-1; y<yt; yt--){//flip the intervening chips
				board[x][yt] = p;
				flipCount++;
				//Console::WriteLine("  flipping "+yt+", "+y+" -> "+p+" :: "+board[x][yt]);//test
			}
			break;
		}
	}

	//right-upper diagonal of played square
	x = i;
	for(y=j-1; y>-1; y--){ 
		x++;
		if(x>7){//left side of board
			break;
		}
		if(board[x][y] == 0){//found empty square
			break;
		}
		if(board[x][y] == p){//found chip of same polarity
			x = i;
			for(yt=j-1; y<yt; yt--){//flip the intervening chips
				x++;
				board[x][yt] = p;
				flipCount++;
				//Console::WriteLine("   flipping "+xt+", "+y+" -> "+p+" :: "+board[x][yt]);//test
			}
			break;
		}
	}

	//right-lower diagonal of played square
	x = i;
	for(y=j+1; y<8; y++){ 
		x++;
		//Console::WriteLine("    right, lower: "+x+", "+y+" -> "+p+" :: "+board[x][y]);//test
		if(x>7){//right side of board
			break;
		}
		if(board[x][y] == 0){//found empty square
			break;
		}
		if(board[x][y] == p){//found chip of same polarity
			x = i;
			for(yt=j+1; yt<y; yt++){//flip the intervening chips
				x++;
				board[x][yt] = p;
				flipCount++;
				//Console::WriteLine("    flipping "+x+", "+yt+" -> "+p+" :: "+board[x][yt]);//test
			}
			break;
		}
	}


	//left-upper diagonal of played square
	x = i;
	for(y=j-1; y>-1; y--){ 
		x--;
		if(x<0){//left side of board
			break;
		}
		if(board[x][y] == 0){//found empty square
			break;
		}
		if(board[x][y] == p){//found chip of same polarity
			x = i;
			for(yt=j-1; y<yt; yt--){//flip the intervening chips
				x--;
				board[x][yt] = p;
				flipCount++;
				//Console::WriteLine("   flipping "+xt+", "+y+" -> "+p+" :: "+board[x][yt]);//test
			}
			break;
		}
	}

	//left-lower diagonal of played square
	x = i;
	for(y=j+1; y<8; y++){ 
		x--;
		if(x<0){//left side of board
			break;
		}
		if(board[x][y] == 0){//found empty square
			break;
		}
		if(board[x][y] == p){//found chip of same polarity
			x = i;
			for(yt=j+1; yt<y; yt++){//flip the intervening chips
				x--;
				board[x][yt] = p;
				flipCount++;
				//Console::WriteLine("   flipping "+xt+", "+y+" -> "+p+" :: "+board[x][yt]);//test
			}
			break;
		}
	}


	//flipCount++;//test
	//Console::WriteLine("flipCount "+flipCount);//test

	////////////// increment turn counter or reset initial chip if no flips occured
	if (flipCount == 0){//the move is not allowed		
		board[i][j] = 0;
	}else{
		turnCount++;
	}

	return flipCount;
}




//display the current board layout
void Game::showBoard(){
	int sq;//the square's value

    //Console::WriteLine(L"Board");
	Console::Write("  A B C D E F G H\n");
	for (int i=0; i<8; i++){
		Console::Write(i+1);
		for (int j=0; j<8; j++){
			sq = board[j][i];
			Console::Write(L" ");
			if(sq == 0)
				Console::Write(".");
			else if(sq == 1)
				Console::Write("O");
			else if(sq == -1)
				Console::Write("X");
		}
		Console::Write(L"\n");
	}
}


void Game::setPlayers(Simulant s1){
	this->s1 = s1;
}