#include "config.h"
#include "training.h"
#include "img.h"
#include <iostream>

#include <fann.h>
#include <floatfann.h>

#ifndef NEURAL_H_
#define NEURAL_H_

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

