#include "handle.h"
#include "config.h"
#include "neural.h"
#include "img.h"
#include "acumbaticshared.h"

#include <iostream>

#ifndef METRICS_H_
#define METRICS_H_

class MetricsExtractor : public CallbackHandler {
	public:
		MetricsExtractor( NeuralNetwork*);
		virtual void handle( const char*, const char* );
		void init();
		void close();
	private:
		#ifdef METRIX_DEBUG
			Image* prove;
		#endif
	    FILE* metrix;
		long stuff, nonStuff, tp, fp, tn, fn;
		NeuralNetwork* nn;
		void setNeuralNetwork(NeuralNetwork*);
		long getStuff(Image*);
		long getNonStuff(Image*);
		long getTp(Image*, Image*);
		long getFp(Image*, Image*);
		long getTn(Image*, Image*);
		long getFn(Image*, Image*);
};

#endif /*METRICS_H_*/
