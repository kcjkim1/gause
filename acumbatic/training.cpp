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
#include "training.h"
#include "dirit.h"
#include "extractor.h"
#include "acumbaticshared.h"

//rndConf says how many images it will use from the imageset
//kflod specifies the number of crossvalidations to be performed
DirectoryTrainingSet::DirectoryTrainingSet( char *trainingDirectory, Configuration* configuration, int rndConf, int horatio, bool delFiles ) {	
	setConfiguration( configuration );
	DirectoryIterator *di = new DirectoryIterator( trainingDirectory );
	FeatureExtractor *fe = new FeatureExtractor( this->configuration, horatio );
	
	#ifdef TS_DEBUG
		cout << "Created objects" << endl;
	#endif
	
	fe->init();
	#ifdef TS_DEBUG
		cout << "Initiated Feature Extractor" << endl;
	#endif	
	di->addSupportedImgExtension(".jpg");
	di->addSupportedImgExtension(".bmp");
	di->addSupportedMskExtension(".pgm");
	if ( rndConf != 0 ) {
		di->setRandom(rndConf);
	}
	di->iterate( fe );
	#ifdef TS_DEBUG
		cout << "Iterated throug " << trainingDirectory << endl;
	#endif	
	fe->consolidate();
	this->delFiles = delFiles;
	
	trainingData = fann_read_train_from_file(TRAIN_FILE);
	this->qtdValidationSamples = fe->getQtdValidationSamples();
	validationData = fann_read_train_from_file(VALIDATION_FILE);
	
	delete fe;
	delete di;
}

DirectoryTrainingSet::DirectoryTrainingSet( Configuration* configuration, int horatio, bool delFiles ) {	
	setConfiguration( configuration );	
    this->delFiles = delFiles;
    
	trainingData = fann_read_train_from_file(TRAIN_FILE);
	this->qtdValidationSamples = countValidationSamples(VALIDATION_FILE);
	validationData = fann_read_train_from_file(VALIDATION_FILE);
}

DirectoryTrainingSet::~DirectoryTrainingSet() {
	if ( delFiles ) {
		remove(TRAIN_FILE);
		remove(VALIDATION_FILE);
	}
	fann_destroy_train( trainingData );
	fann_destroy_train( validationData );
}

fann_train_data* DirectoryTrainingSet::getTrainingData( ) {
	return trainingData;
}

fann_train_data* DirectoryTrainingSet::getValidationData() {	
	return validationData;
}

void DirectoryTrainingSet::setConfiguration(Configuration* config) {
	this->configuration = config;
}

long DirectoryTrainingSet::getQtdValidationSamples() {
	return this->qtdValidationSamples;
}

long DirectoryTrainingSet::countValidationSamples(const char* fileName) {
	FILE* file = fopen(fileName, "r");
	long result;
	fscanf( file, "%ld", &result );
	fclose(file);	
	return result;
}