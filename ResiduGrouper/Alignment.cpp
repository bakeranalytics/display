/*  Alignment.cpp
 *  ResidueGrouper
 *  Created by brian on 8/27/09. */

#include "Alignment.h"

string alignmentName;
Properties p(1);//normalized mode
Phylogeny tree;
SubMatrix submat;

Alignment::Alignment(){}

/** Imports the file s, sets map with phylogeny, and computes the comparison matrixes. */
Alignment::Alignment(const string& s){
	alignmentName = s;
	valdarNorm = -1;
	import(s);
	//tree.printTree();//test
	//computeComparisonMatrixes();
}


/** Computes the SD for each position in the alignment.*/
void Alignment::spreadScan(){
	for(int i=0; i<seqMatrix[0].size(); i++){
		computeSD(i);
	}
}



/** Compute a distance (physiochemical) matrix between all residues at each position across the alignment. */
void Alignment::computeComparisonMatrixes(){
	//replace  computePChemDistanceMatrixes();
	//replace  computePhylogenicDistanceMatrixes();
	computeSubMatrixDistanceMatrixes();
}




/** Appends the physiochemical distance vs. the phylogenic distance for each combination of residue & species at each position into a single table in a file.  
 If the combination includes a gap (-1 flag value) or a single-species combination (phyl = 0)  it is not exported to the file.
 @param exportFileName the file name the data will be appended to; a file extension of .pvp is appended to the end of the specified file name.*/
void Alignment::exportPchemPhylCoordinates(const string& exportFileName){
	string fileNameOut (exportFileName);
	fileNameOut += ".pvp";
	cout <<"writting to: "<<fileNameOut<<endl;//test
	
	ofstream out(fileNameOut.c_str(), ios::app);
	if( ! out ){
		cerr << "Unable to open the export file: "<<fileNameOut;
		exit(3);
	}
	
	for(int r=0; r<phylogenicDistanceMatrixes.size(); r++){
		for(int x=0; x<phylogenicDistanceMatrixes[r].size(); x++){//each sequence residue at the position
			for(int y=0; y<phylogenicDistanceMatrixes[r][x].size(); y++){//each sequence residue at the position
				if (phylogenicDistanceMatrixes[r][x][y]!=-1 && pchemDistanceMatrixes[r][x][y]!=-1 && phylogenicDistanceMatrixes[r][x][y]!=0){
					out << phylogenicDistanceMatrixes[r][x][y] << "\t";
					out << pchemDistanceMatrixes[r][x][y] << "\t";		
					//out << x<<","<<y << ":\t" << seqMatrix[x][r]<<", "<< seqMatrix[y][r] << "\t";
					//out << seqNames[x]<<", "<<seqNames[y] << "\t";
					out << endl;
				}
			}
		}
	}
	
	cout <<endl;//test
	out.close();
	cout << "exported "<< fileNameOut << endl;
}





/** Truncates the alignment specfic pvp file, thencCalls exportPchemPhylCoordinates(const string& exportFileName) with alignment specific file name.  */
void Alignment::exportPchemPhylCoordinates(){
	string fileNameOut (fileName);
	fileNameOut += ".pvp";	
	ofstream out(fileNameOut.c_str(), ios::trunc);
	if( ! out ){
		cerr << "Unable to truncate the file: "<<fileNameOut;
		exit(3);
	}
	
	exportPchemPhylCoordinates(fileName);
}




/** Prints the physiochemical distance vs. the phylogenic distance for each combination of residue & species at one position 
 @param r the residue position to be printed.
 */
void Alignment::printPchemPhylCoordinates(int r){
	cout << "phylo vs pchem @ "<< r+1 << endl;
	for(int x=0; x<phylogenicDistanceMatrixes[r].size(); x++){//each sequence residue at the position
		for(int y=0; y<phylogenicDistanceMatrixes[r][x].size(); y++){//each sequence residue at the position
			//cout << phylogenicDistancefile;//localhost/Users/brian/Desktop/bench/appDev/ResidueGrouper/Alignment.cppMatrixes[r][x][y] << "\t";
			cout << pchemDistanceMatrixes[r][x][y] << "\t";		
			cout << x<<","<<y << ":\t" << seqMatrix[x][r]<<", "<< seqMatrix[y][r] << "\t";
			cout << seqNames[x]<<", "<<seqNames[y] << "\t";
			cout << endl;
		}
	}
	cout << endl;
}




/** Compute a phylogenic distance matrix between all residues at each position across the alignment. */
void Alignment::computePhylogenicDistanceMatrixes(){
	string res;//residue symbol
	double dist;//absolute value of distance between two residue
	int gap = 0;//0 -> not a gap, -1 -> gap
	int a,b;//phylogeny indexes
	
	cout <<"Computing phylogenic distance matrixes:" <<"\n";
	
	for(int r=0; r<seqMatrix[0].size(); r++){//each sequence position
		for(int i=0; i<seqNames.size(); i++){//each sequence residue at the position
			phylogenicDistanceMatrixes[r].push_back(vector<double>());
			if( (res=seqMatrix[i][r]) == "-" ){//gap
				gap = -1;
			}else{//nongap
				a = seqTreeNum[i];
			}
			for(int j=0; j<=i; j++){//each residue at the position, diagonal matrix
				if( (res=seqMatrix[j][r]) == "-" || gap == -1){//gap
					phylogenicDistanceMatrixes[r][i].push_back(-1);
				}else{//nongap
					b = seqTreeNum[j];
					dist = tree.getDistance(a, b);
					phylogenicDistanceMatrixes[r][i].push_back(dist);
				}
				//cout <<"*"<< res<<","<<phylogenicDistanceMatrixes[r][i][j]<<"\t";//test
			}
			gap = 0;
		}		
		//cout<<endl;//test
		//printPhylogenicDistanceMatrix(r);//test
	}
}

	


/** Compute a physiochemical distance matrix between all residues at each position across the alignment. */
void Alignment::computePChemDistanceMatrixes(){
	string res;//residue symbol
	double dist;//absolute value of distance between two residue
	vector<double> point1;//the coordinates for the reference residue at the given position 
	vector<double> point2;//the coordinates for the comparative residue at the given position 
	int gap = 0;//0 -> not a gap, -1 -> gap
	
	cout <<"Computing physiochemical distance matrixes:" <<"\n";
	
	for(int r=0; r<seqMatrix[0].size(); r++){//each sequence position
		for(int i=0; i<seqNames.size(); i++){//each sequence residue at the position
			pchemDistanceMatrixes[r].push_back(vector<double>());
			if( (res=seqMatrix[i][r]) == "-" ){//gap
				gap = -1;
			}else{//nongap
				point1 = p.fingerprint(res);
			}
			for(int j=0; j<=i; j++){//each residue at the position, diagonal matrix
				if( (res=seqMatrix[j][r]) == "-" || gap == -1){//gap
					pchemDistanceMatrixes[r][i].push_back(-1);
				}else{//nongap
					point2 = p.fingerprint(res = seqMatrix[j][r]);
					dist = pow( (point2[0] - point1[0]), 2 ) + pow( (point2[1] - point1[1]), 2 ) + pow( (point2[2] - point1[2]), 2 );
					pchemDistanceMatrixes[r][i].push_back(sqrt(dist));
				}
				//cout <<"*"<< res<<","<<pchemDistanceMatrixes[r][i][j]<<"\t";//test
			}
			gap = 0;
		}		
		//cout<<endl;//test
		//printPchemDistanceMatrix(r);//test
	}
}


/** computes, by delegation, the conservation at each position of all alignments using Valdar's method and appends the info to the specified file. */
void Alignment::conservationScores(string exportFileName){
	double score = 0;	
	//header, species by column
	//for(int i=0; i<tree.dim; i++){
	//	cout << tree.treeNumberVec[i]<<"\t";
	//}
	
	
	//write to file
	string fileNameOut (exportFileName);
	fileNameOut += ".vld";//for valdar score
	cout <<"writting to: "<<fileNameOut<<"  ..";//test
	
	ofstream out(fileNameOut.c_str(), ios::app);
	if( ! out ){
		cerr << "Unable to open the export file: "<<fileNameOut<<endl;
		exit(3);
	}
	for(int i=0; i<seqMatrix[0].size(); i++){//each position
		score = conservationScore(i);
		out<<alignmentName<<":"<<i+1 <<"\t"<< score<<"\t";//position & score
		for(int j=0; j<seqNames.size(); j++){//each sequence
			out<< seqMatrix[j][i]<<"\t";//residue
		}
		out << "\n";
	}
	out.close();
	cout << ".. exported " << endl;
	
	
}



/** Finds the RxRxxS/T motif in the human sequence and scores the region around the motif and writes the score to a file.  
 Default output (full=0) is numeric.
 NOTE: be sure to check the SubMatrix input file - there is one modified to treat S<>T substituions as conserved.
 NOTE: this will miss a motif at the very end of the sequence.
 @param full if !=0 flags output of all sequences and quazi-rounded single digit scores; meant for visual inspection.*/
void Alignment::motifSearch(const string& fileName, int full){
	int radius = 5;//the number of positions prior to and after the motif that are considered
	double insertionThreshold = 0.5;//percent of sequences at a non-motif site that must be non-gap for the column to be scored
	vector<int> positions;//the positions (columns) to be scored
	vector<int> prePositions;//the positions (columns) to be scored before th motif
	vector<int> postPositions;//the positions (columns) to be scored after the motif
	//double preMotifIndex, postMotifIndex;//indexes for the begining & ending of the site, will equal the radius plus the number of columns above the insertionThreshold.
	int motifLength = 6;//index + motifLength is the end of the motif
	double gapThreshold = .5;//sequences with more than this percentage of gap positions will not be included in the scoring
	double gapPercent;//tmp variable of the percent of motif positions that are gaps	cout << "motifSearch RxRxxS/T "<<seqMatrix[0].size()<<endl;//test
	int hi;//humanIndex
	int adj = 0;//temp to adjust for motif at first of alignment
	int k;//temp
	double tmp;//temp
	double mtfAvg, mtfRAvg, mtfXAvg, bkgrdAvg, siteAvg, mbRatio;//averages for all motif; R,R,S/T only; motif x only; pre & post positions; 
	double depth;//the number of non-excluded species in the alignment
	Distribution dist;
	
	string fileNameOut (fileName);
	fileNameOut += ".RxRxxS-T.mtf";
	ofstream out(fileNameOut.c_str(), ios::app);
	if( ! out ){
		cerr << "Unable to open the export file: "<<fileNameOut;
		exit(3);
	}
	cout <<"writting to: "<<fileNameOut<<endl;
	
	//find the human sequence index
	for(int i=0; i<seqNames.size(); i++){
		string::size_type flag;
		flag = seqNames[i].find("Hsap");
		if(flag != string::npos){
			hi = i;
		}
	}
	
	
	for(int j=0; j<seqMatrix[hi].size(); j++){
		if(seqMatrix[hi][j] == 'R'){//R
			if(j+2 < seqMatrix[hi].size() && seqMatrix[hi][j+2] == 'R'){//RxR
				if(j+5 < seqMatrix[0].size() && (seqMatrix[0][j+5] == 'S' || seqMatrix[0][j+5] == 'T')){//RxRxxS/T
					cout <<"motif found at "<<j+1<<"   "<<fileNameOut<<endl;//test
					
					for(int i=0; i<motifGapFlag.size(); i++){//restore tree to previous state & clear the motifGapFlags
						if(motifGapFlag[i] != 0){//reset
							tree.includeSpecies(seqNames[i]);
							motifGapFlag[i] = 0;
						}
					}
										
					if(j < radius){//compensate for window if motif found at begining of sequence
						//for(int w=0; w<radius; w++){
							adj = j;
							//if(j == w){
								//adj = radius - w;
							//	j = j + adj;
								cout<<"adj: "<< adj <<endl;//test
							//}
						//}
					}
					
					
					//filter out sequences with gaps across n% of the motif range (radius-motif-radius)
					//cout << "gap count at: "<< j << "+/-"<<radius<<endl;//test
					for(int i=0; i<seqMatrix.size(); i++){
						//cout << seqNames[i]<<"\t"<<gapCountSeq(i,j,j+motifLength)<<endl;//test
						gapPercent = gapCountSeq(i,j,j+motifLength);	
						if(gapPercent > gapThreshold){
							motifGapFlag[i] = 1;//flag the sequence for omission from conservation
							tree.excludeSpecies(seqTreeNum[i]);
						}					
						//cout << seqNames[i]<<"\t"<<gapPercent<<"\t"<<motifGapFlag[i]<<endl;//test
					}
					//cout << endl;//test
					tree.computeTreeAverages();
					valdarNorm = -1;//flag to recompute norm
					
					
					//set site positions to be scored - filter out site (non-motif) columns with high gap content
					positions.resize(0);					
					prePositions.resize(0);					
					postPositions.resize(0);
					depth = 0;
					for(int i=0; i<seqMatrix.size(); i++){//compute depth - the number of non-excluded species in the alignment
						if(motifGapFlag[i] == 0){//skip excluded species
							depth++;
						}
					}
					//cout<<"depth: "<< depth <<endl;//test
					//tmp = 0;//counts non-insertion colums
					k = 0;//column counter
					while(postPositions.size() < radius && j+motifLength+k < seqMatrix[hi].size()){//stop when reaching the radius or the end of the sequence
						//if(j<600) cout<<"post?? "<<j+1<<":"<<j+motifLength+k<<" :: "<<((double)gapCount(j+motifLength+k) )<<" :: "<<((double)gapCount(j+motifLength+k) / depth)<<endl;//test
						if((double)gapCount(j+motifLength+k) / depth < insertionThreshold){
							postPositions.push_back(j+motifLength+k);
							//cout<<"post "<<j+motifLength+k<<endl;//test				
						}
						k++;
					}
					k = 1;//column counter
					while(prePositions.size() < radius-adj && j-k > 0){//stop when reaching the radius or the begining of the sequence
						if(((double)gapCount(j-k) / depth) < insertionThreshold){
							prePositions.push_back(j-k);
						}
						k++;
					}
					//collate pre, motif, & post positions
					for(int i=prePositions.size()-1; i>=0; i--){//get prepositions
						positions.push_back(prePositions[i]);
					}
					for(int i=j; i<j+motifLength; i++){//get motif positions
						positions.push_back(i);					
					}
					for(int i=0; i<postPositions.size(); i++){//get postpositions
						positions.push_back(postPositions[i]);
					}
					
					//for(int i=0; i<positions.size(); i++){//test
					//	cout << "--- positions: "<<positions[i]+1<< "   "<< seqMatrix[0][positions[i]]  <<endl;//test						
					//}
										
					
					//score, average, & write to file
					out << alignmentName << ":"<<1+j<<":";
					for(int i=0; i<positions.size(); i++){//residue
						if(i == radius-adj || i == radius-adj+motifLength){//formating - place a space between the site & motif positions
							out << " ";
						}
						out << seqMatrix[hi][positions[i]];
					}
					
					//for(int q=j-radius; q<=j+motifLength+radius; q++){
					//	out << seqMatrix[0][q];
					//}
					mtfAvg=0; mtfRAvg=0; mtfXAvg=0; bkgrdAvg=0; siteAvg=0; mbRatio=0;//reset for each motif found
					out <<"\t";
					for(int i=0; i<positions.size(); i++){//score & compute averages
						if(full != 0 ){//full output - short scores
							if(i == radius-adj || i == radius-adj+motifLength){//formating - place a space between the site & motif positions
								out << " ";
							}	
							tmp = conservationScoreDigit(positions[i]);
							out << tmp;
							if(i < radius-adj){//pre-motif background
								bkgrdAvg += tmp;
							}else if(i < radius + motifLength -adj){//motif
								mtfAvg += tmp;
								if(i==radius - adj || i==radius - adj + 2 || i==radius - adj + 5){
									mtfRAvg += tmp;
									//out<<"r";//test
								}else{
									//out<<"x";//test
									mtfXAvg += tmp;
								}
							}else{//post motif background
								bkgrdAvg += tmp;
							}
							siteAvg += tmp;
								
						}else{//numeric output only
							//out << conservationScore(positions[i])<<"\t";
							tmp = conservationScore(positions[i]);
							out << tmp <<"\t";
							if(i < radius - adj){//pre-motif background
								bkgrdAvg += tmp;
							}else if(i < radius + motifLength - adj){//motif
								mtfAvg += tmp;
								if(i==radius - adj || i==radius - adj + 2 || i==radius - adj + 5){
									mtfRAvg += tmp;
								}else{
									mtfXAvg += tmp;
								}
							}else{//post motif background
								bkgrdAvg += tmp;
							}
							siteAvg += tmp;
						}
					}
					mtfRAvg = mtfRAvg / 3;
					mtfXAvg = mtfXAvg / 3;
					mtfAvg = mtfAvg / motifLength;//not reported
					bkgrdAvg = bkgrdAvg / (2 * radius - adj);
					siteAvg = siteAvg / positions.size();//not reported
					out <<"\tMr:\t"<<mtfRAvg<<"\tMx:\t"<<mtfXAvg<<"\tBg:\t"<<bkgrdAvg<<"\tMr/Mx:\t"<<mtfRAvg/mtfXAvg;
					out <<"\n";
					//mtfAvg, bkgrdAvg, siteAvg, mbRatio
					
					//out <<"\t\t\txxRxRxxS/Txx\t\n";
					if(full != 0 ){//full output - sequences
						for(int i=0; i<seqMatrix.size(); i++){
							if(motifGapFlag[i] == 0){//skip excluded species								
								out << seqNames[i]<<"\t";
								for(int q=0; q<positions.size(); q++){
									if(q == radius - adj || q == radius+motifLength - adj){//formating - place a space between the site & motif positions
										out << " ";
									}							
									out << seqMatrix[i][positions[q]];
								}
								out<<endl;
							}
						}
						out << "\n";
					}
										
					if(adj != 0){ //compensate for window if motif at begining of sequence
					//	j = j - adj;  
						out << "****MOTIF AT BEGINNING - SHIFTED SITE POSITIONING!!!  adj="<<adj<<"   ***  "; 
						adj = 0;
					}
					
				}
			}
				
		}
	}
	out.close();
	
	
	//DISTRIBUTION -- sum over each aa type at each site position, print the matrix in sum & probablility format
	//create a Distribution object that holds a vector (for each position) of arrays (for each aa type) - look at Properties for template
	
}



/** Returns the number of gaps a the specified sequence position. */
int Alignment::gapCount(int r){	
	string res;//residue symbol
	int gaps = 0;//gap counter
	
	for(int i=0; i<seqNames.size(); i++){//check each sequence at position r
		if(motifGapFlag[i] == 0){//sequence not removed from computation for high gap content
			if ( (res = seqMatrix[i][r]) == "-")//if residue is a gap
				gaps++;			
		}
	}
	return gaps;	
}


/** computes the percent of positions that are gaps for the specified sequence number between the specified range.*/
double Alignment::gapCountSeq(int seq, int start, int end){
	double percent;//the percent of the positions that are gaps
	string res;//residue symbol
	int gaps = 0;//gap counter
	
	for(int i=start; i<=end; i++){//check each position in the window for the sequence
		if ( (res = seqMatrix[seq][i]) == "-")//if residue is a gap
			gaps++;
	}
	
	percent = (double)gaps / (double)(1 + end - start);
	return percent;	
}




/** Returns the a single digit score = score * 10 and rounded to a single digit.  This is wiered -> replace it with something better... 
 0 - 0.1 => 0
 0.1 - 0.2 => 0
 0.2 - 0.3 => 1
 0.3 - 0.4 => 2
 .
 .
 0.8 - 0.9 =>7
 0.9 - 0.9999999 => 8
 1 => 9
 
 */
int Alignment::conservationScoreDigit(int p){
	int score = 10 * conservationScore(p) - 1;
	if(score < 0) score = 0;
	//cout <<  conservationScore(p) <<"  "<< score<<endl;//test
	
	return score;
}


/**
 computes the position's conservation using Valdar's method. */
double Alignment::conservationScore(int p){
	double score = 0, tmp=0;
	string r1, r2;
	
	//-- check for motifGapFlag !=0 anywhere -> exclude sequence from tree & recompute averages
	
	if(valdarNorm == -1){//flag that the normalization factor hasn't been computed yet
		computeValdarNormalization();
	}
	//cout<<p<<" :: ";//test

	for(int i=0; i<seqNames.size(); i++){//each residue combination at p
		if(motifGapFlag[i] == 0){//sequence not removed from scoring for high gap content
			for(int j=i+1; j<seqNames.size(); j++){
				//cout<< seqMatrix[i][p]<<":"<<seqMatrix[j][p]<<":";//test
				if(motifGapFlag[j] == 0){//sequence not removed from scoring for high gap content
					r1 = seqMatrix[i][p];
					r2 = seqMatrix[j][p];
					tmp =  tree.treeAverages[seqTreeNum[i]] *  tree.treeAverages[seqTreeNum[j]] * submat.distance(r1, r2);
					score += tmp;
					//cout<<"   :: "<<r1<<r2<<" :: " << tree.treeAverages[seqTreeNum[i]] <<" * "  << tree.treeAverages[seqTreeNum[j]] <<  " * "<<submat.distance(r1, r2)<<" = "<< tmp<<"   -> "<<score<<"\n";//test
				}
			}
		}
	}
	//cout<<"   :: "<<score<< " / "<<valdarNorm<<" = ";//test
	//cout<< score<<"\n";//test

	score = score / valdarNorm;	
	//cout<< score<<"\t";//test
	return score;
}



void Alignment::computeValdarNormalization(){
	valdarNorm = 0;
	for(int i=0; i<seqNames.size(); i++){//each sequence
		if(motifGapFlag[i] == 0){//sequence not removed from scoring for high gap content
			for(int j=i+1; j<seqNames.size(); j++){
				if(motifGapFlag[j] == 0){//sequence not removed from scoring for high gap content
					valdarNorm += tree.treeAverages[seqTreeNum[i]] *  tree.treeAverages[seqTreeNum[j]];
					//cout<<"-> valdarNorm: "<<seqNames[i]<<" : "<<seqNames[j]<< tree.treeAverages[seqTreeNum[i]] << " * " << tree.treeAverages[seqTreeNum[j]]<< "  => "<< valdarNorm << endl;//test
				}
			}
		}
	}
	//cout<< "valdarNorm\t" << valdarNorm <<endl;//test
	
}



/** Compute a substitution matrix based distance matrix between all residues at each position across the alignment. 
  BLOSUM NOT SUPPORTED YET
 */
void Alignment::computeSubMatrixDistanceMatrixes(){
	string res;//residue symbol
	//double dist;//absolute value of distance between two residue
	//int gap = 0;//0 -> not a gap, -1 -> gap
	
	double test = submat.distance("F","L");
	cout <<"Computing submat distance matrixes: -function not completed: " << test <<"\n";
	
	
	
	/*
	for(int r=0; r<seqMatrix[0].size(); r++){//each sequence position
		for(int i=0; i<seqNames.size(); i++){//each sequence residue at the position
			blosumDistanceMatrixes[r].push_back(vector<double>());
			if( (res=seqMatrix[i][r]) == "-" ){//gap
				gap = -1;
			}else{//nongap
				point1 = p.fingerprint(res);
			}
			for(int j=0; j<=i; j++){//each residue at the position, diagonal matrix
				if( (res=seqMatrix[j][r]) == "-" || gap == -1){//gap
					pchemDistanceMatrixes[r][i].push_back(-1);
				}else{//nongap
					point2 = p.fingerprint(res = seqMatrix[j][r]);
					dist = pow( (point2[0] - point1[0]), 2 ) + pow( (point2[1] - point1[1]), 2 ) + pow( (point2[2] - point1[2]), 2 );
					pchemDistanceMatrixes[r][i].push_back(sqrt(dist));
				}
				//cout <<"*"<< res<<","<<pchemDistanceMatrixes[r][i][j]<<"\t";//test
			}
			gap = 0;
		}		
		//cout<<endl;//test
		//printPchemDistanceMatrix(r);//test
	}	
	*/
}




/** Prints the physiochemical distance matrix 
 @param r the residue position to be printed.
 */
void Alignment::printPchemDistanceMatrix(int r){
	cout << "@"<< r+1 << "\t";
	for(int x=0; x<seqNames.size(); x++){//each sequence residue at the position
		cout << x << ":" << seqMatrix[x][r] << "\t";
	}		
	cout << endl;
	for(int x=0; x<pchemDistanceMatrixes[r].size(); x++){//each sequence residue at the position
		cout << x << ":" << seqMatrix[x][r] << "\t";
		for(int y=0; y<pchemDistanceMatrixes[r][x].size(); y++){//each sequence residue at the position
			cout << pchemDistanceMatrixes[r][x][y] << "\t";		
		}
		cout << endl;
	}			
	cout << endl;
}




/** Prints the phylogenic distance matrix 
 @param r the residue position to be printed.
 */
void Alignment::printPhylogenicDistanceMatrix(int r){
	cout << "@"<< r+1 << "\t";
	for(int x=0; x<seqNames.size(); x++){//each sequence residue at the position
		cout << x << ":" << seqMatrix[x][r] << "\t";
	}		
	cout << endl;
	for(int x=0; x<phylogenicDistanceMatrixes[r].size(); x++){//each sequence residue at the position
		cout << x << ":" << seqMatrix[x][r] << "\t";
		for(int y=0; y<phylogenicDistanceMatrixes[r][x].size(); y++){//each sequence residue at the position
			cout << phylogenicDistanceMatrixes[r][x][y] << "\t";
		}
		cout << endl;
	}			
	cout << endl;
}





/** Prints the substitution distance matrix (blosum, pet, etc.) - not finished yet.
 @param r the residue position to be printed.
 */
void Alignment::printSubMatrixDistanceMatrix(int r){
	cout << "@"<< r+1 << "\t";
	for(int x=0; x<seqNames.size(); x++){//each sequence residue at the position
		cout << x << ":" << seqMatrix[x][r] << "\t";
	}		
	cout << endl;
	for(int x=0; x<pchemDistanceMatrixes[r].size(); x++){//each sequence residue at the position
		cout << x << ":" << seqMatrix[x][r] << "\t";
		for(int y=0; y<pchemDistanceMatrixes[r][x].size(); y++){//each sequence residue at the position
			cout << pchemDistanceMatrixes[r][x][y] << "\t";
			//put BLOSUM distances here
		}
		cout << endl;
	}			
	cout << endl;
}




/** Computes the standard deviation and maximal residue distance to the centroid at a give position, r, across the property space; they are stored in the spread and maxDist vectors respectively.  
 The residue count used for the computation is taken as the number of sequences less the number of gaps; gaps therby increase the 'spread' of the data.
 Writes the SD and maximum distance to the command line when they exceed arbitrary thresholds.
 @return the standard deviation at position r */
double Alignment::computeSD(int r){	
	string res;//residue symbol
	double sums[p.getPropertyCount()];//holds the sum of all coordinates
	double avg[p.getPropertyCount()];//holds the average across of all coordinate points for each property
	double sd;//temp variable that holds values in computing standard deviation: summed squared difference between each point and the centroid; variance; SD
	vector<double> point;//the coordinates for one residue at the position r
	int residueCount = seqNames.size() - gapCount(r);//the number of non-gap residues at position r"\t"<<resi
	double dist;//square distance from a point to the centroid
	
	// compute sum of values over all sequences for each property dimension
	int noGap = 0;
	while ( (res=seqMatrix[noGap][r]) == "-" ){//find first non-gap sequence
		noGap++;
	}
	point = p.fingerprint(res = seqMatrix[noGap][r]);//initialize with first non-gap point's values
	for (int i=0; i<p.getPropertyCount(); i++) {
		sums[i] = point[i];
	}
	for(int x = noGap + 1; x<seqNames.size(); x++){//sum over remaining sequence residues
		if( (res=seqMatrix[noGap][r]) != "-" ){
			point = p.fingerprint(res = seqMatrix[x][r]);
			for(int i=0; i<point.size(); i++){
				sums[i] += point[i];			}
		}
	}
	
	//compute average value of each property dimension
	for(int i=0; i<p.getPropertyCount(); i++){//compute averages
		avg[i] = sums[i] /residueCount;
	}
	
	//compute squared difference between average and individual residues - variable 'variance' is used as a temp to hold this value
	maxDist[r] = 0;
	for(int i=0; i<seqNames.size(); i++){
		if( (res=seqMatrix[i][r]) != "-" ){//if not a gap
			point = p.fingerprint(res = seqMatrix[i][r]);
			dist = pow( (avg[0] - point[0]), 2 ) + pow( (avg[1] - point[1]), 2 ) + pow( (avg[2] - point[2]), 2 );
			if(dist > maxDist[r]){
				maxDist[r] = sqrt(dist);
			}
			//cout <<"r"<<i<<"::"<<res<<"  "<<point[0]<<"  "<<point[1]<<"  "<<point[2]<<"     "<< sqrt(dist) << endl;//test
			sd += dist;
		}
	}
	sd = sd / (residueCount - 1);//sd is the variance here
	sd = sqrt(sd);
	standardDeviation[r] = sd;
	//cout <<"@" << r+1 << " SD = " << sd << "\t maxDist = " << maxDist[r] << endl;//test
	
	if(sd > 1 || maxDist[r] > 1)//arbitrary thresholds
		cout <<"@" << r+1 << " SD = \t" << sd << "\t maxDist = \t" << maxDist[r] << endl;
	
	return sd;
}



/* Import the alignment file and initialize the standardDeviation, maxDist, and pchemDistanceMatrixes vectors. 
 //send the seqNames to Phylogeny where it will be compared with the known species set.
 //if there is no phylogenic distance known for the species it is rejected
 // if it matches a known species, it is added to the treeNumber vector which maps from the tree matrix to the species position in the alignment file
 // later, another method the distances (phylogenic and physiochemical) will be correlated
 */
void Alignment::import(const string& s){
	fileName = s;
	cout << "import "<< fileName <<" \n";
	
	ifstream in(fileName.c_str(), ios::in);
	if( ! in ){
		cerr << "Unable to open the alignment file: "<<fileName;
		exit(2);
	}
	
	int treenum;
	char line[1000];//temp - input line from file
	while(!(in.eof())){ 
		in.getline(line, 1000, '\n');
		if(line[0]=='>'){//name line	
			
			//-- exclude fish -- string match to fish
			
			//cout << "treenum " << tree.getTreeNumber(line) << endl;//test
			treenum = tree.includeSpecies(line);
			if(treenum == -1){//no matching species
				cout << "   **  Rejected: No phylogeny for " << line<<endl;
			}else{		
				seqNames.push_back(line);
				seqTreeNum.push_back(treenum);
				seqMatrix.push_back(vector<char>());
				propertyMatrix.push_back(vector<Properties>());	
				cout << "\tLoaded: " <<treenum<< "  "<<line<<endl;			
			}
		}else{//sequence line
			for(int i=0; i<strlen(line); i++){//each residue
				seqMatrix[seqMatrix.size()-1].push_back(line[i]);
			}
		}
	}
	tree.computeTreeAverages();
	
	standardDeviation.resize(seqMatrix[0].size());
	maxDist.resize(seqMatrix[0].size());
	pchemDistanceMatrixes.resize(seqMatrix[0].size());
	phylogenicDistanceMatrixes.resize(seqMatrix[0].size());
	blosumDistanceMatrixes.resize(seqMatrix[0].size());
	motifGapFlag.resize(seqMatrix[0].size());
	
	for(int i=0; i<motifGapFlag.size(); i++){//initialize the motifGapFlags to include all sequences in scoring computations--
		motifGapFlag[i] = 0;
	}

	cout << endl;		
}


/** Prints, to the console, a table of the physiochemical properties at the specified seqeunce position. */
void Alignment::printPropMap(int r){
	string res;
	
	p.printPropertyNames();
	for(int i=0; i<seqNames.size(); i++){
		res = seqMatrix[i][r];
		p.fingerprint(res);
		cout << seqNames[i] << "\t";
		p.printPropertyValues();
	}
}


/* Print sequence data to command line. */
void Alignment::printAlignment(){
	//cout << "\nSequence alignment data for: "<< fileName <<" \n";
	for(int i=0; i<seqNames.size(); i++){
		cout << seqNames[i] << "\n";
		for(int j=0; j<seqMatrix[i].size(); j++){
			cout<< seqMatrix[i][j];			
		}
		cout << "\n";
	}
}


/* Print sequence summary data (dimensions & names) to command line. */
void Alignment::printSummary(){
	cout << "\nSequence summary data for: "<< fileName <<" \n";
	cout << "Alignment dimensions: "<< seqNames.size() <<" sequences X "<< seqMatrix[0].size() <<" residues\n";
	
	//print matrix content
	for(int i=0; i<seqNames.size(); i++){
		cout << seqNames[i] << "\n";
	}
}


//////// PRIVATE FUNCTIONS  ////////////