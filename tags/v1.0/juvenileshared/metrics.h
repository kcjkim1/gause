#include "handle.h"
#include "img.h"

#include <iostream>

#ifndef METRICS_H_
#define METRICS_H_	

class MetricsExtractor : public CallbackHandler {
	public:
		MetricsExtractor(int, int, int, int);
		virtual void handle( const char*, const char* );
		void init();
		void close(double &);
	private:
		#ifdef METRIX_DEBUG
			Image* prove;
		#endif
		int orientations;
		int scales;
		int lower;
		int upper;
		long stuff, nonStuff, tp, fp, tn, fn;
		long getStuff(Image*);
		long getNonStuff(Image*);
		long getTp(Image*, Image*);
		long getFp(Image*, Image*);
		long getTn(Image*, Image*);
		long getFn(Image*, Image*);
};

#endif /*METRICS_H_*/
