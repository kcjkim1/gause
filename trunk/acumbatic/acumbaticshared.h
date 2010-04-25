#include <string>
 
#ifndef ACUMBATICSHARED_H_
#define ACUMBATICSHARED_H_

//#define MAIN_DEBUG
//#define TS_DEBUG
//#define DEBUG
//#define NN_DEBUG
//#define METRIX_DEBUG

#define MIN_INPUT_RANGE -1.0
#define MAX_INPUT_RANGE 1.0

#define STUFF_RESULT 1.0
#define NON_STUFF_RESULT -1.0

#define EPSILON 0.00000000001

#define TEMP_TRAIN_FILE "temptrain.tmp"

#define TEMP_VALIDATION_FILE "tempval.tmp"

#define TRAIN_FILE "train.dat"

#define VALIDATION_FILE "val.dat"

#define MINIMUM_MSE_NET_FILE_NAME "bestSoFar.net"
#define NETWORK_FILE_NAME "network.net"
#define CONFIG_FILE_NAME "network.cfg"
#define THRESHOLD_FILE_NAME "threshold.th"

#define ANN_FILE_PREFIX "ann"
#define METRIX_FILE_PREFIX "mtr"
#define TH_FILE_PREFIX "th"
#define RESULT_FILE_PREFIX "res"
#define ANN_FILE_SEP "_"
#define METRIX_FILE_SEP "_"
#define TH_FILE_SEP "_"
#define RESULT_FILE_SEP "_"

void normalizeInputs( double*, double*, double* );
void normalizeInput( double* );
double abs( double x );
std::string getColorSpaceString( int );
std::string getMetrixFileName( int , int , int , std::string, int  );
std::string getNNFileName( int , int , int , std::string , int  );
std::string getThFileName( int , int , int , std::string , int  );
std::string getResultFileName( int , int , int , std::string , int  );
std::string getFileName(std::string, std::string, int , int , int , std::string , int  );
double getFMeasure( double, double );


#endif /*ACUMBATICSHARED_H_*/
