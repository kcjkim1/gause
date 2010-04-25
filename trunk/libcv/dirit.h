#include "handle.h"

#ifndef DIRIT_H_
#define DIRIT_H_

#define IMG_EXT 6
#define MSK_EXT 4

void initGlobal();

class DirectoryIterator {
	public:
		DirectoryIterator( char* );
		void iterate( CallbackHandler* );
		void addSupportedImgExtension( const char* );
		void addSupportedMskExtension( const char* );
		void setRandom( int );
	private:
		const char* supportedImgExtensions[IMG_EXT];
		const char* supportedMskExtensions[MSK_EXT];
		char* directory;
		void setDirectory( char* );	
		int imgCount;
		int mskCount;
		int rndCount;
		int rndSet;
};

#endif /*DIRIT_H_*/
