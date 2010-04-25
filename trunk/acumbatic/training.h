#include "config.h"
#include <fann.h>
#include <floatfann.h>

#ifndef TRAINING_H_
#define TRAINING_H_

class TrainingSet {
	public: 
		TrainingSet( char*, Configuration*, int, int, bool );
		TrainingSet( Configuration*, int, bool );
	   ~TrainingSet();
	   
	   fann_train_data *getTrainingData();
	   long getQtdValidationSamples();
	   fann_train_data *getValidationData();
	   
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
