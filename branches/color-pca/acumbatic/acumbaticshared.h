#include <string>
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
#ifndef ACUMBATICSHARED_H_
#define ACUMBATICSHARED_H_

#define MIN_INPUT_RANGE -1.0
#define MAX_INPUT_RANGE 1.0

#define STUFF_RESULT 1.0
#define NON_STUFF_RESULT -1.0

#define EPSILON 0.00000000001

#define TEMP_TRAIN_FILE "temptrain.tmp"

#define TEMP_VALIDATION_FILE "tempval.tmp"

#define TRAIN_FILE "train.dat"

#define VALIDATION_FILE "val.dat"

#define MINIMUM_MSE_NET_FILE_NAME "bestSoFar.net"
#define NETWORK_FILE_NAME "network.net"
#define CONFIG_FILE_NAME "network.cfg"
#define THRESHOLD_FILE_NAME "threshold.th"

#define ANN_FILE_PREFIX "ann"
#define METRIX_FILE_PREFIX "mtr"
#define TH_FILE_PREFIX "th"
#define RESULT_FILE_PREFIX "res"
#define ANN_FILE_SEP "_"
#define METRIX_FILE_SEP "_"
#define TH_FILE_SEP "_"
#define RESULT_FILE_SEP "_"

void normalizeInputs(double*, double*, double*);
void normalizeInput(double*);
std::string getColorSpaceString(int);
std::string getMetrixFileName(std::string, int, int, int);
std::string getNNFileName(std::string, int, int, int);
std::string getThFileName(std::string, int, int, int);
std::string getResultFileName(std::string, int, int, int);
std::string getFileName(std::string, std::string, std::string, int, int, int);
double getFMeasure(double, double);

#endif /*ACUMBATICSHARED_H_*/
