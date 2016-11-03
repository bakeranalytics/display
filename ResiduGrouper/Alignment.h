/*  Alignment.h
 *  ResidueGrouper
 *  Created by brian on 8/27/09. */

#include "Properties.h"
#include "SubMatrix.h"
#include "Phylogeny.h"
#include "Distribution.h"
#include <fstream>
#include "tnt.h"

using namespace TNT;

class Alignment{
public:	
	Alignment();
	Alignment(const string& fileName);//auto import
	~Alignment(){};
	
	double conservationScore(int p);//the conservation score at the indicated position
	//void conservationScores();//the conservation score at all positions 
	void conservationScores(string exportFileName);//the conservation score at all positions for all alignments
	void import(const string& fileName);
	void computeComparisonMatrixes();
	void spreadScan();//computes the SD and maxDistance for each position
	double computeSD(int r);//computes the standard deviation in property position at the specified position
	void printPropMap(int r);//prints property space coordinates for all residues at the specified alignment position
	void printSummary();	
	void printAlignment();
	int gapCount(int r);//returns the number of gaps at the specified position
	double gapCountSeq(int seq, int start, int end);//returns the number of gaps in a specified sequence between the the specified positions
	void printPchemDistanceMatrix(int r);//prints the distance matrix at the position provided	
	void printPhylogenicDistanceMatrix(int r);//prints the distance matrix at the position provided		
	void printSubMatrixDistanceMatrix(int r);//prints the distance matrix at the position provided	
	void printPchemPhylCoordinates(int r);//prints pchem vs phylo coord for ploting 
	void exportPchemPhylCoordinates();//writes all pchem vs phylo coords into an alignment specific file
	void exportPchemPhylCoordinates(const string& exportFileName);//appends all pchem vs phylo coords into the specified file
	
	void motifSearch(const string& fileName, int full=0);//identifies a motif (RxRxxS/T) on the human sequence and computes the conservation in the motif region; full=0 for numeric output, full!=0 for all sequences;writes to RxRxxS-T.mtf
	//void motifSearchWriteAll(int file=0);//identifies a motif on the human sequence and computes the conservation in the motif region; writes all sequences.  file=0 for no file output, cmd line output instead
	//void motifSearchCSV(int file=0);//identifies a motif on the human sequence and computes the conservation in the motif region - csv file format.  file=0 for no file output, cmd line output instead
	int conservationScoreDigit(int P);//returens a single digit score equal to 10 times the score rounded down
	
private:
	string fileName;
	vector<string> seqNames;
	vector<int> seqTreeNum;
	vector<vector <char> > seqMatrix;
	vector<vector <Properties> > propertyMatrix;
	vector<double> standardDeviation;//the standard deviation at each position
	vector<double> maxDist;//the maximum distance of a residue from the centroid at each position
	double valdarNorm;//the normalization factor used in valdar scoring.  set to -1 when unintitalized
	vector<int> motifGapFlag;//indicates which sequences are to be omitted from scoring & gap counts; 0 => use for computations
	
	//comparison matrixes - intended for use with normalized metrics only
	vector<vector<vector <double> > > pchemDistanceMatrixes;//physichemical distance of each residue's coordiante vector with all others at that position, for each position
	vector<vector<vector <double> > > phylogenicDistanceMatrixes;//physichemical distance of each residue's coordiante vector with all others at that position, for each position
	vector<vector<vector <double> > > blosumDistanceMatrixes;//physichemical distance of each residue's coordiante vector with all others at that position, for each position
	vector<vector <double> > ipMatrix;//inner product of each positions coordiante vector with all other positions
	
	void computePChemDistanceMatrixes();
	void computePhylogenicDistanceMatrixes();
	void computeSubMatrixDistanceMatrixes();
	void computeValdarNormalization();
	
};
