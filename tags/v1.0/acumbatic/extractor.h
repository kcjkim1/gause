#include <iostream>

#include "handle.h"
#include "config.h"
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
		
		FeatureExtractor( Configuration*, int ); //int: the kfold parameter
	   ~FeatureExtractor();
	   long getQtdValidationSamples();
	private:
		node *start_ptr;
		long size;
		Configuration* config;
		long qtdTrainingSamples;
		long qtdValidationSamples;
		int hor; //hor = (h)old (o)ut (r)atio
		void dealWith( TrainPattern *, bool );
		long createValidationData();
		void putHeader(FILE*, const char*, const char*, long);
};

#endif /*EXTRACTOR_H_*/