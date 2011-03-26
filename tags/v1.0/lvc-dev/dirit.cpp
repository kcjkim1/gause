#include "dirit.h"
#include "cvutils.h"

#include <dirent.h>
#include <iostream>
#include <string>

//#define DEBUG

using namespace std;

DirectoryIterator::DirectoryIterator(char* directory) {
	imgCount = 0;
	mskCount = 0;
	setDirectory( directory );
	rndSet = 0;
	srand ( time(NULL) );	
}

void DirectoryIterator::setRandom( int rndSet ) {
	this->rndSet = rndSet;
}

void DirectoryIterator::setDirectory( char* directory ) {
	this->directory = directory;
}

void DirectoryIterator::iterate( CallbackHandler *cb ) {
	rndCount = 0;
	#ifdef DEBUG
    	cout << "Begin iteration" << endl;
    #endif
	//Opens the directory 
	struct dirent   *dit;
    //Iterates all the way over the files
    do {
		DIR* dir = opendir( directory );
		#ifdef DEBUG
           cout << "Directory open" << endl;
        #endif
        if ( !dir ) {
        	throw "Invalid Directory";
        }
		while ((dit = readdir(dir)) != NULL) {
		 	#ifdef DEBUG
		   		cout << "File found" << endl;
		   	#endif    	
		  	//If it is a valid input file
		   	string fileName(dit->d_name);
		   	fileName = directory + fileName;
		   	#ifdef DEBUG
		   		cout << "File is " << fileName << endl;
		   		cout << "Will look for " << imgCount << " extensions" << endl;
		   	#endif
		   	bool isSupported = false;
		   	const char *ext = NULL;
		   	for ( int i = 0; i < imgCount && !isSupported; i++ ) {
		   		#ifdef DEBUG
		     		cout << "Looking for extension " << supportedImgExtensions[i] << endl;
			    #endif 
				isSupported = (fileName.find(supportedImgExtensions[i]) == ( fileName.size() - strlen(supportedImgExtensions[i]) ));
				ext = supportedImgExtensions[i];
		    }
		    
		    if ( isSupported ) {
		    	#ifdef DEBUG
		    		cout << "File Suported" << endl;
		    	#endif
		    	//Verifies if it is going to be processed
		    	if ( rndSet != 0 && rndCount < rndSet ) {
		    		double rnd = (double)rand() / RAND_MAX;
		    		if ( rnd < 0.5 ) {
		    			continue;
		    		}
		    	}
		    	//Finds the correspondent mask
		    	string pureName = fileName.substr( 0, fileName.size() - strlen(ext));
		    	string maskName;
		    	bool hasMask = false;
		    	for ( int i = 0; i < mskCount && !hasMask; i++ ) { 
		    		 maskName = pureName + supportedMskExtensions[i];
		    		 hasMask = fileExists( maskName );
		    	}
		    	
		    	if ( hasMask ) {
		    		#ifdef DEBUG
		    		  cout << "Valid File: " << fileName << endl;
		    		#endif
		    		#ifdef DIRIT_DEBUG
						cout << "Before handling" << fileName << endl;
					#endif	
		    		cb->handle(fileName.data(), maskName.data());
		    		if ( rndSet != 0) {
		    			rndCount++;
		    		}
		    		#ifdef DIRIT_DEBUG
						cout << "After handling" << fileName << endl;
					#endif	
		   		} else {
		   			cout << "No mask found for file " << fileName << endl;
		   		}    		
		   	} else {
		   		#ifdef DEBUG
		   			cout << "Invalid File: " << fileName << endl;
		   		#endif
		   	}
		}
		closedir( dir );
    } while ( rndCount < rndSet );
}

//void DirectoryIterator::init() {
//	//Initializes Global Structures
//	supportedImgExtensions[0] = ".jpg";
//	supportedImgExtensions[1] = ".JPG";
//	supportedImgExtensions[2] = ".jpeg";
//	supportedImgExtensions[3] = ".JPEG";
//	supportedImgExtensions[4] = ".bmp";
//	supportedImgExtensions[5] = ".BMP";
//	
//	supportedMskExtensions[0] = ".pgm";
//	supportedMskExtensions[1] = ".PGM";
//}

void DirectoryIterator::addSupportedImgExtension( const char* ext ) {
	supportedImgExtensions[imgCount++] = ext;
}

void DirectoryIterator::addSupportedMskExtension( const char* ext ) {
	supportedMskExtensions[mskCount++] = ext;
}
