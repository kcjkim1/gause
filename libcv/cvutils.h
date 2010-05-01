#ifndef CVUTILS_H_
#define CVUTILS_H_

#define getRed(img,i,j) (((uchar *)(img->imageData + i*img->widthStep))[j*img->nChannels + 2]) //[0..255]
#define getGreen(img,i,j) (((uchar *)(img->imageData + i*img->widthStep))[j*img->nChannels + 1]) //[0..255]
#define getBlue(img,i,j) (((uchar *)(img->imageData + i*img->widthStep))[j*img->nChannels + 0]) //[0..255]

#define setRed(img,i,j,r) (((uchar *)(img->imageData + i*img->widthStep))[j*img->nChannels + 2]) = r
#define setGreen(img,i,j,g) (((uchar *)(img->imageData + i*img->widthStep))[j*img->nChannels + 1]) = g
#define setBlue(img,i,j,b) (((uchar *)(img->imageData + i*img->widthStep))[j*img->nChannels + 0]) = b
 
//HSV
#define getHsvValue(img,i,j) (((uchar *)(img->imageData + i*img->widthStep))[j*img->nChannels + 2]) / 255.0 //[0..1]
#define getSaturation(img,i,j) (((uchar *)(img->imageData + i*img->widthStep))[j*img->nChannels + 1]) / 255.0 //[0..1]
#define getHue(img,i,j) 2*(((uchar *)(img->imageData + i*img->widthStep))[j*img->nChannels + 0]) //[0..360]

//YCbCr
#define getChromaBlue(img,i,j) (((uchar *)(img->imageData + i*img->widthStep))[j*img->nChannels + 2]) //[0..255]
#define getChromaRed(img,i,j) (((uchar *)(img->imageData + i*img->widthStep))[j*img->nChannels + 1]) //[0..255]
#define getLuma(img,i,j) (((uchar *)(img->imageData + i*img->widthStep))[j*img->nChannels + 0]) //[0..255]

//XYZ
#define getZ(img,i,j) (((uchar *)(img->imageData + i*img->widthStep))[j*img->nChannels + 2]) //[0..255]
#define getY(img,i,j) (((uchar *)(img->imageData + i*img->widthStep))[j*img->nChannels + 1]) //[0..255]
#define getX(img,i,j) (((uchar *)(img->imageData + i*img->widthStep))[j*img->nChannels + 0]) //[0..255]

//HLS
#define getSaturation_LS(img,i,j) (((uchar *)(img->imageData + i*img->widthStep))[j*img->nChannels + 2]) / 255.0 //[0..1]
#define getLightness(img,i,j) (((uchar *)(img->imageData + i*img->widthStep))[j*img->nChannels + 1]) / 255.0 //[0..1]

//L*a*b* 
#define getB(img,i,j) (((uchar *)(img->imageData + i*img->widthStep))[j*img->nChannels + 2]) - 128 //[-127..127]
#define getA(img,i,j) (((uchar *)(img->imageData + i*img->widthStep))[j*img->nChannels + 1]) - 128 // [-127..127]
#define getLightness_CIE(img,i,j) (((uchar *)(img->imageData + i*img->widthStep))[j*img->nChannels + 0]) * 100 / 255 // [0..100]

//L*u*v* 
#define getV(img,i,j) ((((uchar *)(img->imageData + i*img->widthStep))[j*img->nChannels + 2]) * 262 /255 ) - 140  //-140=v=122
#define getU(img,i,j) ((((uchar *)(img->imageData + i*img->widthStep))[j*img->nChannels + 1]) * 354 /255 ) - 134  //-134=u=220

//GL
#define getGrayLevel(img,i,j) (((uchar *)(img->imageData + i*img->widthStep))[j*img->nChannels + 0]) //[0..255]
#define setGrayLevel(img,i,j,value) (((uchar *)(img->imageData + i*img->widthStep))[j*img->nChannels + 0]) = value

#define RGB 0
#define HSV 1
#define YCBCR 2
#define XYZ 3
#define HLS 4
#define LAB 5
#define LUV 6
#define GL 7

#define IS_STUFF 255
#define DONT_CARE 127
#define IS_NON_STUFF 0

#include "highgui.h"
#include "cv.h"
#include "cvaux.h"
#include "cxcore.h"
#include "cvgabor.h"

#include "img.h"

#include <string>

int getConvertion( int newColorSpace );
bool fileExists(std::string);

Image* binaryAnd( Image*, Image* );

#endif /*CVUTILS_H_*/
