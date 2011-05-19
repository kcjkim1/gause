/*
 *  GAuSe - An Automatic Image Segmenter Generator
 *  Copyright (C) 2011 Fernando Cradoso, Jovan Andrade
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef ARFFCONFIGURATION_H_
#define ARFFCONFIGURATION_H_


#define RGB_R 0
#define RGB_G 1
#define RGB_B 2
#define XYZ_X 3
#define XYZ_Y 4
#define XYZ_Z 5
#define YCbCr_Y 6
#define YCbCr_Cb 7
#define YCbCr_Cr 8
#define HSV_H 9
#define HSV_S 10
#define HSV_V 11
#define HSL_H 12
#define HSL_S 13
#define HSL_L 14
#define LAB_L 15
#define LAB_A 16
#define LAB_B 17
#define LUV_L 18
#define LUV_U 19
#define LUV_V 20


#include "Configuration.h"
#include "img.h"
/*
 //Definition Color Mapping
 0=RGB_R 			10=HSV_S
 1=RGB_G				11=HSV_V
 2=RGB_B				12=HSL_H
 3=XYZ_X				13=HSL_S
 4=XYZ_Y				14=HSL_L
 5=XYZ_Z				15=LAB_L
 6=YCbCr_Y			16=LAB_A
 7=YCbCr_Cb			17=LAB_B
 8=YCbCr_Cr			18=LUV_L
 9=HSV_H				19=LUV_U
 20=LUV_V

 */
class ArffConfiguration : public Configuration {

public:
	struct colorPCA {
		std::string channel1;
		std::string channel2;
		std::string channel3;
		std::string channel4;
		std::string channel5;

	};
	ArffConfiguration(int, int);
	ArffConfiguration(std::string);
	virtual int getWindowSize();
	virtual std::string getColorSpaceAndPositionDescription();
	virtual int getNumberOfLevels();
	virtual void fillInput(float[], Image*, int, int);
	double* getColorPCA(int,int,int,int);
	void setColorPCA(std::string,int);
	void save();
	bool isCompatible(ArffConfiguration*);
	double* rgb2HSL(int, int, int);
	double* rgb2HSV(int, int, int);
	double* rgb2XYZ(int, int, int);
	double* rgb2YCbCr(int, int, int);
	double* rgb2LAB(int, int, int);
	double* rgb2LUV(int, int, int);
	int getDimension();
	void preprocess(Image*);
protected:
	int windowSize;
	colorPCA colorSpacePCA;
	int numberOfLevels;
	double maxNumber(double, double, double);
	double minNumber(double, double, double);
	void setNumberOfLevels(int);
	void setWindowSize(int);
	double getNormalizedChannel(int, double);
	double convertChannel(int, int, int, int, int);

};

#endif /* ARFFCONFIGURATION_H_ */
