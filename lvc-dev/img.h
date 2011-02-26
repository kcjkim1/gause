#include "cvutils.h"

#ifndef IMG_H_
#define IMG_H_
 
class Image {
	public:
		Image(const char*, int);
		Image(const char*);
		Image(int, int);
		Image(int, int, int);
		Image( IplImage*, int, int );
		~Image();
		
		void smooth( int ); 
		void blur( int );
		void convert( int );
		
		int getHeight();
		int getWidth();
		
		void setNumberOfLevels( int );
		
		void setColorSpace( int );
		int getColorSpace( );
		
		int getGray( int, int );
		void setGray( int, int, int );
		
		double getNomalizedChannel(int, int, int);
		double getChannel(int, int, int);
		
		int getRedChannel(int, int);
		int getGreenChannel(int, int);
		int getBlueChannel(int, int);
		
		int getHueChannel(int, int);
		double getSaturationChannel(int, int);
		double getValueChannel(int, int);
		
		int getLumaChannel(int, int);
		int getChromaBlueChannel( int, int );
		int getChromaRedChannel( int, int );
		
		int getXChannel(int, int);
		int getYChannel(int, int);
		int getZChannel(int, int);
		
		//int getHueChannel(int, int);
		double getLightnessChannel( int, int );
		double getSaturation_LSChannel( int, int );
		
		int getLightness_CIEChannel(int, int);
		int getAChannel(int, int );
		int getBChannel(int, int );
		
		//int getLightness_CIEChannel(int, int);
		int getUChannel(int, int );
		int getVChannel(int, int );
		
		
		void showAndWait(const char*);
		void show(const char*);
		void hide(const char*);
		
		void setPixel(int i, int j, int r, int g, int b );
		
		void negative();
		
		void save(const char*);
		
		void toGray();
		
		void binarize(int, int);
		
		IplImage* getIplImage();
		
		void resize( int, int );
		
		float getNormalizedVertical(int);
		float getNormalizedHorizontal(int);
		
		void clear();
	private:
		IplImage* img;
		
		int numberOfLevels;
		int colorSpace;
		
		double reduce( double );
};

#endif /*IMG_H_*/
