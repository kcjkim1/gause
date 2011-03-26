#include "ExtendedFill.h"
#include <iostream>

ExtendedFill::ExtendedFill() {
}

ExtendedFill::ExtendedFill( Image* source, Image* seeds, double areaThreshold, double borderThreshold ) {
	this->source = source;
	this->seeds = seeds;
	this->areaThreshold = areaThreshold;
	this->borderThreshold = borderThreshold;
}

ExtendedFill::~ExtendedFill() {
}

Image * ExtendedFill::run() {
	Image * control = new Image( source->getWidth(), source->getHeight(), 1 );
	control->clear();
	
	//cvWaitKey(0);
	long mCh1 = 0;
	long mCh2 = 0;
	long mCh3 = 0;
	long area = 0;
	if(verbose){
		std::cout << "Initializing means to zero" << std::endl;
	}
	for ( int i = 0; i < seeds->getHeight(); i++ ) {
		for ( int j = 0; j < seeds->getWidth(); j++ ) {
			if ( seeds->getGray( i, j ) == 255 ) {
				if(verbose){
					std::cout << "Starting with seed (" << i << ", " << j << ")" << std::endl;
				}
				this->floodIt( source, i, j, control, mCh1, mCh2, mCh3, area );
				mCh1 = 0;
				mCh2 = 0;
				mCh3 = 0;
				area = 0;
				if(verbose){
					std::cout << "Reinitializing means to zero" << std::endl;
				}
			}
		}
	}	
	return control;
}

void ExtendedFill::floodIt(Image * img, int x, int y, Image * control, long & mCh1, long & mCh2, long & mCh3, long & area ) {
	if ( control->getGray(x,y) == 0 ) {
		//Verifica se satisfaz o limiar
		int ch1 = img->getChannel(1, x, y);
		int ch2 = img->getChannel(2, x, y);
		int ch3 = img->getChannel(3, x, y);		
		if ( area == 0 || (checkThreshold(ch1, ch2, ch3, mCh1, mCh2, mCh3, areaThreshold)) ) {
			if(verbose){
				std::cout << "Position (" << x << ", " << y << ") = (" << ch1 << ", " << ch2 << ", " << ch3 << ") passed on the area threshold" << std::endl;
			}			
			control->setGray( x, y, 255 );
			updateMean(ch1, ch2, ch3, mCh1, mCh2, mCh3, area);
			for ( int i = x - 1; i <= x + 1; i++ ) {
				for ( int j = y - 1; j <= y + 1; j++ ) {
					if ( i >= 0 && i < img->getHeight() && j >= 0 && j < img->getWidth() ) {
						int vCh1 = img->getChannel(1, i, j);
						int vCh2 = img->getChannel(2, i, j);
						int vCh3 = img->getChannel(3, i, j);
						if ( checkThreshold(ch1, ch2, ch3, vCh1, vCh2, vCh3, borderThreshold) ) {
							if(verbose){
								std::cout << "Position (" << i << ", " << j << ") = (" << vCh1 << ", " << vCh2 << ", " << vCh3 << ") passed on the border threshold. Point is (" << ch1 << ", " << ch2 << ", " << ch3 << ")" << std::endl;
							}								
							floodIt(img, i, j, control, mCh1, mCh2, mCh3, area);
						} else if(verbose){
							std::cout << "Position (" << i << ", " << j << ") = (" << vCh1 << ", " << vCh2 << ", " << vCh3 << ") FAILED on the border threshold. Point is (" << ch1 << ", " << ch2 << ", " << ch3 << ")" << std::endl;
						}
					}
				}
			}			
		} else if(verbose){
			std::cout << "Position (" << x << ", " << y << ") = (" << ch1 << ", " << ch2 << ", " << ch3 << ") FAILED on the area threshold." << std::endl;
		}		
	} else if(verbose){
			std::cout << "Position (" << x << ", " << y << ") already marked, skipping" << std::endl;
		}
}

bool ExtendedFill::checkThreshold(int ch1, int ch2, int ch3, long mCh1, long mCh2, long mCh3, int threshold ) {
	int sum = (ch1 - mCh1) * (ch1 - mCh1) + (ch2 - mCh2) * (ch2 - mCh2) + (ch3 - mCh3) * (ch3 - mCh3);
	int distance = sqrt(sum);
	return distance < threshold; 
}

void ExtendedFill::updateMean(int ch1, int ch2, int ch3, long & mCh1, long & mCh2, long & mCh3, long & area ) {
	mCh1 = (mCh1 * area + ch1)/(area+1);
	mCh2 = (mCh2 * area + ch2)/(area+1);
	mCh3 = (mCh3 * area + ch3)/(area+1);
	area = area + 1;
	if(verbose){
		std::cout << "Mean updated to (" << mCh1 << ", " << mCh2 << ", " << mCh3 << ")" << std::endl;
	}
}

void ExtendedFill::setVerbose(bool verbose) {
	this->verbose = verbose;
}
