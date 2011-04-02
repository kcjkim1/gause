/*
 *  GAuSe - An Automatic Image Segmenter Generator
 *  Copyright (C) 2011 Fernando Cardoso
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

void normalizeInputs( double *ch1, double *ch2, double *ch3 ) {
	normalizeInput(ch1);
	normalizeInput(ch2);
	normalizeInput(ch3);
}

void normalizeInput( double *i ) {
	(*i) = (*i) * (MAX_INPUT_RANGE - MIN_INPUT_RANGE) + MIN_INPUT_RANGE;
}

std::string getColorSpaceString( int colorspace ) {
	switch (colorspace) {
		case RGB: return "RGB"; break;
		case HSV: return "HSV"; break;
		case YCBCR: return "YCbCr"; break;
		case XYZ: return "XYZ"; break;
		case HLS: return "HLS"; break;
		case LAB: return "Lab"; break;
		case LUV: return "Luv"; break;
		default : return "Invalid";
	}
}

std::string getMetrixFileName( int colorspace, int numberOfLevels, int windowSize, std::string positionDescription, int hiddenNeurons ) {
	return getFileName( METRIX_FILE_PREFIX, METRIX_FILE_SEP, colorspace, numberOfLevels, windowSize, positionDescription, hiddenNeurons );
}

std::string getNNFileName( int colorspace, int numberOfLevels, int windowSize, std::string positionDescription, int hiddenNeurons ) {
	return getFileName( ANN_FILE_PREFIX, ANN_FILE_SEP, colorspace, numberOfLevels, windowSize, positionDescription, hiddenNeurons );
}

std::string getThFileName( int colorspace, int numberOfLevels, int windowSize, std::string positionDescription, int hiddenNeurons ) {
	return getFileName( TH_FILE_PREFIX, TH_FILE_SEP, colorspace, numberOfLevels, windowSize, positionDescription, hiddenNeurons );
}

std::string getResultFileName( int colorspace, int numberOfLevels, int windowSize, std::string positionDescription, int hiddenNeurons ) {
	return getFileName( RESULT_FILE_PREFIX, RESULT_FILE_SEP, colorspace, numberOfLevels, windowSize, positionDescription, hiddenNeurons ); 
}

std::string getFileName( std::string prefix, std::string separator, int colorspace, int numberOfLevels, int windowSize, std::string positionDescription, int hiddenNeurons ) {
	std::string fileName = prefix;
	fileName = fileName + separator + getColorSpaceString( colorspace );
	char buff[10];
	sprintf(buff,"%d",numberOfLevels);
	fileName = fileName + separator + buff;
	sprintf(buff,"%d",windowSize);
	fileName = fileName + separator + buff;
	fileName = fileName + separator + positionDescription;
	sprintf(buff,"%d",hiddenNeurons);
	fileName = fileName + separator + buff;
	return fileName;
}

double getFMeasure( double detectionRate, double precision ) {
	if ( detectionRate + precision == 0 ) {
		return 0;
	} else {
		return 2 * detectionRate * precision / (detectionRate + precision );
	}
}
