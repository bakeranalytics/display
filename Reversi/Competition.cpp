#include "StdAfx.h"
#include "Competition.h"
#include <iostream>
#include <dirent.h> 
#include <vector>
#include <stdlib.h>
#include <stdio.h>



using namespace std;


Competition::Competition(void)
{
	quiet = true;
}

Competition::~Competition(void)
{
}


/* Plays the sims against each other.  Each is given the chance to go first once.
*/
void Competition::competeSims(const string* simNames){
	int matchWinner = 0;//keeps score of both games in the competition

	Simulant s1;
	if ( ! s1.readBrain(simNames[1]+".sim") ){
		cout << "sim " << simNames[1] << " cannot be read.";
		return;
	}

	Simulant s2;
	if( ! s2.readBrain(""+simNames[2]+".sim") ){
		cout << "sim " << simNames[2] << " cannot be read.";
		return;
	}

	Game game;
	int winner;
	//Console::Write("@"+s1.getID()+":"+s2.getID()+"_");//print contestants

	winner = game.compete(s1, s2);
	matchWinner += winner;
	if(winner == -1){cout << simNames[2];}
	if(winner == 0){cout << "A tie";}
	if(winner == 1){cout << simNames[1];}
	cout << " won the first game.";
	//game.showBoard();

	winner = game.compete(s2, s1);//reverse starting order
	matchWinner += -1 * winner;//polarity reversed with starting order
	if(winner == 1){cout << simNames[2];}
	if(winner == 0){cout << "A tie";}
	if(winner == -1){cout << simNames[1];}
	cout << " won the second game.";
	//game.showBoard();

	//display competition winner
	cout << "The winner was ";
	if(matchWinner < 0){cout << simNames[2]+".\n";}
	if(matchWinner == 0){cout << "a tie.\n";}
	if(matchWinner > 0){cout << simNames[1]+".\n";}
}




/* Plays the sets of sims against each other.
*/
void Competition::competeSimSets(const string* simDirs){
	cout << "competing Sim Sets: " << simDirs[1] << " & " << simDirs[2];
	Simulant *simSet1;
	Simulant *simSet2;
	
	DIR *dir;
    //struct dirent *dirp;
    vector<string> files;
	
	//char path[500];//_MAX_PATH  
	
	//strcpy(path, "X:\\");      /* fill string with form of response: X:\ */
  //path[0] = 'A' + getdisk();    /* replace X with current drive letter */
  //getcurdir(0, path+3);  /* fill rest of string with current directory */
	
	//GetCurrentPath(CurrentPath);
	 
	// display the path for demo purposes only
	//char temp[_MAX_PATH];
	//cout << path << endl;
	//cout << "Press Enter to continue";
	
	
    //DIR *dp;
    //dir = opendir(".\\simBrains\\"+simDirs[1].c_str());
    string path (".\\simBrains\\");
    path += simDirs[1];
    dir = opendir(".\\simBrains\\");	
    if(dir == NULL) {
        cout << "Error opening " << dir << endl;
    }

    while ((dirp = readdir(dp)) != NULL) {
        files.push_back(string(dirp->d_name));
    }
    closedir(dir);
    //return 0;
	
	
	
	//FileInfo^ fiTemp;
	//Collections::IEnumerator^ files;
	//DirectoryInfo^ dir;
	int i=0;//input index
	int setSize;
	int winner;
	Game game;
	Simulant s1, s2;
	vector <float> scores1;//score (number of games won) by each simulant in set 1;
	vector <float> scores2;//score (number of games won) by each simulant in set 2;
 
	//load first set
	//dir = gcnew DirectoryInfo(Environment::CurrentDirectory+"\\simBrains\\"+simDirs[1]);
	//files = dir->GetFiles()->GetEnumerator();
	setSize = files.size();//dir->GetFiles()->GetLength(0);
	simSet1 = new Simulant[setSize];
	if (!quiet) cout << "Set " << simDirs[1] << ":\t";
    while (files->MoveNext()) {
        fiTemp = (FileInfo^)(files->Current);
        if (!quiet) Console::Write(fiTemp->Name+"\t");
		try{
			simSet1[i].readBrain(simDirs[1]+"\\"+fiTemp->Name);//read in the sim brains
		} catch (Exception^ e) {
			Console::WriteLine("Unable to read sim: {0}", e);
		}
		i++;
    }
	if (!quiet) Console::WriteLine("  ");

	//load second set
	i = 0;
	dir = gcnew DirectoryInfo(Environment::CurrentDirectory+"\\simBrains\\"+simDirs[2]);
	files = dir->GetFiles()->GetEnumerator();
	if (setSize != dir->GetFiles()->GetLength(0)){
		Console::WriteLine("The directories do not contain the same number of sims.  Please make sure they do & restart. :)");
		return;
	}
	simSet2 = new Simulant[setSize];
	if (!quiet) Console::Write("Set "+simDirs[2]+":\t");
    while (files->MoveNext()) {
        fiTemp = (FileInfo^)(files->Current);
        if (!quiet) Console::Write(fiTemp->Name+"\t");
		try{
			simSet2[i].readBrain(simDirs[2]+"\\"+fiTemp->Name);//read in the sim brains
		} catch (Exception^ e) {
			Console::WriteLine("Unable to read sim: {0}", e);
		}
		i++;
    }
	if (!quiet) Console::WriteLine("  "); 

	//init scores	
	scores1.resize(setSize);
	scores2.resize(setSize); 

	//compete sets - each sim in set1 plays each sim in set2	
	for(int i=0; i<setSize; i++){
		s1 = simSet1[i];		
		for(int j=0; j<setSize; j++){
			s2 = simSet2[j];//[setSize - 1 - j];
			//Console::Write("@"+s1.getID()+":"+s2.getID()+"_");//print contestants

			winner = game.compete(s1, s2);
			if( ! game.supressOutput)
				if(! quiet)Console::Write("("+i+","+j+")");
			if(winner == -1){
				if(! quiet)Console::Write(2);
				scores2[j]++;
			}
			if(winner == 0){
				if(! quiet)Console::Write("T");
				scores2[j] = scores2[j] + 0.5;
				scores1[i] = scores1[i] + 0.5;
			}
			if(winner == 1){
				if(! quiet)Console::Write(1);
				scores1[i]++;
			}
			if( ! game.supressOutput)game.showBoard();

			winner = game.compete(s2, s1);//reverse starting order
			if(! quiet)Console::Write(":");
			if(winner == -1){
				if(! quiet)Console::Write(1);
				scores1[i]++;
			}
			if(winner == 0){
				if(! quiet)Console::Write("T");
				scores2[j] = scores2[j] + 0.5;
				scores1[i] = scores1[i] + 0.5;
			}
			if(winner == 1){
				if(! quiet)Console::Write(2);
				scores2[j]++;
			}
			if( ! game.supressOutput)game.showBoard();
			if(! quiet)Console::Write(" ");
		}
		if(! quiet)Console::WriteLine("");
	}
	
	//compute & display competition scores
	float scoreSum1 = 0, scoreSum2 = 0;
	Console::WriteLine("Competition scores:");
	for(int q=0; q<setSize; q++){
		scoreSum1 +=  scores1[q];
		scoreSum2 +=  scores2[q];
		if(! quiet) Console::WriteLine(q+"  "+scores1[q]+" : "+scores2[q]);
	}
	Console::WriteLine(scoreSum1+" : "+scoreSum2);


}



/* Plays the set of sims against uniform weight sims.
*/
void Competition::competeSimSetToUniformWeights(const string* simDirs){
	Console::WriteLine("competing Sim Set: "+simDirs[1]+" & uniform weight sims");
	Simulant *simSet1;
	Simulant *simSet2;
	FileInfo^ fiTemp;
	Collections::IEnumerator^ files;
	DirectoryInfo^ dir;
	int i=0;//input index
	int setSize;
	int winner;
	Game game;
	Simulant s1, s2;
	vector <float> scores1;//score (number of games won) by each simulant in set 1;
 
	//load first set
	dir = gcnew DirectoryInfo(Environment::CurrentDirectory+"\\simBrains\\"+simDirs[1]);
	files = dir->GetFiles()->GetEnumerator();
	setSize = dir->GetFiles()->GetLength(0);
	simSet1 = new Simulant[setSize];
	if (!quiet) Console::Write("Set "+simDirs[1]+":\t");
    while (files->MoveNext()) {
        fiTemp = (FileInfo^)(files->Current);
        if (!quiet) Console::Write(fiTemp->Name+"\t");
		try{
			simSet1[i].readBrain(simDirs[1]+"\\"+fiTemp->Name);//read in the sim brains
		} catch (Exception^ e) {
			Console::WriteLine("Unable to read sim: {0}", e);
		}
		i++;
    }
	if (!quiet) Console::WriteLine("  ");

	//init scores	
	scores1.resize(setSize);

	//compete sets - each sim in set1 plays each sim in set2	
	for(int i=0; i<setSize; i++){
		s1 = simSet1[i];	
		winner = game.competeUniformWeights(s1, s2, false);
		if(winner == -1){
			if(! quiet)Console::Write(2);
		}
		if(winner == 0){
			if(! quiet)Console::Write("T");
			scores1[i] = scores1[i] + 0.5;
		}
		if(winner == 1){
			if(! quiet)Console::Write(1);
			scores1[i]++;
		}
		if( ! game.supressOutput)game.showBoard();

		winner = game.competeUniformWeights(s2, s1, true);//reverse starting order
		if(! quiet)Console::Write(":");
		if(winner == -1){
			if(! quiet)Console::Write(1);
			scores1[i]++;
		}
		if(winner == 0){
			if(! quiet)Console::Write("T");
			scores1[i] = scores1[i] + 0.5;
		}
		if(winner == 1){
			if(! quiet)Console::Write(2);
		}
		if( ! game.supressOutput)game.showBoard();
		if(! quiet)Console::Write(" ");
	}
	
	//compute & display competition scores
	float scoreSum1 = 0, scoreSum2 = 0;
	Console::WriteLine("Competition scores:");
	for(int q=0; q<setSize; q++){
		scoreSum1 +=  scores1[q];
		if(! quiet) Console::WriteLine(q+"  "+scores1[q]);
	}
	scoreSum2 +=  2*setSize-scoreSum1;
	Console::WriteLine(scoreSum1+" : "+scoreSum2);


}




/* Plays the set of sims against max flip sims.
*/
void Competition::competeSimSetToMaxFlip(const string* simDirs){
	cout << "competing Sim Set: " << simDirs[1] << " & maxFlips";
	Simulant *simSet1;
	Simulant *simSet2;
	FileInfo^ fiTemp;
	Collections::IEnumerator^ files;
	DirectoryInfo^ dir;
	int i=0;//input index
	int setSize;
	int winner;
	Game game;
	Simulant s1, s2;
	vector <float> scores1;//score (number of games won) by each simulant in set 1;
 
	//load first set
	dir = gcnew DirectoryInfo(Environment::CurrentDirectory+"\\simBrains\\"+simDirs[1]);
	files = dir->GetFiles()->GetEnumerator();
	setSize = dir->GetFiles()->GetLength(0);
	simSet1 = new Simulant[setSize];
	if (!quiet) Console::Write("Set "+simDirs[1]+":\t");
    while (files->MoveNext()) {
        fiTemp = (FileInfo^)(files->Current);
        if (!quiet) Console::Write(fiTemp->Name+"\t");
		try{
			simSet1[i].readBrain(simDirs[1]+"\\"+fiTemp->Name);//read in the sim brains
		} catch (Exception^ e) {
			Console::WriteLine("Unable to read sim: {0}", e);
		}
		i++;
    }
	if (!quiet) Console::WriteLine("  ");

	//init scores	
	scores1.resize(setSize);

	//compete sets - each sim in set1 plays each sim in set2	
	for(int i=0; i<setSize; i++){
		s1 = simSet1[i];		
		winner = game.competeMaxFlip(s1, s2, false);
		if(winner == -1){
			if(! quiet)Console::Write(2);
		}
		if(winner == 0){
			if(! quiet)Console::Write("T");
			scores1[i] = scores1[i] + 0.5;
		}
		if(winner == 1){
			if(! quiet)Console::Write(1);
			scores1[i]++;
		}
		if( ! game.supressOutput)game.showBoard();

		winner = game.competeMaxFlip(s2, s1, true);//reverse starting order
		if(! quiet)Console::Write(":");
		if(winner == -1){
			if(! quiet)Console::Write(1);
			scores1[i]++;
		}
		if(winner == 0){
			if(! quiet)Console::Write("T");
			scores1[i] = scores1[i] + 0.5;
		}
		if(winner == 1){
			if(! quiet)Console::Write(2);
		}
		if( ! game.supressOutput)game.showBoard();
		if(! quiet)Console::Write(" ");
	}
	
	//compute & display competition scores
	float scoreSum1 = 0, scoreSum2 = 0;
	Console::WriteLine("Competition scores:");
	for(int q=0; q<setSize; q++){
		scoreSum1 +=  scores1[q];
		if(! quiet) Console::WriteLine(q+"  "+scores1[q]);
	}
	scoreSum2 +=  2*setSize-scoreSum1;
	Console::WriteLine(scoreSum1+" : "+scoreSum2);


}
