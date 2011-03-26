#ifndef JUVENILEUTILS_H_
#define JUVENILEUTILS_H_

#include <string>

#define TXT_FILE_PREFIX "txt"
#define TXT_FILE_SEP "_"
#define TXT_FILE_EXT ".txt"
#define TXT_IMG_EXT ".pgm"

void testRel( char* , int , int , int , int , double & );
std::string getResultFileName( int, int, int, int );
std::string getGaborResultFileName( std::string, int, int);

#endif /*JUVENILEUTILS_H_*/
