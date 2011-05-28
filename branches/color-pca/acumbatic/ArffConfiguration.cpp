/*
 *  GAuSe - An Automatic Image Segmenter Generator
 *  Copyright (C) 2011 Fernando Cardoso, Jovan Andrade
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

#include "acumbaticshared.h"
#include "cvutils.h"
#include "ArffConfiguration.h"
#include <fstream>
#include <iostream>
#include <string>
#include <math.h>

struct colorPCA {
	std::string channel1;
	std::string channel2;
	std::string channel3;
	std::string channel4;
	std::string channel5;

};

//colorPCA colorSpacePCA;

ArffConfiguration::ArffConfiguration(int numberOfLevels, int windowSize) {
	this->numberOfLevels = numberOfLevels;
	this->windowSize = windowSize;
}

ArffConfiguration::ArffConfiguration(string fileName) {
	std::ifstream configFile;
	std::string line;
	configFile.open(fileName.c_str());
	if (configFile.is_open()) {
		while (configFile.good()) {
			getline(configFile, line);
			this->colorSpacePCA.channel1 = line;
			getline(configFile, line);
			this->colorSpacePCA.channel2 = line;
			getline(configFile, line);
			this->colorSpacePCA.channel3 = line;
			getline(configFile, line);
			this->colorSpacePCA.channel4 = line;
			getline(configFile, line);
			this->colorSpacePCA.channel5 = line;
			getline(configFile, line);
			setNumberOfLevels(atoi(line.c_str()));
			getline(configFile, line);
			setWindowSize(atoi(line.c_str()));
		}
	}

	configFile.close();
}

int ArffConfiguration::getWindowSize() {
	return windowSize;
}

double ArffConfiguration::getColorPCA(int channel, int r, int g, int b,
		int pos_vert, int pos_hor) {
	string line;
	switch (channel) {
	case 1:
		line = this->colorSpacePCA.channel1;
		break;
	case 2:
		line = this->colorSpacePCA.channel2;
		break;
	case 3:
		line = this->colorSpacePCA.channel3;
		break;
	case 4:
		line = this->colorSpacePCA.channel4;
		break;
	case 5:
		line = this->colorSpacePCA.channel5;
		break;
	default:
		break;
	}
	string out;
	int esp;
	double convCanal[3];

	for (int i = 0; i < line.size(); ++i) {

		out = line.substr(i, 5);

		esp = switchSpace(out);
		if (esp != -1) {

			convertChannel(convCanal, esp, r, g, b);

			std::stringstream ss;
			if (esp == POS_V) {
				ss << pos_vert;
			} else if (esp == POS_H) {
				ss << pos_hor;
			} else {
				ss << getNormalizedChannel(esp,convCanal[esp - numBase(esp)]);
				//ss << roundtol(convCanal[esp - numBase(esp)]);
			}
			line.replace(i, 5, ss.str());
		}

	}

	return resolvEquation(line);
}

int ArffConfiguration::numBase(int reference) {
	if (reference == RGB_R || reference == RGB_G || reference == RGB_B) {
		return 0;
	} else if (reference == XYZ_X || reference == XYZ_Y || reference == XYZ_Z) {
		return 3;
	} else
	//rgb2ycbcr
	if (reference == YBR_Y || reference == YBR_B || reference == YBR_R) {
		return 6;
	} else
	//rgb2hsv
	if (reference == HSV_H || reference == HSV_S || reference == HSV_V) {
		return 9;
	} else
	//rgb2hsl
	if (reference == HSL_H || reference == HSL_S || reference == HSL_L) {
		return 12;
	} else
	//rgb2lab
	if (reference == LAB_L || reference == LAB_A || reference == LAB_B) {
		return 15;
	} else
	//rgb2luv
	if (reference == LUV_L || reference == LUV_U || reference == LUV_V) {
		return 18;
	} else if (reference == POS_V) {
		return 21;
	} else if (reference == POS_H) {
		return 22;
	} else {
		return 0;
	}
}

double ArffConfiguration::resolvEquation(string line) {
	double result;
	double a, b;
	string number;
	bool flagfim = true;
	int cont = 0;
	while (flagfim) {
		for (int l = cont; l < line.size(); ++l) {
			if (line.substr(l, 1).compare("*") != 0) {
				number = number + line.substr(l, 1);
			} else {
				cont = l + 1;
				std::istringstream buffer(number);
				buffer >> a;
				number = "";
				break;
			}
		}//for do *

		for (int x = cont; x < line.size(); ++x) {
			if (line.substr(x, 1).compare("%") == 0) {
				flagfim = false;
				std::istringstream buffer(number);
				buffer >> b;
				break;
			}
			if (line.substr(x, 1).compare("+") != 0) {
				number = number + line.substr(x, 1);
			} else {
				cont = x + 1;
				std::istringstream buffer(number);
				buffer >> b;
				number = "";
				break;
			}
		}

		result = result + (a * b);
		a = 0;
		b = 0;

	}//while

	return result;

}

std::string ArffConfiguration::processLine(std::string linha) {
	if (linha.substr(0, 2) == "@a") {
		//retira @attribute
		linha.replace(0, 11, "");
		//retira ... numeric
		if (linha.substr((linha.size() - 11), linha.size()).compare("É numeric")
				== 0) {
			linha.replace((linha.size() - 11), linha.size(), "%");
		}

		if (linha.substr(0, 1).compare("C") == 0)
			return "";
	}
			return linha;
}

int ArffConfiguration::getNumberOfLevels() {
	return this->numberOfLevels;
}

void ArffConfiguration::setColorPCA(std::string colorSpace, int canal) {
	switch (canal) {
	case 1:
		this->colorSpacePCA.channel1 = processLine(colorSpace);
		break;
	case 2:
		this->colorSpacePCA.channel2 = processLine(colorSpace);
		break;
	case 3:
		this->colorSpacePCA.channel3 = processLine(colorSpace);
		break;
	case 4:
		this->colorSpacePCA.channel4 = processLine(colorSpace);
		break;
	case 5:
		this->colorSpacePCA.channel5 = processLine(colorSpace);
		break;

	default:
		break;
	}
}

void ArffConfiguration::setNumberOfLevels(int numberOfLevels) {
	this->numberOfLevels = numberOfLevels;
}

void ArffConfiguration::setWindowSize(int windowSize) {
	this->windowSize = windowSize;
}
void ArffConfiguration::save() {
	std::ofstream configFile;
	configFile.open(CONFIG_FILE_NAME);
	configFile << this->colorSpacePCA.channel1 << std::endl;
	configFile << this->colorSpacePCA.channel2 << std::endl;
	configFile << this->colorSpacePCA.channel3 << std::endl;
	configFile << this->colorSpacePCA.channel4 << std::endl;
	configFile << this->colorSpacePCA.channel5 << std::endl;
	configFile << this->numberOfLevels << std::endl;
	configFile << this->windowSize << std::endl;
	configFile.close();
}

bool ArffConfiguration::isCompatible(ArffConfiguration * other) {
	bool checkWindowAndLevels = (this->numberOfLevels)
			== (other->getNumberOfLevels()) && (this->windowSize)
			== (other->getWindowSize());
	bool colorCheck = this->colorSpacePCA.channel1
			== other->colorSpacePCA.channel1 && this->colorSpacePCA.channel2
			== other->colorSpacePCA.channel2 && this->colorSpacePCA.channel3
			== other->colorSpacePCA.channel3 && this->colorSpacePCA.channel4
			== other->colorSpacePCA.channel4 && this->colorSpacePCA.channel5
			== other->colorSpacePCA.channel5;

	return checkWindowAndLevels && colorCheck;
}

void ArffConfiguration::rgb2HSL(int r, int g, int b, double HSL[3]) {
	double R, G, B, Vmax, Vmin;

	R = r / 255.f;
	G = g / 255.f;
	B = b / 255.f;

	Vmax = this->maxNumber(R, G, B);
	//Vmax = Math.max(Math.max(R, G), B);
	Vmin = this->minNumber(R, G, B);
	//Vmin = Math.min(Math.min(R, G), B);
	// Calculating L
	HSL[2] = (Vmax + Vmin) / 2;

	// Calculating S
	if (Vmax == Vmin) {
		HSL[1] = 0;
	} else if (HSL[2] <= 0.5) {
		HSL[1] = (Vmax - Vmin) / (2 * HSL[2]);

	} else if (HSL[2] > 0.5) {
		HSL[1] = (Vmax - Vmin) / (2 - (Vmax + Vmin));

	}

	// Calculating H
	if (HSL[1] == 0.0) {
		HSL[0] = 0; // H invï¿½lido
	}

	else {

		double delta = Vmax - Vmin;

		if (R == Vmax)
			HSL[0] = (G - B) / delta; // H
		else if (G == Vmax)
			HSL[0] = 2.0 + (B - R) / delta; // H
		else if (B == Vmax)
			HSL[0] = 4.0 + (R - G) / delta; // H

		HSL[0] = HSL[0] * 60; // convertendo os graus

		if (HSL[0] < 0.0)
			HSL[0] = HSL[0] + 360;

	}

}

void ArffConfiguration::rgb2HSV(int R, int G, int B, double hsv[3]) {
	double max, min;
	double r = R / 255.f;
	double g = G / 255.f;
	double b = B / 255.f;
	double H, S, V;

	//max = Math.max(r, Math.max(g, b));
	max = this->maxNumber(r, g, b);
	//min = Math.min(r, Math.min(g, b));
	min = this->minNumber(r, g, b);

	V = max; // V

	if (max != 0.0)
		S = (max - min) / max; // S
	else
		S = 0.0; // S

	if (S == 0.0) {
		H = 0.0; // H invï¿½lido
	} else {
		double delta = max - min;
		if (r == max)
			H = (g - b) / delta; // H
		else if (g == max)
			H = 2.0 + (b - r) / delta; // H
		else if (b == max)
			H = 4.0 + (r - g) / delta; // H

		H = H * 60.0; // convertendo os graus
		if (H < 0.0)
			H = H + 360.0;
	}

	hsv[0] = H;
	hsv[1] = (double) S;
	hsv[2] = (double) V;
}

void ArffConfiguration::rgb2XYZ(int r, int g, int b, double xyz[3]) {

	xyz[0] = (0.412453 * r) + (0.357580 * g) + (0.180423 * b);
	xyz[1] = (0.212671 * r) + (0.715160 * g) + (0.072169 * b);
	xyz[2] = (0.019334 * r) + (0.119193 * g) + (0.950227 * b);
	//
	//		if(xyz[0] > 255)
	//			xyz[0] = 255;
	//		if(xyz[1] > 255)
	//			xyz[1] = 255;
	//		if(xyz[2] > 255)
	//			xyz[2] = 255;
}

void ArffConfiguration::rgb2YCbCr(int r, int g, int b, double ycbcr[3]) {

	ycbcr[0] = ((0.299 * r) + (0.587 * g) + (0.114 * b));
	ycbcr[1] = ((b - ycbcr[0]) * 0.564 + 128);
	ycbcr[2] = ((r - ycbcr[0]) * 0.713 + 128);

}
void ArffConfiguration::rgb2LAB(int r, int g, int b, double Lab[3]) {

	double X, Y, Z;
	// COnverting XYZ

	double var_R = r / 255.f; // R from 0 to 255
	double var_G = g / 255.f; // G from 0 to 255
	double var_B = b / 255.f; // B from 0 to 255

	if (var_R > 0.04045)
		var_R = pow(((var_R + 0.055) / 1.055), 2.4);
	else
		var_R = var_R / 12.92;
	if (var_G > 0.04045)
		var_G = pow(((var_G + 0.055) / 1.055), 2.4);
	else
		var_G = var_G / 12.92;
	if (var_B > 0.04045)
		var_B = pow(((var_B + 0.055) / 1.055), 2.4);
	else
		var_B = var_B / 12.92;

	var_R = var_R * 100;
	var_G = var_G * 100;
	var_B = var_B * 100;

	X = (var_R * 0.4124 + var_G * 0.3576 + var_B * 0.1805);
	Y = (var_R * 0.2126 + var_G * 0.7152 + var_B * 0.0722);
	Z = (var_R * 0.0193 + var_G * 0.1192 + var_B * 0.9505);

	double ref_X = 95.047;
	double ref_Y = 100.000;
	double ref_Z = 108.883;
	double var_X = X / ref_X;
	double var_Y = Y / ref_Y;
	double var_Z = Z / ref_Z;
	double raiz = (double) 1 / 3;

	if (var_X > 0.008856)
		var_X = pow(var_X, raiz);
	else
		var_X = (7.787 * var_X) + (16 / 116);
	if (var_Y > 0.008856)
		var_Y = pow(var_Y, raiz);
	else
		var_Y = (7.787 * var_Y) + (16 / 116);
	if (var_Z > 0.008856)
		var_Z = pow(var_Z, raiz);
	else
		var_Z = (7.787 * var_Z) + (16 / 116);

	if ((Y / ref_Y) > 0.008856)
		Lab[0] = (116 * var_Y) - 16;
	else
		Lab[0] = 903.3 * (Y / ref_Y);
	Lab[1] = (500 * (var_X - var_Y));
	Lab[2] = (200 * (var_Y - var_Z));

}
void ArffConfiguration::rgb2LUV(int r, int g, int b, double Luv[3]) {

	double X, Y, Z;
	// COnverting XYZ

	double var_R = r / 255.f; // R from 0 to 255
	double var_G = g / 255.f; // G from 0 to 255
	double var_B = b / 255.f; // B from 0 to 255
	double raiz = (double) 1 / 3;

	if (var_R > 0.04045)
		var_R = pow(((var_R + 0.055) / 1.055), 2.4);
	else
		var_R = var_R / 12.92;
	if (var_G > 0.04045)
		var_G = pow(((var_G + 0.055) / 1.055), 2.4);
	else
		var_G = var_G / 12.92;
	if (var_B > 0.04045)
		var_B = pow(((var_B + 0.055) / 1.055), 2.4);
	else
		var_B = var_B / 12.92;

	var_R = var_R * 100;
	var_G = var_G * 100;
	var_B = var_B * 100;

	X = (var_R * 0.4124 + var_G * 0.3576 + var_B * 0.1805);
	Y = (var_R * 0.2126 + var_G * 0.7152 + var_B * 0.0722);
	Z = (var_R * 0.0193 + var_G * 0.1192 + var_B * 0.9505);

	double var_Un = 4 * X;
	double var_D = (X + (15 * Y) + (3 * Z));
	double var_Vn = 9 * Y;
	double var_U = 0.0;
	double var_V = 0.0;
	if ((var_Un == 0.0 && var_D == 0.0) || (var_Vn == 0.0 && var_D == 0.0)) {
		var_U = 0;
		var_V = 0;
	} else {
		var_U = var_Un / var_D;
		var_V = var_Vn / var_D;
	}

	double var_Y = Y / 100;
	if (var_Y > 0.008856)
		var_Y = pow(var_Y, raiz);
	else
		var_Y = (7.787 * var_Y) + (16 / 116);

	double ref_X = 98.074;
	double ref_Y = 100.000;
	double ref_Z = 118.232;

	double ref_U = (4 * ref_X) / (ref_X + (15 * ref_Y) + (3 * ref_Z));
	double ref_V = (9 * ref_Y) / (ref_X + (15 * ref_Y) + (3 * ref_Z));

	if ((Y / ref_Y) > 0.008856)
		Luv[0] = (116 * var_Y) - 16;
	else
		Luv[0] = 903.3 * (Y / ref_Y);

	Luv[1] = 13 * Luv[0] * (var_U - ref_U);
	Luv[2] = 13 * Luv[0] * (var_V - ref_V);

}

double ArffConfiguration::maxNumber(double a, double b, double c) {
	double maior;
	if (a > b)
		maior = a;
	else
		maior = b;

	if (maior < c)
		maior = c;

	return maior;
}

double ArffConfiguration::minNumber(double a, double b, double c) {
	double menor;
	if (a < b)
		menor = a;
	else
		menor = b;

	if (menor > c)
		menor = c;

	return menor;
}

void ArffConfiguration::convertChannel(double espace[3], int outSpace, int r,
		int g, int b) {

	//rgb2xyz
	if (outSpace == XYZ_X || outSpace == XYZ_Y || outSpace == XYZ_Z) {
		rgb2XYZ(r, g, b, espace);
	} else
	//rgb2ycbcr
	if (outSpace == YBR_Y || outSpace == YBR_B || outSpace == YBR_R) {
		rgb2YCbCr(r, g, b, espace);
	} else
	//rgb2hsv
	if (outSpace == HSV_H || outSpace == HSV_S || outSpace == HSV_V) {
		rgb2HSV(r, g, b, espace);
	} else
	//rgb2hsl
	if (outSpace == HSL_H || outSpace == HSL_S || outSpace == HSL_L) {
		rgb2HSL(r, g, b, espace);
	} else
	//rgb2lab
	if (outSpace == LAB_L || outSpace == LAB_A || outSpace == LAB_B) {
		rgb2LAB(r, g, b, espace);
	} else
	//rgb2luv
	if (outSpace == LUV_L || outSpace == LUV_U || outSpace == LUV_V) {
		rgb2LUV(r, g, b, espace);
	} else {

	}
}

int ArffConfiguration::switchSpace(std::string espaco) {
	if (espaco.compare("RGB_R") == 0) {
		return 0;
	} else if (espaco.compare("RGB_G") == 0) {
		return 1;
	} else if (espaco.compare("RGB_B") == 0) {
		return 2;
	} else if (espaco.compare("XYZ_X") == 0) {
		return 3;
	} else if (espaco.compare("XYZ_Y") == 0) {
		return 4;
	} else if (espaco.compare("XYZ_Z") == 0) {
		return 5;
	} else if (espaco.compare("YBR_Y") == 0) {
		return 6;
	} else if (espaco.compare("YBR_B") == 0) {
		return 7;
	} else if (espaco.compare("YBR_R") == 0) {
		return 8;
	} else if (espaco.compare("HSV_H") == 0) {
		return 9;
	} else if (espaco.compare("HSV_S") == 0) {
		return 10;
	} else if (espaco.compare("HSV_V") == 0) {
		return 11;
	} else if (espaco.compare("HSL_H") == 0) {
		return 12;
	} else if (espaco.compare("HSL_S") == 0) {
		return 13;
	} else if (espaco.compare("HSL_L") == 0) {
		return 14;
	} else if (espaco.compare("LAB_L") == 0) {
		return 15;
	} else if (espaco.compare("LAB_A") == 0) {
		return 16;
	} else if (espaco.compare("LAB_B") == 0) {
		return 17;
	} else if (espaco.compare("LUV_L") == 0) {
		return 18;
	} else if (espaco.compare("LUV_U") == 0) {
		return 19;
	} else if (espaco.compare("LUV_V") == 0) {
		return 20;
	} else if (espaco.compare("POS_V") == 0) {
		return 21;
	} else if (espaco.compare("POS_H") == 0) {
		return 22;
	} else {
		return -1;
	}
}

double ArffConfiguration::getNormalizedChannel(int espaceNumber, double value) {
	int max, min;
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
	switch (espaceNumber) {
	//RGB_R
	case 0:
		max = 255;
		min = 0;
		break;
		//RGB_G
	case 1:
		max = 255;
		min = 0;
		break;
		//RGB_B
	case 2:
		max = 255;
		min = 0;
		break;
		//XYZ_X
	case 3:
		max = 255;
		min = 0;
		break;
		//XYZ_Y
	case 4:
		max = 255;
		min = 0;
		break;
		//XYZ_Z
	case 5:
		max = 255;
		min = 0;
		break;
		//YCbCr_Y
	case 6:
		max = 255;
		min = 0;
		break;
		//YCbCr_Cb
	case 7:
		max = 255;
		min = 0;
		break;
		//YCbCr_Cr
	case 8:
		max = 255;
		min = 0;
		break;
		//HSV_H
	case 9:
		max = 360;
		min = 0;
		break;
		//HSV_S
	case 10:
		max = 1;
		min = 0;
		break;
		//HSV_V
	case 11:
		max = 1;
		min = 0;
		break;
		//HSL_H
	case 12:
		max = 360;
		min = 0;
		break;
		//HSL_S
	case 13:
		max = 1;
		min = 0;
		break;
		//HSL_L
	case 14:
		max = 1;
		min = 0;
		break;
		//LAB_L
	case 15:
		max = 100;
		min = 0;
		break;
		//LAB_A
	case 16:
		max = 127;
		min = -127;
		break;
		//LAB_B
	case 17:
		max = 127;
		min = -127;
		break;
		//LUV_L
	case 18:
		max = 100;
		min = 0;
		break;
		//LUV_U
	case 19:
		max = 220;
		min = -134;
		break;
		//LUV_V
	case 20:
		max = 122;
		min = -140;
		break;
	default:
		max = 0;
		min = 0;
		break;
	}

	float n1 = (float) (value - min);
	float n2 = (float) (max - min);
	float n = (float) (2 * (n1 / n2)) - 1;

	return n;
}

int ArffConfiguration::getDimension() {
	return 0;
}

void ArffConfiguration::preprocess(Image*) {

}

std::string ArffConfiguration::getColorSpaceAndPositionDescription() {
	 std::string description = "PCA";
	    return description;
}

void ArffConfiguration::fillInput(float[], Image*, int, int) {
	//TODO: Implementar!
}
