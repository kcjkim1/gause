#include "MinimumDimensionExtractor.h"

#include "img.h"

MinimumDimensionExtractor::MinimumDimensionExtractor() {
	this->result = 999999999;
}

MinimumDimensionExtractor::~MinimumDimensionExtractor()
{
}

void MinimumDimensionExtractor::handle( const char* fileName, const char* maskName){
	Image * img = new Image( fileName, 256 );
	if ( img->getHeight() < this->result ) {
		this->result = img->getHeight();
	}
	if ( img->getWidth() < this->result ) {
		this->result = img->getWidth();
	}
	delete img;
}

int MinimumDimensionExtractor::getResult( ){
	return result;
}