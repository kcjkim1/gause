#include <popt.h>
#include <iostream>
#include <cstring>

#include "ExtendedFill.h"
#include "img.h"

using namespace std;

#define MAX_DIRECTORY_SIZE 256

char srcImageName[MAX_DIRECTORY_SIZE];
char seedImageName[MAX_DIRECTORY_SIZE];
char outputPath[MAX_DIRECTORY_SIZE];
double thArea;
double thBorder;
int colorSpace = 0;
bool verbose=false;

int processOptions(int argc, const char* argv[]) {
	struct poptOption table[] = {
		{"source-image", 'i', POPT_ARG_STRING, srcImageName, 'i',
		 "The image to apply the floodfill", "A valid image file"},
		{"seed-image", 's', POPT_ARG_STRING, seedImageName, 's',
		 "The image with the floodfill seed points", "A valid image file"},
		{"area-threshold", 'a', POPT_ARG_DOUBLE, &thArea, 'a',
		 "The threshold for the floodfill area", "A valid number"},
		{"border-threshold", 'b', POPT_ARG_DOUBLE, &thBorder, 'b',
		 "The threshold for the floodfill border", "A valid number"},
		{"colorspace", 'c', POPT_ARG_INT, &colorSpace, 'c',
		 "The colorspace (0-RGB, 1-HSV, 2-YCBCR, 3-XYZ, 4-HLS, 5-LAB, 6-LUV)", "A valid integer (1 to 7)"},
		{"output-path", 'o', POPT_ARG_STRING, outputPath, 'o',
		 "The path to the output image", "A valid path"},
		{"verbose", 'v', POPT_ARG_NONE, NULL, 'v',
		 "Outputs everything", "No argument"},
		POPT_TABLEEND 
	};
	
	poptContext ctx = poptGetContext(NULL, argc, argv, table, 0);
	
	int rc = 0;
	while ( ( rc = poptGetNextOpt( ctx ) ) >= 0 ) {
		switch (rc) {
			case 'i': 
			  strcpy( srcImageName, poptGetOptArg(ctx));
			  cout << "Set Source Image to " << srcImageName << endl;
			  break;
			case 's': 
			  strcpy( seedImageName, poptGetOptArg(ctx));
			  cout << "Set Seed Image to " << seedImageName << endl;
			  break;
			case 'o': 
			  strcpy( outputPath, poptGetOptArg(ctx));
			  cout << "Output image " << outputPath << endl;
			  break;
			case 'v': 
			  verbose = true;
			  break;
			default:
			  cout << "Set " << (char)rc << " parameter to " << poptGetOptArg(ctx) << endl;
		}
	}
	
	return 0;
}

int main(int argc, const char* argv[]) {
	try {
		processOptions(argc, argv);
	
		Image * srcImage = new Image( srcImageName );
		if ( colorSpace != 0 ) {
			srcImage->convert(colorSpace);
		}
		
		Image * seedImage = new Image( seedImageName );
		if (seedImage->getColorSpace() != GL ) {
			seedImage->toGray();
		}
		
		ExtendedFill * fill = new ExtendedFill(srcImage, seedImage, thArea, thBorder);
		fill->setVerbose(true);		
		Image * resultImage = fill->run();
		resultImage->save(outputPath);
	
		delete fill;
		delete srcImage;
		delete seedImage;
		delete resultImage;
	} catch (const char * c) {
		cout << c << endl;
	}
}