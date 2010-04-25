#ifndef MINIMUMDIMENSIONEXTRACTOR_H_
#define MINIMUMDIMENSIONEXTRACTOR_H_

#include <handle.h>

class MinimumDimensionExtractor : public CallbackHandler
{
	private:
		int result;
	public:
		MinimumDimensionExtractor();
		virtual ~MinimumDimensionExtractor();
		virtual void handle( const char*, const char* );
		int getResult();
};

#endif /*MINIMUMDIMENSIONEXTRACTOR_H_*/
