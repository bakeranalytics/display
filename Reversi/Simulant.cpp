#include "StdAfx.h"
#include "Simulant.h"
#include <stdio.h>

using namespace std;

Simulant::Simulant(void)
{
	id = -1;//uninitialized flag
	uniformWeights = false;//defaults to indiviual weight values
	//Console::WriteLine(L"new sim: "+polarity);//test//
}

Simulant::~Simulant(void)
{
}


///////////  score board machinery   ////////////////////////


//encodes the Simulants move.  The move selected is the one that produces the highest boardScore.
//returns the maximium boardScore of any move
double Simulant::getSmartMove(int const board[8][8], int move[3], int turnCount){//find the best move by boardScore
	double score = 0, maxScore = 0;//the board score, the largest board score 
	int mi, mj;//the coordinates associated with the move giving maxScore
	move[2] = this->polarity;//-1;
	turn = turnCount; 

	for (int i=0; i<8; i++){
		move[1] = i;
		for (int j=0; j<8; j++){
			move[0] = j;
			score = computePutativeBoardScore(move, board);
			if (score > maxScore){
				mi = i;
				mj = j;
				maxScore = score;
			}			 
		}
	}
	if(maxScore == 0){//no moves were possible
		mj = mi = -1;
	}
	move[0] = mj;
	move[1] = mi;

	return maxScore;
}


//computes which chips would be flipped by a move and what the board score would be after the flips
//input: array of: i & j are board coordinates, p is the color/polarity of chip played (-1 or 1)
//returns the board score the move would produce
//: possible score values are 0, 1 - 129 : 0 means no moves were possible, 
	//lower numbers mean the opponent has a better board position
double Simulant::computePutativeBoardScore(int move[3], int const board[8][8]){
	int cboard[8][8];//a copy of the board that the putative filps are made on to compute the putative board score
	copyBoard(board, cboard);// this is to be used for board scoring & deep decisions (looking a move ahead)

	//get move params
	int i = move[0];//the i coordinate
	int j = move[1];//the j coordinate
	int p = move[2];//the move polarity (which player placed the chip)
	//Console::WriteLine("compute sim move putative flips for: "+move[0]+", "+move[1]+", "+move[2]);//test

	int flipCount = 0;

	////////////// initial chip
	if(board[i][j] == 0){
		cboard[i][j] = p;
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
				cboard[xt][y] = p;
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
				cboard[xt][y] = p;
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
				cboard[x][yt] = p;
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
				cboard[x][yt] = p;
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
				cboard[x][yt] = p;
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
				cboard[x][yt] = p;
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
				cboard[x][yt] = p;
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
				cboard[x][yt] = p;
				flipCount++;
				//Console::WriteLine("   flipping "+xt+", "+y+" -> "+p+" :: "+board[x][yt]);//test
			}
			break;
		}
	}

	//flipCount++;//test
	//Console::WriteLine("flipCount "+flipCount);//test

	if(flipCount == 0){//the move was not possible bacause it wouldn't flip anything
		return 0;
	}

	double score = scoreBoard(cboard);

	return score;
}


//computes the board score from the scoring matrix and each board occupation
//returns the board score, high is good for the sim, low is good for the opponent
double Simulant::scoreBoard(int const pboard[8][8]){
	double score = 0;

	for (int i=0; i<8; i++){
		for (int j=0; j<8; j++){
			if(uniformWeights)
				score += pboard[j][i];
			else
				score += pboard[j][i] * scoringMatrix[turn][j][i];
		}
	}

	//Console::Write("score "+score+" -> ");//test
	score = score * polarity + 65;
	//Console::WriteLine("score "+score+" @ "+turn);//test
	return (score);//range is 1 to 129 :: high favors the sim
}



///////////  flip count machinery   ////////////////////////

//encodes the Simulants move.  The move selected is the one that flips the most chips.
//returns the maximium number of flips possible in any move
int Simulant::getMove(int const board[8][8], int move[3]){//find the best move by max # of flips
	int flip = 0, maxFlip = 0, mi, mj;//the flip count, the largest flip count & the coordinates associated with the move giving maxFlip
	move[2] = polarity;//-1;
	for (int i=0; i<8; i++){
		move[1] = i;
		for (int j=0; j<8; j++){
			move[0] = j;
			flip = computePutativeFlips(move, board);
			if (flip > maxFlip){
				mi = i;
				mj = j;
				maxFlip = flip;
			}			 
		}
	}
	if(maxFlip == 0){//no moves were possible
		mj = mi = -1;
	}
	move[0] = mj;
	move[1] = mi;

	return maxFlip;
}

//computes which chips would be flipped by a move
//input: array of: i & j are board coordinates, p is the color/polarity of chip played (-1 or 1)
//returns the number of flips induced by the move
//NOTE changing all board refs to cboard & replacing the cboard changes will make this method provide a board-copy for deep decisions
int Simulant::computePutativeFlips(int move[3], int const board[8][8]){
	//get move params
	int i = move[0];//the i coordinate
	int j = move[1];//the j coordinate
	int p = move[2];//the move polarity (which player placed the chip)
	//Console::WriteLine("compute sim move putative flips for: "+move[0]+", "+move[1]+", "+move[2]);//test

	int flipCount = 0;

	////////////// initial chip
	if(board[i][j] == 0){
		//cboard[i][j] = p;
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
				//cboard[xt][y] = p;
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
				//cboard[xt][y] = p;
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
				//cboard[x][yt] = p;
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
				//cboard[x][yt] = p;
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
				//cboard[x][yt] = p;
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
				//cboard[x][yt] = p;
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
				//cboard[x][yt] = p;
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
				//cboard[x][yt] = p;
				flipCount++;
				//Console::WriteLine("   flipping "+xt+", "+y+" -> "+p+" :: "+board[x][yt]);//test
			}
			break;
		}
	}


	//flipCount++;//test
	//Console::WriteLine("flipCount "+flipCount);//test

	return flipCount;
}



///////////  utility functions   ////////////////////////

//copies the board to cboard
void Simulant::copyBoard(int const board[8][8], int cboard[8][8]){
	for (int i=0; i<8; i++){
		for (int j=0; j<8; j++){
			cboard[j][i] = board[j][i];
		}
	}
}


void Simulant::setPolarity(int p){
	polarity = p;
}



//creates a new, randomly assigned scoring matrix
void Simulant::newRandomBrain(int seed){
	srand(seed);
	//Console::WriteLine("new random brain for "+id+" from seed "+seed+":");

	for (int t=0; t<60; t++){
		for (int i=0; i<8; i++){
			for (int j=0; j<8; j++){
				scoringMatrix[t][j][i] = (double)(rand() % 2);//values between 0 and 1
			}
		}
	}
}


//randomly selects one turn.  Prior to that turn all brain fields are taken from sim1.  At
//and after that turn all brain fields art taken from sim2.  The random turn is taken between
//2 and 60 so that at least one turn's worth of brain fields come from each sim.
void Simulant::newBrainSingleCrossover(Simulant const &sim1, Simulant const &sim2, int seed){
	srand(seed);
	int r=-1;//random number used for deciding when to cross over

	do{
		r = (int) rand() % 60;//values between 1 & 59
	}while(r == 0  ||  r == 60);
	//Console::WriteLine("new brain from single crossover of "+sim1.getID()+" and "+sim2.getID()+" from seed "+seed+" at "+r);//test
	for (int t=0; t<60; t++){
		for (int i=0; i<8; i++){
			for (int j=0; j<8; j++){
				if(r < t){
					scoringMatrix[t][j][i] = sim1.scoringMatrix[t][j][i];
				}else{
					scoringMatrix[t][j][i] = sim2.scoringMatrix[t][j][i];
				}
				
			}
		}
	}
}


//randomly selects two turns.  Prior to the smaller the brain fields are taken from sim1.  
//At the smaller the brain fields start coming from sim2 and then change back to sim2 at the larger.
//The turns are chosen from 2 to 60
void Simulant::newBrainDoubleCrossover(Simulant const &sim1, Simulant const &sim2, int seed){
	srand(seed);
	int r=-1, q=-1;//random numbers used for deciding when to cross over
	int c1, c2;//the crossover points

	do{
		r = (int) rand() % 60;//values between 1 & 59
	}while(r == 0  ||  r == 60);
	do{
		q = (int) rand() % 60;//values between 1 & 59
	}while(q == 0  ||  q == 60);
	if(r>q){//set crossover order
		c2 = r;
		c1 = q;
	}else{
		c2 = q;
		c1 = r;
	}

	//Console::WriteLine("new brain from double crossover of "+sim1.getID()+" and "+sim2.getID()+" from seed "+seed+" at "+c1+", "+c2);//test

	for (int t=0; t<60; t++){
		for (int i=0; i<8; i++){
			for (int j=0; j<8; j++){
				if(t < c1){
					scoringMatrix[t][j][i] = sim1.scoringMatrix[t][j][i];
				}else if(t < c2){
					scoringMatrix[t][j][i] = sim2.scoringMatrix[t][j][i];
				}else{
					scoringMatrix[t][j][i] = sim1.scoringMatrix[t][j][i];
				}
				
			}
		}
	}
}


//randomly selects 10% of the brain fields to change to a random number
void Simulant::newBrainMutation(Simulant const &sim, int seed){
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



//writes out the sims brain to the cmd, stopping when the number of parameters printed equals the input paramtere b, or when finished
void Simulant::showBrain(int b){
	cout <<"\nBrain of sim "<< id <<" --------\n";
	for (int t=0; t<60; t++){
		cout <<"-------"<< id <<":"<< t <<"--------\n";
		for (int i=0; i<8; i++){
			for (int j=0; j<8; j++){
				cout << scoringMatrix[t][j][i] <<"\t";
				if(b<t*i*j){//stop
					cout <<":::::::::::::::::::::::::\n";
					return;
				}
			}
			cout <<"\n";
		}
	}
	cout <<":::::::::::::::::::::::::\n";
}


//write the scoring matrix values to a file
void Simulant::writeBrain(const string name){
	string fileName = "simBrains\\tournament\\"+name+".sim";
	ofstream sw (fileName.c_str());
	if(sw.fail()){
		cerr << "Unable to write "<< fileName <<endl;	
		return;
	} 
   //StreamWriter^ sw = gcnew StreamWriter(fileName);

	for (int t=0; t<60; t++){//turn grid - one sheet of 8X8 per turn
		for (int i=0; i<8; i++){//row
			for (int j=0; j<8; j++){//colum
				//sw->WriteLine(""+scoringMatrix[t][j][i]);//one value per line
				sw << scoringMatrix[t][j][i] <<endl;//one value per line
			}
			//sw->WriteLine("");//end of row
		}
		//sw->WriteLine("");//end of turn grid
	}
   sw.close();
}

//write the scoring matrix values of the simulants to seed the next generation to a file in the simBrain\seeds directory
void Simulant::writeSeedBrain(const string name){
	string fileName = "simBrains\\seeds\\"+name+".sim";
   //StreamWriter^ sw = gcnew StreamWriter(fileName);
   ofstream sw (fileName.c_str());
	if(sw.fail()){
		cerr << "Unable to write "<< fileName <<endl;	
		return;
	} 

	for (int t=0; t<60; t++){//turn grid - one sheet of 8X8 per turn
		for (int i=0; i<8; i++){//row
			for (int j=0; j<8; j++){//colum
				sw << scoringMatrix[t][j][i] <<endl;//one value per line
			}
			//sw->WriteLine("");//end of row
		}
		//sw->WriteLine("");//end of turn grid
	}
   sw.close();
}

//read the scoring matrix values from a file
bool Simulant::readBrain(const string name){
	string fileName = "simBrains\\"+name;
    int t=0, i=0, j=0;
	bool itWorked = true;
	 
	FILE *inFile;
	inFile = fopen(fileName.c_str(), "r");	
	if(inFile == NULL){
		cerr << "Unable to read "<< fileName <<endl;
		return false;
	}
	
	  double v;
      while ( fscanf(inFile, "%d", &v) != 0 ){
		scoringMatrix[t][j][i] = v;//one value per line	  
		//Console::WriteLine(t+"\t"+i+"\t"+j+"\t"+scoringMatrix[t][j][i]);//test
		i++;
		if(i == 8){ i=0; j++;}
		if(j == 8){ j=0; t++;}
		if(t == 61) cout << "there is more data in this file than is expected.  you may have a corrupted sim file.";
      }
	  fclose(inFile);
   
	return itWorked;
}

void Simulant::setID(int i){
	id = i;
}

int Simulant::getID(){
	return id;
}