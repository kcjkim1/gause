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

#ifndef NEURAL_H_
#define NEURAL_H_

#include "Configuration.h"
#include "TrainingSet.h"
#include "img.h"
#include <iostream>

#include <fann.h>
#include <floatfann.h>

class NeuralNetwork {
	public:
		NeuralNetwork( int, Configuration* );
		NeuralNetwork( string, string, Configuration*  );
	   ~NeuralNetwork( );
		void train( TrainingSet*, int, int, int, double );
		struct fann* nn;
		double th;
		void save();
		int getHiddenNeurons();
		Configuration* getConfiguration();
		Image* apply( Image* );
		FILE* resultFile;
	private:
		int hiddenNeurons;
		Configuration *configuration;
		TrainingSet* ts;
		void setHiddenNeurons( int );		
		void setConfiguration( Configuration* );
		struct fann* trainAnn( int, fann_train_data*, fann_train_data*, int, int );
		double learnThreshold( struct fann*, double, double*, double* );
		double testThreshold( struct fann*, double, double& );
		int applyAnn( struct fann*, float*, double );		
		void saveMetrix( double, double, double );
		void initResults();
		void saveResults( int, double, double, double );
		void closeResults();		
};

#endif /*NEURAL_H_*/

