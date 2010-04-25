#include "config.h"
#include "acumbaticshared.h"
#include "cvutils.h"

Configuration::Configuration( int colorSpace, int numberOfLevels, int windowSize, bool horizontalFlag, bool verticalFlag ) {
	setColorSpace(colorSpace);
	setNumberOfLevels(numberOfLevels);
	setWindowSize(windowSize);
	this->horizontalFlag = horizontalFlag;
	this->verticalFlag = verticalFlag;
}

Configuration::Configuration( string fileName ){
	int readResult = 0;
	FILE* file = fopen( fileName.data(), "r" );
	int colorSpace;
	readResult += fscanf( file, "%d", &colorSpace );
	setColorSpace(colorSpace);
	int numberOfLevels;
	readResult += fscanf( file, "%d", &numberOfLevels );
	setNumberOfLevels(numberOfLevels);
	int windowSize;
	readResult += fscanf( file, "%d", &windowSize );
	setWindowSize(windowSize);
	int flag;
	readResult += fscanf( file, "%d", &flag );
	horizontalFlag = (flag==1);
	readResult += fscanf( file, "%d", &flag );
    verticalFlag = (flag==1);	
	fclose( file );
	if ( readResult != 5 ) {
		throw "Invalid configuration file!";
	}
}

int Configuration::getWindowSize() {
	return windowSize;
}

int Configuration::getColorSpace() {
	return this->colorSpace;
}

int Configuration::getNumberOfLevels() {
	return this->numberOfLevels;
}

void Configuration::setColorSpace( int colorSpace ) {
	this->colorSpace = colorSpace;
}

void Configuration::setNumberOfLevels( int numberOfLevels ) {
	this->numberOfLevels = numberOfLevels;
}

void Configuration::setWindowSize( int windowSize ) {
	this->windowSize = windowSize;
}

void Configuration::save() {
	string fileName = CONFIG_FILE_NAME;
	FILE* cfg = fopen( CONFIG_FILE_NAME, "w" );
	fprintf(cfg, "%d\n%d\n%d\n%d\n%d\n", colorSpace, numberOfLevels, windowSize, (horizontalFlag?1:0), (verticalFlag?1:0) );
	fclose( cfg );
}

string Configuration::getPositionDescription() {
	if ( horizontalFlag && verticalFlag ) {
		return "hor_vert";		
	}
	
	if ( horizontalFlag ) {
		return "hor";		
	}
	
	if ( verticalFlag ) {
		return "vert";		
	}
	
	return "no_pos";
}

int Configuration::getDimension() {
	if ( horizontalFlag && verticalFlag ) {
		return 5;		
	}
	
	if ( horizontalFlag || verticalFlag ) {
		return 4;		
	}
	
	return 3;
}

bool Configuration::usesHorizontal() {
	return horizontalFlag;
}

bool Configuration::usesVertical() {
	return verticalFlag;
}

bool Configuration::isCompatible(Configuration * other) {
	return ( this->colorSpace )     == ( other->colorSpace )     &&
	       ( this->numberOfLevels ) == ( other->numberOfLevels ) &&
	       ( this->windowSize )     == ( other->windowSize )     &&
	       ( this->horizontalFlag ) == ( other->horizontalFlag ) &&
	       ( this->verticalFlag )   == ( other->verticalFlag );
}