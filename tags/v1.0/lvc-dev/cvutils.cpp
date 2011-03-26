#include "cvutils.h"

#include <sys/stat.h>

using namespace std;

int getConvertion( int newColorSpace ) {
	switch (newColorSpace) {
		case HSV: return CV_BGR2HSV; break;
		case YCBCR: return CV_BGR2YCrCb; break;
		case XYZ: return CV_BGR2XYZ; break;
		case HLS: return CV_BGR2HLS; break;
		case LAB: return CV_BGR2Lab; break;
		case LUV: return CV_BGR2Luv; break;
		case GL : return CV_BGR2GRAY; break;
		default : return -1;
	}
}

bool fileExists(string strFilename) {
  struct stat stFileInfo;
  bool blnReturn;
  int intStat;

  // Attempt to get the file attributes
  intStat = stat(strFilename.c_str(),&stFileInfo);
  if(intStat == 0) {
    // We were able to get the file attributes
    // so the file obviously exists.
    blnReturn = true;
  } else {
    // We were not able to get the file attributes.
    // This may mean that we don't have permission to
    // access the folder which contains this file. If you
    // need to do that level of checking, lookup the
    // return values of stat which will give you
    // more details on why stat failed.
    blnReturn = false;
  }
  
  return(blnReturn);
}

Image* binaryAnd( Image* a, Image* b ) {
	Image* res = new Image( a->getWidth(), a->getHeight() );
	res->toGray();
	
		
	for(int i=0;i<res->getHeight();i++) {
		for(int j=0;j<res->getWidth();j++) {
			if ( a->getGray(i,j) == 255 && b->getGray(i,j) == 255 ) {
				res->setGray(i,j,255);
			} else {
				res->setGray(i,j,0);
			}
		}
	}
	
	return res;
}
