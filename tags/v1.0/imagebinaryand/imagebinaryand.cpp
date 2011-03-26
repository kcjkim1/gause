#include <cvutils.h>
#include <img.h>
#include <popt.h>

using namespace std;

#define MAX_DIRECTORY_SIZE 256

char image1Path[MAX_DIRECTORY_SIZE];
char image2Path[MAX_DIRECTORY_SIZE];
char outputPath[MAX_DIRECTORY_SIZE];

int processOptions(int argc, const char* argv[]) {
	struct poptOption table[] = {
		{"image1-path", 'i', POPT_ARG_STRING, image1Path, 'i',
		 "The path to an image to operate", "A valid path"},
		{"image1-path", 'j', POPT_ARG_STRING, image2Path, 'j',
		 "The path to an image to operate", "A valid path"},
		{"output-path", 'o', POPT_ARG_STRING, outputPath, 'o',
		 "The path to the output image", "A valid path"},
		 POPT_TABLEEND 
	};
	
	poptContext ctx = poptGetContext(NULL, argc, argv, table, 0);
	
	int rc = 0;
	while ( ( rc = poptGetNextOpt( ctx ) ) >= 0 ) {
		switch (rc) {
			case 'i': 
			  strcpy( image1Path, poptGetOptArg(ctx));
			  cout << "Image 1:  " << image1Path << endl;
			  break;
			case 'j': 
			  strcpy( image2Path, poptGetOptArg(ctx));
			  cout << "Image 2:  " << image2Path << endl;
			  break;
			case 'o': 
			  strcpy( outputPath, poptGetOptArg(ctx));
			  cout << "Output image " << outputPath << endl;
			  break;
			default:
			  cout << "Set " << (char)rc << " parameter to " << poptGetOptArg(ctx) << endl;
		}
	}
	
	return 0;
}

int main( int argc, const char* argv[] ) {
	
	//Option management
	processOptions(argc, argv);
	
	Image *img1 = new Image( image1Path, 256 ); 
	img1->toGray();
	
	Image *img2 = new Image( image2Path, 256 ); 
	img2->toGray();

	Image* res = binaryAnd( img1, img2 );
	
	res->save( outputPath );
	
	delete img1;
	delete img2;
	delete res;
}
