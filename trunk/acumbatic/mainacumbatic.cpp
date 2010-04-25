#include <popt.h>
#include <iostream>

#include "acumbaticshared.h"
#include "neural.h"
#include "training.h"
#include "config.h"
#include "dirit.h"

using namespace std;

#define MAX_DIRECTORY_SIZE 256
#define MAX_CS_SIZE 6

char trainingDirectory[MAX_DIRECTORY_SIZE];
char imagePath[MAX_DIRECTORY_SIZE];
char outputPath[MAX_DIRECTORY_SIZE];
char csString[MAX_CS_SIZE];
bool detectFlag = false;
bool delFiles = false;
bool vFlag = false;
bool horizontalFlag = false;
bool verticalFlag = false;
int hidden, epochs, epochsBetweenReports, trainingIterations, levels, windowSize, colorSpace, rndConf, horatio;
double thresholdStep;

int processColorSpace( ) {
	int res = atoi(csString);
	//If zero, must check if the string is valid.
	if ( res == 0 ) {
		if ( csString[0] == '0' ) {
			return 0;
		} else {
			if ( strcmp(csString, "RGB") == 0 ) {
				return RGB;
			} else if ( strcmp(csString, "HSV") == 0 ) {
				return HSV;
			} else if ( strcmp(csString, "YCbCr") == 0 ) {
				return YCBCR;
			} else if ( strcmp(csString, "XYZ") == 0 ) {
				return XYZ;
			} else if ( strcmp(csString, "HLS") == 0 ) {
				return HLS;
			} else if ( strcmp(csString, "Lab") == 0 ) {
				return LAB;
			} else if ( strcmp(csString, "Luv") == 0 ) {
				return LUV;
			}
		}
	} else {
		return res;
	}
	return -1;
}

int processOptions(int argc, const char* argv[]) {
	struct poptOption table[] = {
		{"training-directory", 't', POPT_ARG_STRING, trainingDirectory, 't',
		 "The directory containing the training set to be used during nn training.", "A valid directory"},
		{"hidden-neurons", 'h', POPT_ARG_INT, &hidden, 'h',
		 "Number of hidden neurons.", "A valid integer."},		 
		{"epochs", 'e', POPT_ARG_INT, &epochs, 'e',
		 "Number of training epochs", "A valid integer."},
		{"epochs-between-reports", 'r', POPT_ARG_INT, &epochsBetweenReports, 'r',
		 "Frequency of reports", "A valid integer."},
		{"training-iterations", 'i', POPT_ARG_INT, &trainingIterations, 'i',
		 "The number of times each configuration will be trained.", "A valid integer"},
		{"number-levels", 'q', POPT_ARG_INT, &levels, 'q',
		 "The number of quantization levels", "A valid integer (power of two)"},
		{"colorspace", 'c', POPT_ARG_STRING, csString, 'c',
		 "The colorspace (0-RGB, 1-HSV, 2-YCBCR, 3-XYZ, 4-HLS, 5-LAB, 6-LUV)", "A valid integer (0 to 6), or the name of the colorspace"},
		{"window-size", 'w', POPT_ARG_INT, &windowSize, 'w',
		 "The maximum window size", "A valid integer (power of two)"},
		{"threshold-step", 's', POPT_ARG_DOUBLE, &thresholdStep, 's',
		 "The threshold step", "A valid number"},
		{"image-path", 'd', POPT_ARG_STRING, imagePath, 'd',
		 "The path to an image to detect", "A valid path"},
		{"output-path", 'o', POPT_ARG_STRING, outputPath, 'o',
		 "The path to the output image", "A valid path"},
		{"visualize", 'v', POPT_ARG_NONE, NULL, 'v',
		 "Visualizes the result", ""},
		{"useshorizontal", 'z', POPT_ARG_NONE, NULL, 'z',
		 "Uses horizontal position", ""},
		{"usesvertical", 'a', POPT_ARG_NONE, NULL, 'a',
		 "Uses vertical position", ""},
		{"random", 'n', POPT_ARG_INT, &rndConf, 'n',
		 "Random number of images", ""},
		{"holdoutratio", 'k', POPT_ARG_INT, &horatio, 'k',
		 "The k parameter for the holdout ratio method", ""},
		{"delFiles", 'f', POPT_ARG_NONE, NULL, 'f',
		 "Deletes training files after execution.", ""},
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
			  cout << "Image to perform detection " << imagePath << endl;
			  detectFlag = true;
			  break;
			case 'o': 
			  strcpy( outputPath, poptGetOptArg(ctx));
			  cout << "Output image " << outputPath << endl;
			  detectFlag = true;
			  break;
			case 'c': 
			  strcpy( csString, poptGetOptArg(ctx));
			  colorSpace = processColorSpace();
			  cout << "Color Space is now " << getColorSpaceString(colorSpace) << endl;
			  break;
			case 'v': 
			  vFlag = true;
			  detectFlag = true;
			  break;
			case 'z': 
			  horizontalFlag = true;
			  cout << "Will use horizontal position" << endl;
			  break;
			case 'a':
			  cout << "Will use vertical position" << endl;
			  verticalFlag = true;
			  break;
			case 'f':
			  cout << "Will delete training files after execution" << endl;
			  delFiles = true;
			  break;
			default:
			  cout << "Set " << (char)rc << " parameter to " << poptGetOptArg(ctx) << endl;
		}
	}
	
	return 0;
}

int main(int argc, const char* argv[]) {
	try {
		//Option management
		rndConf = 0;
		horatio = 10;
		processOptions(argc, argv);
		
		if ( detectFlag ) {
			Configuration *cf = new Configuration(CONFIG_FILE_NAME);		
			NeuralNetwork *nn = new NeuralNetwork(NETWORK_FILE_NAME, THRESHOLD_FILE_NAME, cf);		
			Image* i = new Image( imagePath, cf->getNumberOfLevels() );
			Image* r = nn->apply(i);
			if ( vFlag ) {
				string res = "Resultado";
				r->showAndWait(res.data());
			}
			r->save(outputPath);
			delete r;
			delete i;
			delete nn;
			delete cf;
		} else {
			TrainingSet   *ts = NULL;
			Configuration *extant = NULL;			
			Configuration *cf = new Configuration( colorSpace, levels, windowSize, horizontalFlag, verticalFlag );
			#ifdef MAIN_DEBUG
				cout << "Configuration Created" << endl; 
			#endif
			if ( fileExists(TRAIN_FILE) && fileExists(VALIDATION_FILE) && fileExists(CONFIG_FILE_NAME) && 
			     cf->isCompatible(extant = new Configuration(CONFIG_FILE_NAME) ) ) {
                   cout << "Found a compatible training set. Will use it!" << endl;
				   ts = new TrainingSet( extant, horatio, delFiles );
				
			} else {
				cout << "Creating Training Set for specified configuration" << endl;
				ts = new TrainingSet( trainingDirectory, cf, rndConf, horatio, delFiles );								
				#ifdef MAIN_DEBUG
					cout << "Training Set Created" << endl;
				#endif
			}
			
			NeuralNetwork *nn;
			nn = new NeuralNetwork( hidden, cf );
			#ifdef MAIN_DEBUG
				cout << "NN Created" << endl;
			#endif
					
			nn->train( ts, trainingIterations, epochs, epochsBetweenReports, thresholdStep );
			nn->save();	
			cf->save();	
				
			delete ts;
			delete cf;
			delete nn;
			if ( extant != NULL ) {
				delete extant;
			}
		}
	} catch (const char* c) {
		cout << c << endl;
	}
}
