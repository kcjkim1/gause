#include "GaborFilter.h"

#include <math.h>

GaborFilter::GaborFilter(int orientations, int scales) {
	this->orientations = orientations;
	this->scales = scales;
	//std::cout << "Created Gabor Filter combining " << this->orientations << " orientations and " << this->scales << " scales" << std::endl;
}

GaborFilter::~GaborFilter() {
}

Image* GaborFilter::apply(Image* img) {	
	Image* res = new Image( img->getWidth(), img->getHeight() );
	res->toGray();
	
	double resMtr[img->getHeight()][img->getWidth()];
	for(int i=0;i<img->getHeight();i++) {
		for(int j=0;j<img->getWidth();j++) {
			resMtr[i][j] = 0;
		} 
	}
	
    //img->showAndWait("Visualization");
    
	for ( int scale = 0; scale < this->scales; scale++ ) {
    	for ( int orientationIndex = 0; orientationIndex < orientations; orientationIndex++ ) {			
			double orientation = orientationIndex*PI/orientations;
			//std::cout << scale << "\t" << orientation << std::endl;
			Image* magImg = getGaborMagnitude(img, orientation, scale);
			//magImg->showAndWait("debug");
			for(int i=0;i<img->getHeight();i++) {
				for(int j=0;j<img->getWidth();j++) {
					double value = resMtr[i][j] + magImg->getGray(i,j)*magImg->getGray(i,j);
					resMtr[i][j] = value;
				} 
			}
			
			delete magImg;
    	}
    }
    
    long max = 0;
    for(int i=0;i<img->getHeight();i++) {
		for(int j=0;j<img->getWidth();j++) {
			long value = cvRound( sqrt( resMtr[i][j] ) / (scales * orientations));
			if ( value > max ) {
				max = value;
			}
			res->setGray(i,j,value);
		}
	}
	
	return res;
}

Image* GaborFilter::getKernel( double orientation, int scale, int type ) {
	CvGabor *gabor1 = new CvGabor;
	double Sigma = 2*PI;
	double F = sqrt(2.0);
	gabor1->Init(orientation, scale, Sigma, F);
	IplImage *kernel = cvCreateImage( cvSize(gabor1->get_mask_width(), gabor1->get_mask_width()), IPL_DEPTH_8U, 1);
	kernel = gabor1->get_image(type);
	Image * result = new Image( kernel, 256, GL );
	delete gabor1;
	return result;
}

Image* GaborFilter::getGaborMagnitude(Image* img, double orientation, int scale) {
	Image* magImg = new Image(img->getWidth(),img->getHeight());
	magImg->toGray();
			
    double Sigma = 2*PI;
	double F = sqrt(2.0);
	CvGabor *gabor1 = new CvGabor;
	gabor1->Init(orientation, scale, Sigma, F);
	
	gabor1->conv_img(img->getIplImage(), magImg->getIplImage(), CV_GABOR_MAG);
	delete gabor1;
	return magImg;
}
