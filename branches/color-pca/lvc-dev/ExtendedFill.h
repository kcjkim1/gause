#ifndef EXTENDEDFILL_H_
#define EXTENDEDFILL_H_

#include "img.h"

class ExtendedFill {
	public:
		ExtendedFill();
		ExtendedFill( Image*, Image*, double, double);
		virtual ~ExtendedFill();
		
		Image * run();
		void    setVerbose(bool);
	private:
		Image* source;
		Image* seeds;
		int areaThreshold;
		int borderThreshold;
		bool verbose;
		
		void floodIt(Image *, int, int, Image *, long &, long &, long &, long &);
		bool checkThreshold(int, int, int, long, long, long, int );
		void updateMean(int, int, int, long &, long &, long &, long & );
};

#endif /*EXTENDEDFILL_H_*/
