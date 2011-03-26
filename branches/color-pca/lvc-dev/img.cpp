#include "img.h"

Image::Image( const char* fileName, int numberOfLevels ) {
	this->img = cvLoadImage( fileName, 1 );
	
	setNumberOfLevels(numberOfLevels);
	if ( this->img->nChannels == 1 ) {
		setColorSpace( GL );
	} else {
		setColorSpace(RGB);
	}
}

Image::Image( const char* fileName ) {
	this->img = cvLoadImage( fileName, 1 );
	
	setNumberOfLevels(256);
	if ( this->img->nChannels == 1 ) {
		setColorSpace( GL );
	} else {
		setColorSpace(RGB);
	}
}  

Image::Image( int width, int height ) {
	this->img = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 3 ); 
	
	setNumberOfLevels(256);
	setColorSpace(RGB);
}

Image::Image( int width, int height, int nChannels ) {
	this->img = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, nChannels ); 
	
	setNumberOfLevels(256);
	if ( nChannels == 3 ) {
		setColorSpace(RGB);
	} else {
		setColorSpace(GL);
	}
}

Image::Image( IplImage * sourceImage, int numberOfLevels, int colorSpace ) {
	this->img = sourceImage; 
	
	setNumberOfLevels(numberOfLevels);
	setColorSpace(colorSpace);
}
 
Image::~Image() {
	cvReleaseImage( &(this->img) );
}

void Image::smooth( int windowSize ) {
	IplImage* smt = cvCreateImage(cvSize(this->img->width, this->img->height), IPL_DEPTH_8U, 3 );
	cvSmooth( img, smt, CV_GAUSSIAN, windowSize, windowSize );
	cvReleaseImage(&img);
	this->img = smt;	
}

void Image::blur( int windowSize ) {
	IplImage* blr = cvCreateImage(cvSize(this->img->width, this->img->height), IPL_DEPTH_8U, 3 );
	cvSmooth( img, blr, CV_BLUR, windowSize, windowSize, 0, 0);
	cvReleaseImage(&img);
	this->img = blr;	
}
  
void Image::convert( int newColorSpace ) {
	if ( this->getColorSpace() != newColorSpace ) {
		int convertion = getConvertion(newColorSpace);
		int noLevels = newColorSpace==GL?1:3;
		IplImage* converted = cvCreateImage(cvSize(this->getWidth(),this->getHeight()), IPL_DEPTH_8U, noLevels );
		cvCvtColor( img, converted, convertion );
		cvReleaseImage(&img);
		this->img = converted;
		this->setColorSpace(newColorSpace);
	} 
}

int Image::getHeight() {
	return this->img->height;
}

int Image::getWidth() {
	return this->img->width;
}
 
void Image::setNumberOfLevels(int numberOfLevels) {
	this->numberOfLevels = numberOfLevels;
}

void Image::setColorSpace( int colorSpace ) {
	this->colorSpace = colorSpace;
}

int Image::getColorSpace() {
	return this->colorSpace;
}

int Image::getGray( int i, int j ) {
	if ( this->getColorSpace() == GL ) {
		return getGrayLevel( img, i, j );
	} else {
		throw "Image is not in Gray Level.";
	}
}

void Image::resize( int height, int width ) {
	IplImage* resized = cvCreateImage(cvSize(width,height), IPL_DEPTH_8U, this->img->nChannels );
	cvResize( img, resized );
	cvReleaseImage(&img);
	this->img = resized;
}

void Image::setGray( int i, int j, int value ) {
	if ( this->getColorSpace() == GL ) {
		setGrayLevel( img, i, j, value );	
	} else if ( this->getColorSpace() == RGB ) {
		setRed( img, i, j, value );
		setGreen( img, i, j, value );
		setBlue( img, i, j, value );
	} else {
		throw "Setting gray value is valid only for RGB and GrayLevel images.";
	}
}

void Image::setPixel( int i, int j, int r, int g, int b ) {
	setRed( img, i, j, r );
	setGreen( img, i, j, g );
	setBlue( img, i, j, b );
}

double Image::getChannel(int channel, int i, int j) {	
	if ( channel == 1 ) {
		switch (this->getColorSpace() ) {
			case RGB:   return this->getRedChannel(i, j);
			case HSV:   return this->getHueChannel(i, j);
			case YCBCR: return this->getLumaChannel(i, j);
			case XYZ:   return this->getXChannel(i, j);
			case HLS:   return this->getHueChannel(i, j);
			case LAB:   return this->getLightness_CIEChannel(i, j);
			case LUV:   return this->getLightness_CIEChannel(i, j);
		}
	} else if ( channel == 2 ) {
		switch (this->getColorSpace() ) {
			case RGB:   return this->getGreenChannel(i, j);
			case HSV:   return this->getSaturationChannel(i, j);
			case YCBCR: return this->getChromaBlueChannel(i, j);
			case XYZ:   return this->getYChannel(i, j);
			case HLS:   return this->getLightnessChannel(i, j);
			case LAB:   return this->getAChannel( i, j );
			case LUV:   return this->getUChannel( i, j );
		}
	} else if ( channel == 3 ) {
		switch (this->getColorSpace() ) {
			case RGB:   return this->getBlueChannel(i, j);
			case HSV:   return this->getValueChannel(i, j);
			case YCBCR: return this->getChromaRedChannel(i, j);
			case XYZ:   return this->getZChannel(i, j);
			case HLS:   return this->getSaturation_LSChannel(i, j);
			case LAB:   return this->getBChannel( i, j );
			case LUV:   return this->getVChannel( i, j );
		}
	} else {
		throw "Cannot return channel value";
	}
	return -1;
}

double Image::getNomalizedChannel(int channel, int i, int j) {
	double beforeReduction = 0;
	if ( channel == 1 ) {
		switch (this->getColorSpace() ) {
			case RGB:   beforeReduction = this->getRedChannel(i, j) / 255.0; break;
			case HSV:   beforeReduction = this->getHueChannel(i, j) / 360.0; break;
			case YCBCR: beforeReduction = this->getLumaChannel(i, j) / 255.0; break;
			case XYZ:   beforeReduction = this->getXChannel(i, j) / 255.0; break;
			case HLS:   beforeReduction = this->getHueChannel(i, j) / 360.0; break;
			case LAB:   beforeReduction = this->getLightness_CIEChannel(i, j) / 100.0; break;
			case LUV:   beforeReduction = this->getLightness_CIEChannel(i, j) / 100.0; break;
		}
	} else if ( channel == 2 ) {
		switch (this->getColorSpace() ) {
			case RGB:   beforeReduction = this->getGreenChannel(i, j) / 255.0; break;
			case HSV:   beforeReduction = this->getSaturationChannel(i, j); break;
			case YCBCR: beforeReduction = this->getChromaBlueChannel(i, j) / 255.0; break;
			case XYZ:   beforeReduction = this->getYChannel(i, j) / 255.0; break;
			case HLS:   beforeReduction = this->getLightnessChannel(i, j); break;
			case LAB:   beforeReduction = (this->getAChannel( i, j ) + 127) / 254.0; break;
			case LUV:   beforeReduction = (this->getUChannel( i, j ) + 134) / 354.0; break;
		}
	} else if ( channel == 3 ) {
		switch (this->getColorSpace() ) {
			case RGB:   beforeReduction = this->getBlueChannel(i, j) / 255.0; break;
			case HSV:   beforeReduction = this->getValueChannel(i, j); break;
			case YCBCR: beforeReduction = this->getChromaRedChannel(i, j) / 255.0; break;
			case XYZ:   beforeReduction = this->getZChannel(i, j) / 255.0; break;
			case HLS:   beforeReduction = this->getSaturation_LSChannel(i, j); break;
			case LAB:   beforeReduction = (this->getBChannel( i, j ) + 127) / 254.0; break;
			case LUV:   beforeReduction = (this->getVChannel( i, j ) + 140) / 262.0; break;
		}
	}
	double afterReduction = this->reduce( beforeReduction );
	return afterReduction;
}

//--
int Image::getRedChannel( int i, int j ) {
	return getRed( img, i, j );
}

int Image::getGreenChannel( int i, int j ) {
	return getGreen( img, i, j );
}

int Image::getBlueChannel( int i, int j ) {
	return getBlue( img, i, j );
}
//--
int Image::getHueChannel( int i, int j ) {
	return getHue( img, i, j );
}

double Image::getSaturationChannel( int i, int j ) {
	return getSaturation( img, i, j );
}

double Image::getValueChannel( int i, int j ) {
	return getHsvValue( img, i, j );
}
//--
int Image::getLumaChannel( int i, int j ) {
	return getLuma( img, i, j );
}

int Image::getChromaBlueChannel( int i, int j ) {
	return getChromaBlue( img, i, j );
}

int Image::getChromaRedChannel( int i, int j ) {
	return getChromaRed( img, i, j );
}
//--
int Image::getXChannel( int i, int j ) {
	return getX( img, i, j );
}

int Image::getYChannel( int i, int j ) {
	return getY( img, i, j );
}

int Image::getZChannel( int i, int j ) {
	return getZ( img, i, j );
}
//--
double Image::getLightnessChannel(int i, int j) {
	return getLightness(img, i, j);
}

double Image::getSaturation_LSChannel(int i, int j) {
	return getSaturation_LS(img, i, j);
}
//--
int Image::getLightness_CIEChannel( int i, int j ) {
	return getLightness_CIE( img, i, j );
}

int Image::getAChannel( int i, int j ) {
	return getA( img, i, j );
}

int Image::getBChannel( int i, int j ) {
	return getB( img, i, j );
}
//--
int Image::getUChannel( int i, int j ) {
	return getU( img, i, j );
}

int Image::getVChannel( int i, int j ) {
	return getV( img, i, j );
}


double Image::reduce( double d ) {
	if ( this->numberOfLevels < 256 ) {
		return floor((d * 255 * numberOfLevels) / 256) / (numberOfLevels - 1);
	} else {
		return d;
	}
}

void Image::showAndWait(const char* windowName) {
	show( windowName );
	cvWaitKey( 0 );
	hide( windowName );
} 

void Image::show( const char* windowName ) {	
	cvNamedWindow( windowName );
	cvShowImage( windowName, this->img );
}
 
void Image::hide( const char* windowName ) {	
	cvDestroyWindow( windowName );
}

void Image::negative() {
	for(int i=0;i<this->getHeight();i++) {
		for(int j=0;j<this->getWidth();j++) {
			this->setPixel( i,  j, 255-this->getRedChannel(i,j), 255-this->getGreenChannel(i,j), 255-this->getBlueChannel(i,j) );
		}
	}
}

void Image::save( const char* fileName ) {
	cvSaveImage( fileName, this->img );
}

void Image::toGray() {
	if ( this->getColorSpace() != GL ) {
		IplImage* converted = cvCreateImage(cvSize(this->getWidth(),this->getHeight()), IPL_DEPTH_8U, 1 );
		cvCvtColor( img, converted, CV_BGR2GRAY );
		cvReleaseImage(&img);
		this->img = converted;
		this->setColorSpace(GL);
	} 
}

void Image::binarize(int lower, int upper) {
	for(int i=0;i<this->getHeight();i++) {
		for(int j=0;j<this->getWidth();j++) {
			int value = this->getGray(i,j);
			if ( value >= lower && value < upper ) {
				this->setGray(i,j,255);
			} else {
				this->setGray(i,j,0);
			}
		}
	}
}

IplImage* Image::getIplImage() {
	return img;
}

float Image::getNormalizedHorizontal( int j ) {
	return (1.0 * j) / this->getWidth();
}

float Image::getNormalizedVertical( int i ) {
	return (1.0 * i) / this->getHeight();
}

void Image::clear() {
	this->binarize(256, 257);
}
