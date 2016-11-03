#include "StdAfx.h"
#include "Tournament.h"

#using <mscorlib.dll>

using namespace System;
using namespace System::IO;
using namespace std;


//a training tournament - not competition to ses which is the better sim
Tournament::Tournament(void){
	quiet = true;
	tournamentLength = 4000;
	roundCounter = 0;
	winnerSize = 100;//the number of winners taken from each generation / tournament
	setSize = 5*winnerSize;//the population of each generation
	sims = new Simulant[setSize];
	scores.resize(setSize);
	winners.resize(winnerSize);
	for(int j=0; j<winnerSize; j++){//initialize the array
		winners[j] = -1;		
	}
}


Tournament::~Tournament(void){
	delete [] sims;
	
}


//gives each sim an ID & a brain from the stored sim files and initializes the scores
void Tournament::seed(){
	//read in the seed sims
	int i=0;//input index
	Console::Write("Seeding: "); 
	DirectoryInfo^ di = gcnew DirectoryInfo(Environment::CurrentDirectory+"\\simBrains\\seeds");
    Collections::IEnumerator^ myEnum = di->GetFiles()->GetEnumerator();
    while (myEnum->MoveNext()) {
        FileInfo^ fiTemp = (FileInfo^)(myEnum->Current);
        Console::Write(fiTemp->Name+"\t");
		try{
			sims[i].readBrain("seeds\\"+fiTemp->Name);//read in the sim brains
		} catch (Exception^ e) {
			Console::WriteLine("Unable to read seeds: {0}", e);
		}
		winners[i] = i;//set the winners in the winners array to be the seeds
		i++;
		if(i >= winnerSize) break;
    }
	Console::WriteLine("  "); 
	
	buildNewBrains();//build a new set from the seeds
	
	scores[i] = 0;
}


//gives each sim an ID & a random brain and initializes the scores
void Tournament::initialize(){
	for(int i=0; i<setSize; i++){//load the array with an even number of sims & initialize the scores
		sims[i].setID(i);
		sims[i].newRandomBrain(i+time(0));
		scores[i] = 0;
	}
}


//build a new generation of brains for the next tournament & resets the score array
void Tournament::buildNewBrains(){
	bool keeper;//true if the sim was a winner -> not replaced; kept into the next generation
	srand(time(0)+roundCounter);//for random seed 
	int w = 0;//index of winners
	bool quietBrain = true;
	//Console::WriteLine("buildNewBrains\t firstRandomseed "+(time(0)+roundCounter)+":");//test//

	/////create a new generation of brains/////////
	//1. keep the winners, 2. make s single & 3. double crossover from each winner with a random winner, 
	//4. make a mutaion from each winner, 5. make some new random ones
	//each winner has an equal chance for producing offspring - 
	//UPGRADE - GIVE STRONGER WINNERS A BETTER CHANCE for producing offspring
		
	for(int i=0; i<setSize; i++){//keep the winning brains for the next generation & do some house keeping
		keeper = false;
		sims[i].setID(i);		
		scores[i] = 0;//reset the scores array

		for(w=0; w<winnerSize; w++){
			if ( i == winners[w]){
				keeper = true;			
				if(!quietBrain)Console::WriteLine("buildNewBrains\t winners: "+i+":"+sims[i].getID());//test//
				break;
			}
		}
	}
	
	int s=-1, q=-1;//index for winner that will produce offspring
	int nextGenerationCounter = 0;//tally of how many non-winner-carry-over new brains are created for the next generation
	int t;//index through setsize - used to ensure each sim gets a new brain only once while moving through aall adaptation processs
	for(int i=0; i<setSize; i++){//make single cross overs from the winning brains for the next generation
		if (nextGenerationCounter == winnerSize) break;//only take winnerSize of crossovers into the next generation
		keeper = false;
		for(w=0; w<winnerSize; w++){
			if ( i == winners[w]){
				keeper = true;
				break;
			}
		}
		if( ! keeper){//replace the losers with a new brains
			//xx s = (int)( winnerSize*(float)(rand() / (float)RAND_MAX));//values between 0 and winnerSize
			s = rand() % winnerSize;//values between 0 and winnerSize-1
			do{ 
				q = rand() % winnerSize;//values between 0 and winnerSize-1
			}while(s == q);
			if(!quietBrain)Console::Write("new brain for "+i+" ("+s+":"+q+")");//test//
			if(!quietBrain)Console::WriteLine(" by crossover of "+winners[s]+" & "+winners[q]);//test//
			//xx if(!quietBrain)Console::WriteLine("new brain for "+i+" by crossover of "+winners[s]+":"+sims[winners[s]].getID()+" & "+winners[q]+":"+sims[winners[q]].getID()+"  <- "+s+":"+q);//test//
			sims[i].newBrainSingleCrossover(sims[winners[s]], sims[winners[q]], (int)rand());
			nextGenerationCounter++;//count each sim given a new brain
		}
		t = i;
	}
	
	for(int i=t+1; i<setSize; i++){//make double overs from the winning brains for the next generation
		if (nextGenerationCounter == winnerSize*2) break;//only take winnerSize of double double crossovers into the next generation
		keeper = false;
		for(w=0; w<winnerSize; w++){//
			if ( i == winners[w]){
				keeper = true;
				break;
			}
		}
		if( ! keeper){//replace the losers with a new brains
			s = rand() % winnerSize;//values between 0 and winnerSize-1
			do{ 
				q = rand() % winnerSize;//values between 0 and winnerSize-1
			}while(s == q);
			if(!quietBrain)Console::Write("new brain for "+i+" ("+s+":"+q+")");//test//
			if(!quietBrain)Console::WriteLine(" by double crossover of "+winners[s]+" & "+winners[q]);//test//
			sims[i].newBrainDoubleCrossover(sims[winners[s]], sims[winners[q]], (int)rand());
			nextGenerationCounter++;//count each sim given a new brain
		}
		t = i;
	}
	
	for(int i=t+1; i<setSize; i++){//make mutaions of the winning brains for the next generation
		if (nextGenerationCounter == winnerSize*3) break;//only take winnerSize of mutations into the next generation
		keeper = false;
		for(w=0; w<winnerSize; w++){//
			if ( i == winners[w]){
				keeper = true;
				break;
			}
		}
		if( ! keeper){//replace the losers with a new brains
			s = rand() % winnerSize;//values between 0 and winnerSize-1
			if(!quietBrain)Console::WriteLine("new brain for "+i+" by mutation of "+winners[s]+"  <- "+s);//test//
			sims[i].newBrainMutation(sims[winners[s]], (int)rand());
			nextGenerationCounter++;//count each sim given a new brain
		}
		t = i;
	}
	
	for(int i=t+1; i<setSize; i++){//make random new brains for the remaining sims for the next generation
		keeper = false;
		for(w=0; w<winnerSize; w++){//
			if ( i == winners[w]){
				keeper = true;
				break;
			}
		}
		if( ! keeper){//replace the losers with a new brains
			if(!quietBrain)Console::WriteLine("new brain for "+i+" by random generation");//test//
			sims[i].newRandomBrain((int)rand());//*(roundCounter+setSize*nextGenerationCounter));
			nextGenerationCounter++;//count each sim given a new brain
		}
	}

}


void Tournament::run(){

	while(roundCounter < tournamentLength){
		if(! quiet)Console::WriteLine("Tournament "+roundCounter+":");
		else Console::Write(roundCounter);
		playRound();
		scoreRound();	
		writeWinningBrains();
		buildNewBrains();
		roundCounter++;
		if(! quiet)Console::WriteLine("::\n");
	}

}//end run


//plays each simulant against all the others & tallies their scores
void Tournament::playRound(){
	//int p = 1;//polarity toggle

	for(int i=0; i<setSize; i++){
		s1 = sims[i];
		//s1.setPolarity(1);
		
		//for(int j=0; j<i; j++){//output only
				//Console::Write("         ");//use when printing contestants too
		//		if(! quiet)Console::Write("    ");
		//}
		for(int j=i; j<setSize; j++){
			if(i != j){//don't play the sim against itself
				s2 = sims[j];//[setSize - 1 - j];
				//s2.setPolarity(-1);

				//Console::Write("@"+s1.getID()+":"+s2.getID()+"_");//print contestants

				winner = game.compete(s1, s2);
				if( ! game.supressOutput)
					if(! quiet)Console::Write("("+i+","+j+")");
				if(winner == -1){
					if(! quiet)Console::Write(j);
					scores[j]++;
				}
				if(winner == 0){
					if(! quiet)Console::Write("T");
					scores[j] = scores[j] + 0.5;
					scores[i] = scores[i] + 0.5;
				}
				if(winner == 1){
					if(! quiet)Console::Write(i);
					scores[i]++;
				}
				if( ! game.supressOutput)game.showBoard();

				//s2.setPolarity(1);//reverse polarity
				//s1.setPolarity(-1);//reverse polarity
				winner = game.compete(s2, s1);//reverse starting order
				if(! quiet)Console::Write(":");
				if(winner == -1){
					if(! quiet)Console::Write(i);
					scores[i]++;
				}
				if(winner == 0){
					if(! quiet)Console::Write("T");
					scores[j] = scores[j] + 0.5;
					scores[i] = scores[i] + 0.5;
				}
				if(winner == 1){
					if(! quiet)Console::Write(j);
					scores[j]++;
				}
				if( ! game.supressOutput)game.showBoard();
				if(! quiet)Console::Write(" ");
			}				
		}
		if(! quiet)Console::WriteLine("");
	}
}



//selects the winners of the round
void Tournament::scoreRound(){
	if(! quiet)Console::WriteLine("scores:");
	//find the winning sims
	int t;//temp index translating from the winners-range to the scores-range

	for(int j=0; j<winnerSize; j++){//initialize the array
		winners[j] = -1;		
	}

	float minWinScore = setSize*setSize;//the score for the winner with the lowest score
	int minWinIndex = -1;//the index for the winner with the lowest score, on winnerSize range
	for(int i=0; i<setSize; i++){//find the winners
		if(! quiet)Console::Write(i+" "+scores[i]);//+" "+minWinIndex+" "+t+" "+minWinScore);//list all sim ids & their scores
		if(i<winnerSize){//the first few sims in the set are initialy assigned as winners
			winners[i] = i;
			if(scores[i] < minWinScore){//find the initial mins 
				minWinIndex = i;//the index of the lowest scoring winning sim
				minWinScore = scores[i];//the lowest winning score
			}
		}else{
			//Console::Write("\t"+scores[i]+" ? "+minWinScore+" -> ");//test
			if(scores[i] > minWinScore){//found a new winner - replace the old min-winner with this new one 
				winners[minWinIndex] = i;//replace the old lowest scoring winning-index with the index of the new winner
				minWinScore = scores[i];//replace with the  winning score

				//find the new mins
				//t = winners[0];
				//minWinScore = scores[t];
				//Console::Write("\tt:"+t+" => "+minWinScore);//test
				for(int j=0; j<winnerSize; j++){//ensure minWinScore is the minimium of the winning scores
					t = winners[j];
					//Console::Write("\tt:"+t+" => "+scores[t]+" ? "+minWinScore);//test	
					if(scores[t] < minWinScore){//the min
						minWinScore = scores[t];
						minWinIndex = j;
					}
					//Console::WriteLine("\t CHANGED "+minWinIndex+" & "+minWinScore);//test
				}					
			}//end if
			if(! quiet)Console::WriteLine("\t:: "+minWinIndex+" : "+minWinScore+"  ::  "+winners[0]+"@"+scores[winners[0]]+" : "+winners[1]+"@"+scores[winners[1]]);//test
		}
	}

	
	if(! quiet){
		Console::WriteLine("winners:");
		for(int j=0; j<winnerSize; j++){
			t = winners[j];
			Console::WriteLine(j+" @ "+sims[winners[j]].getID()+" = "+t+"  :  "+scores[t]);//list all winner ids & their scores
		}
	}else{
		int winnerIndex = -1;
		float winnerScore = -1;
		for(int j=0; j<winnerSize; j++){
			t = winners[j];
			if(scores[t] > winnerScore){
				winnerScore = scores[t];
				winnerIndex = j;
			}
		}
		Console::WriteLine(" :*: "+sims[winners[winnerIndex]].getID()+" @ "+winnerScore);
	}
}


//writes the brains of the winning sims
void Tournament::writeWinningBrains(){
	String^ simName = "s"+"1";

	//IO test
	//s1.writeBrain(simName);
	//s1.readBrain(simName);

	//remove the last generation seeds  
	DirectoryInfo^ di = gcnew DirectoryInfo(Environment::CurrentDirectory+"\\simBrains\\seeds");
    Collections::IEnumerator^ myEnum = di->GetFiles()->GetEnumerator();
    while (myEnum->MoveNext()) {
        FileInfo^ fiTemp = (FileInfo^)(myEnum->Current);
        //Console::WriteLine(fiTemp->Name);
		try{
			File::Delete("simBrains\\seeds\\"+fiTemp->Name);
		} catch (Exception^ e) {
			Console::WriteLine("Unable to delete seeds: {0}", e);
		}
    }

	//write winners to the seed directory
	for(int j=0; j<winnerSize; j++){
		simName = ""+roundCounter+"."+sims[winners[j]].getID();
		//sims[winners[j]].writeBrain(simName);//writes to the tournament directory
		sims[winners[j]].writeSeedBrain(simName);
	}
}
