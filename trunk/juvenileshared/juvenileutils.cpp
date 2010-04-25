#include "juvenileutils.h"
#include "dirit.h"
#include "metrics.h"

#include <iostream>

void setInFile( std::string resultFileName, double fmeasure ) {
	FILE* resultFile = fopen( resultFileName.data(), "w" );
	fprintf( resultFile, "%f", fmeasure );
	fclose(resultFile);
}

double getFromFile( std::string resultFileName ) {
	FILE* resultFile = fopen( resultFileName.data(), "r" );
	float fmeasure;
	fscanf(resultFile, "%f", &fmeasure);
	fclose(resultFile);
	return fmeasure;
}

void testRel( char* trainingDirectory, int o, int s, int l, int u, double &fmeasure ) {	
	if ( u <= l ) {		
		fmeasure = 0;
		//std::cout << "Result: " << fmeasure << std::endl;
		return;
	}	
	
	std::string resultFileName = getResultFileName(o, s, l, u);
	if ( !fileExists(resultFileName) ) {
		std::cout << "Testing combination: " << o << "\t" << s << "\t" << l << "\t" << u << std::endl;	
		DirectoryIterator *di = new DirectoryIterator(trainingDirectory);
		di->addSupportedImgExtension(".jpg");
		di->addSupportedMskExtension(".pgm");
		MetricsExtractor *me = new MetricsExtractor( o, s, l, u );
		
		di->iterate(me);
		me->close(fmeasure);	
		
		delete di;
		delete me;
		setInFile( resultFileName, fmeasure );
		std::cout << "Found: " << fmeasure << std::endl;
	} else {
		fmeasure = getFromFile(resultFileName);
	}
}

std::string getResultFileName( int o, int s, int l, int u ) {
	std::string result = TXT_FILE_PREFIX;
	char buff[10];
	sprintf(buff,"%d",o);
	result = result + TXT_FILE_SEP + buff;
	
	sprintf(buff,"%d",s);
	result = result + TXT_FILE_SEP + buff;

	sprintf(buff,"%d",l);
	result = result + TXT_FILE_SEP + buff;
	
	sprintf(buff,"%d",u);
	result = result + TXT_FILE_SEP + buff;
	
	result = result + TXT_FILE_EXT;
	
	return result;
}

std::string getGaborResultFileName( std::string pureName, int o, int s) {
	std::string result = pureName + TXT_FILE_SEP + TXT_FILE_PREFIX;
	char buff[10];
	sprintf(buff,"%d",o);
	result = result + TXT_FILE_SEP + buff;
	
	sprintf(buff,"%d",s);
	result = result + TXT_FILE_SEP + buff;
	
	result = result + TXT_IMG_EXT;
	
	return result;
}