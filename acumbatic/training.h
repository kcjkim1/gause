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
#include "config.h"
#include <fann.h>
#include <floatfann.h>
#include "TrainingSet.h"

#ifndef TRAINING_H_
#define TRAINING_H_

class DirectoryTrainingSet : public TrainingSet {
	public: 
		DirectoryTrainingSet( char*, Configuration*, int, int, bool );
		DirectoryTrainingSet( Configuration*, int, bool );
	   ~DirectoryTrainingSet();
	   
	   virtual fann_train_data *getTrainingData();
	   virtual long getQtdValidationSamples();
	   virtual fann_train_data *getValidationData();
	   
	   void saveValidationData();
	private:
		bool delFiles;
		Configuration *configuration;
		void setConfiguration( Configuration* );
		fann_train_data * trainingData;
		long qtdValidationSamples;
		long countValidationSamples(const char *);
		fann_train_data * validationData;
};

#endif /*TRAINING_H_*/
