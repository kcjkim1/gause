#ifndef GABORFILTER_H_
#define GABORFILTER_H_

#include <img.h>

class GaborFilter
{
public:
	GaborFilter(int, int);
	virtual ~GaborFilter();
	
	Image* apply(Image*);
	
	Image* getKernel( double, int, int );
private:
	int orientations;
	int scales;
	
	Image* getGaborMagnitude(Image*, double, int);
};

#endif /*GABORFILTER_H_*/
