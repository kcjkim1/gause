#include <iostream>

#include "handle.h"
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
 
#include "SerialConfiguration.h"
#include "TrainPattern.h"

#ifndef EXTRACTOR_H_
#define EXTRACTOR_H_

struct node {
	TrainPattern * pattern;
    node *nxt;
};

class FeatureExtractor : public CallbackHandler {
	public:
		FILE* tempTrainingFile;
		FILE* tempValidationFile;

		virtual void handle( const char*, const char* );
		void init();
		void preconsolidate();
		void consolidate();
		
		FeatureExtractor( SerialConfiguration*, int ); //int: the kfold parameter
	   ~FeatureExtractor();
	   long getQtdValidationSamples();
	private:
		node *start_ptr;
		long size;
		SerialConfiguration* config;
		long qtdTrainingSamples;
		long qtdValidationSamples;
		int hor; //hor = (h)old (o)ut (r)atio
		void dealWith( TrainPattern *, bool );
		long createValidationData();
		void putHeader(FILE*, const char*, const char*, long);
};

#endif /*EXTRACTOR_H_*/
