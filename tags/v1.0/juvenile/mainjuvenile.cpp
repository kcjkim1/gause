#include <cvutils.h>
#include <img.h>
#include <dirit.h>
#include <math.h>
#include <popt.h>

#include "juvenileutils.h"
#include "GaborFilter.h"
#include "MinimumDimensionExtractor.h"

#include <ga/GASStateGA.h>
#include <ga/GASimpleGA.h>
#include <ga/GABin2DecGenome.h>

using namespace std;

#define MAX_DIRECTORY_SIZE 256

char imagePath[MAX_DIRECTORY_SIZE];
char outputPath[MAX_DIRECTORY_SIZE];
char trainingDirectory[MAX_DIRECTORY_SIZE];
int numberOfOrientations;
int numberOfScales;
int lower, upper;
int populationSize;
bool detectFlag = false;
bool gaFlag = false;

int processOptions(int argc, const char* argv[]) {
	struct poptOption table[] = {
		{"image-path", 'd', POPT_ARG_STRING, imagePath, 'd',
		 "The path to an image to detect", "A valid path"},
		{"output-path", 'o', POPT_ARG_STRING, outputPath, 'o',
		 "The path to the output image", "A valid path"},
		{"orientations", 'r', POPT_ARG_INT, &numberOfOrientations, 'r',
		 "The number of orientations of the Gabor Filters", "A valid integer"},
		{"scales", 's', POPT_ARG_INT, &numberOfScales, 's',
		 "The number of scales of the Gabor Filters", "A valid integer"},
	    {"lower-boundary", 'l', POPT_ARG_INT, &lower, 'l',
		 "Lower Boundary for Binarization", "A valid integer"},
		{"upper-boundary", 'u', POPT_ARG_INT, &upper, 'u',
		 "Upper Boundary for Binarization", "A valid integer"},
		{"training-directory", 't', POPT_ARG_STRING, trainingDirectory, 't',
		 "The directory containing the training set to be used during nn training.", "A valid directory"},
		{"use-ga", 'g', POPT_ARG_NONE, NULL, 'g',
		 "Set the flag to use genetic algorithms.", "No arguments."},
		{"population-size", 'p', POPT_ARG_INT, &populationSize, 'p',
		 "The size of the population.", "A valid integer"},
		 POPT_TABLEEND 
	};
	
	poptContext ctx = poptGetContext(NULL, argc, argv, table, 0);
	
	int rc = 0;
	while ( ( rc = poptGetNextOpt( ctx ) ) >= 0 ) {
		switch (rc) {
			case 't': 
			  strcpy( trainingDirectory, poptGetOptArg(ctx));
			  cout << "Set Training Directory to " << trainingDirectory << endl;
			  break;
			case 'd': 
			  strcpy( imagePath, poptGetOptArg(ctx));
			  cout << "Image to perform texture analysis " << imagePath << endl;
			  detectFlag = true;
			  break;
			  break;
			case 'o':
			  strcpy( outputPath, poptGetOptArg(ctx));
			  cout << "Output image " << outputPath << endl;
			  detectFlag = true;
			  break;
			case 'g':
			  gaFlag = true;
			  cout << "Will use genetic algorithm" << endl;
			  break;
			default:
			  cout << "Set " << (char)rc << " parameter to " << poptGetOptArg(ctx) << endl;
		}
	}
	
	poptFreeContext( ctx );
	
	return 0;
}

int detectIt() {
	Image *img = new Image( imagePath, 256 ); 
	img->toGray(); 
	GaborFilter *gf = new GaborFilter( numberOfOrientations, numberOfScales );
	Image* res = gf->apply(img);
	res->binarize(lower, upper);
	res->save( outputPath );
	delete gf;
	delete img;
	delete res;
	return 0;
}

int trainIt() {
	double bestFMeasure = -1;
	int bestO = -1;
	int bestS = -1;
	int bestL = -1;
	int bestU = -1;
	
	for (int o = 1; o <= 10; o++ ) {
		for (int s = 1; s <= 10; s++ ) {
			for (int l = 0; l < 256; l++ ) {
				for ( int u = l+1; u < 256; u++ ){
					double fmeasure;
					testRel( trainingDirectory, o, s, l, u, fmeasure );
					cout << "o\t" << o << "\ts\t" << s << "\tl\t" << l << "\tu\t" << u << "\tf-measure\t" << fmeasure << endl;
					if ( bestFMeasure > fmeasure ) {
						bestFMeasure = fmeasure;
						bestO = o;
						bestS = s;
						bestL = l;
						bestU = u;
					}
				}
			}
		}
	}
	cout << "Best f-measure found: " << bestFMeasure << "  -  Parameters are: o=" << bestO << " s=" << bestS << " l=" << bestL << " u=" << bestU << endl;
	return 0;
}

// Retorna o fmeasure
float fitness(GAGenome& g) {	
  GABin2DecGenome & genome = (GABin2DecGenome &)g;
  
  double fmeasure;  
  testRel( trainingDirectory,  genome.phenotype(0),  genome.phenotype(1),  genome.phenotype(2),  genome.phenotype(3), fmeasure );
  if ( fmeasure == 0 ) {
  	fmeasure = 0.000001; 
  }  
  return fmeasure;
}

double desiredRatio = 0.9;
GABoolean GATerminateUponScoreConvergence(GAGeneticAlgorithm & ga){
	double ratio = ga.statistics().current(GAStatistics::Mean) /
     ga.statistics().current(GAStatistics::Maximum);
     cout << "Razao Media/Melhor: " << ratio << endl;
  if( ratio >= desiredRatio)
    return gaTrue;
  else
    return gaFalse;
}


int gaIt() {
	cout << "Training using Genetic Algorithm (GAlib)" << endl;
 	
	//Variáveis do GA
	unsigned int seed = 0;
  	int popsize  	  = populationSize;
  	int ngen       	  = 5*populationSize;
  	float pmut   	  = 0.01;
  	float pcross 	  = 0.9;

	//Template do gene
  	GARandomSeed(seed);
  	int min[] = {1, 					 1,   			   0,     0};
 	int max[] = {numberOfOrientations, numberOfScales, lower, upper};
  	
    //Cria o fenótipo
  	GABin2DecPhenotype map;
  	map.add(4, min[0], max[0]);
  	map.add(4, min[1], max[1]);
  	map.add(8, min[2], max[2]);
  	map.add(8, min[3], max[3]);  

	// Cria o genoma
  	GABin2DecGenome genome(map, fitness, NULL);

	GASteadyStateGA ga(genome);
  	ga.populationSize(popsize);
  	ga.pReplacement(0.50);
	ga.nGenerations(ngen);
  	ga.pMutation(pmut);
  	ga.pCrossover(pcross);
  	ga.selectScores(GAStatistics::AllScores);
  	ga.scoreFrequency(1);
  	ga.flushFrequency(1);  
    ga.scoreFilename("gascores.dat");
  	
  	cout << "-----*****-----\nGerando População Inicial " << endl;
  	ga.initialize(seed);
  	ga.terminator(GATerminateUponScoreConvergence);
  	
  	while (!ga.done()) {
  		cout << "\n-----*****-----\nIniciando Geração " << ga.generation() << endl;
  		ga.step();
  		cout << "Melhor Indivíduo: " << endl;
  		genome = ga.statistics().bestIndividual();
  		for(unsigned int i=0; i < map.nPhenotypes(); i++){
		   cout.width(10); cout << (int)genome.phenotype(i) << " ";
		}
		cout << "\n\n"; cout.flush();
  		//ga.flushScores();
  	}

	// Exibe o melhor indivíduo
  	genome = ga.statistics().bestIndividual();
  	cout << "Resultado do algoritmo: " << endl;
  	genome = ga.statistics().bestIndividual();
  		for(unsigned int i=0; i < map.nPhenotypes(); i++){
		   cout.width(10); cout << (int)genome.phenotype(i) << " ";
		}
		cout << "\n\n"; cout.flush();

//  cout << "best of generation data are in 'bog.dat'\n";
  return 0;
}

void checkScales() {
	DirectoryIterator *di = new DirectoryIterator(trainingDirectory);
	di->addSupportedImgExtension(".jpg");
	di->addSupportedMskExtension(".pgm");
	MinimumDimensionExtractor *mde = new MinimumDimensionExtractor();
	di->iterate(mde);
	int minimum = mde->getResult();
	
	int scale = numberOfScales - 1;
	GaborFilter * gf = new GaborFilter(1,1);
	Image * kernel = gf->getKernel(0,scale,CV_GABOR_REAL);
	int scaleLength = kernel->getWidth();
	delete kernel;
	
	while ( scaleLength >= minimum ) {
		scale--;
		kernel = gf->getKernel(0,scale,CV_GABOR_REAL);
		scaleLength = kernel->getWidth();
		delete kernel;
	}
	
	if ( scale != numberOfScales - 1 ) {
		numberOfScales = scale + 1;
		cout << "Corrigindo escalas para " << numberOfScales << endl;
	}
	
	delete gf;
	delete di;
	delete mde;
}

int main( int argc, const char* argv[] ) {
	try {
		//Option management
		processOptions(argc, argv);
		
		if ( detectFlag ) {
			return detectIt();
		} else {
			checkScales();
			if (gaFlag) {
				return gaIt();
			} else {
				return trainIt();
			}
		}
	} catch (char * c) {
		cout << c << endl;
	}
}
