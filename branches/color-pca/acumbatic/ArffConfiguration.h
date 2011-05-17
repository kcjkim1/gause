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


#define RGB-R 0
#define RGB-G 1
#define RGB-B 2
#define XYZ-X 3
#define XYZ-Y 4
#define XYZ-Z 5
#define YCbCr-Y 6
#define YCbCr-Cb 7
#define YCbCr-Cr 8
#define HSV-H 9
#define HSV-S 10
#define HSV-V 11
#define HSL-H 12
#define HSL-S 13
#define HSL-L 14
#define LAB-L 15
#define LAB-A 16
#define LAB-B 17
#define LUV-L 18
#define LUV-U 19
#define LUV-V 20


#include "Configuration.h"
#include "img.h"
/*
 //Definition Color Mapping
 0=RGB-R 			10=HSV-S
 1=RGB-G				11=HSV-V
 2=RGB-B				12=HSL-H
 3=XYZ-X				13=HSL-S
 4=XYZ-Y				14=HSL-L
 5=XYZ-Z				15=LAB-L
 6=YCbCr-Y			16=LAB-A
 7=YCbCr-Cb			17=LAB-B
 8=YCbCr-Cr			18=LUV-L
 9=HSV-H				19=LUV-U
 20=LUV-V

 */
class ArffConfiguration: public Configuration {


	struct colorPCA {
		std::string channel1;
		std::string channel2;
		std::string channel3;
		std::string channel4;
		std::string channel5;

	};
public:
	ArffConfiguration(int, int, int);
	ArffConfiguration(std::string);
	virtual int getWindowSize();
	virtual std::string getColorSpaceAndPositionDescription();
	virtual int getNumberOfLevels();
	virtual void preprocess(Image * img);
	virtual void fillInput(float[], Image*, int, int);
	colorPCA getColorPCA();
	void setColorPCA(int);
	void save();
	bool isCompatible(ArffConfiguration*);
	double* rgb2HSL(int, int, int);
	double* rgb2HSV(int, int, int);
	double* rgb2XYZ(int, int, int);
	double* rgb2YCbCr(int, int, int);
	double* rgb2LAB(int, int, int);
	double* rgb2LUV(int, int, int);
protected:
	int windowSize;
	int colorSpace;
	int numberOfLevels;
	double maxNumber(double, double, double);
	double minNumber(double, double, double);
	//void setColorSpace(int);
	void setNumberOfLevels(int);
	void setWindowSize(int);
	double getNormalizedChannel(int, double);
	double convertChannel(int, int, int, int, int);

};

#endif /* ARFFCONFIGURATION_H_ */
