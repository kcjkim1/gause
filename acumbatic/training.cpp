#include "training.h"
#include "dirit.h"
#include "extractor.h"
#include "acumbaticshared.h"

//rndConf says how many images it will use from the imageset
//kflod specifies the number of crossvalidations to be performed
TrainingSet::TrainingSet( char *trainingDirectory, Configuration* configuration, int rndConf, int horatio, bool delFiles ) {	
	setConfiguration( configuration );
	DirectoryIterator *di = new DirectoryIterator( trainingDirectory );
	FeatureExtractor *fe = new FeatureExtractor( this->configuration, horatio );
	
	#ifdef TS_DEBUG
		cout << "Created objects" << endl;
	#endif
	
	fe->init();
	#ifdef TS_DEBUG
		cout << "Initiated Feature Extractor" << endl;
	#endif	
	di->addSupportedImgExtension(".jpg");
	di->addSupportedImgExtension(".bmp");
	di->addSupportedMskExtension(".pgm");
	if ( rndConf != 0 ) {
		di->setRandom(rndConf);
	}
	di->iterate( fe );
	#ifdef TS_DEBUG
		cout << "Iterated throug " << trainingDirectory << endl;
	#endif	
	fe->consolidate();
	this->delFiles = delFiles;
	
	trainingData = fann_read_train_from_file(TRAIN_FILE);
	this->qtdValidationSamples = fe->getQtdValidationSamples();
	validationData = fann_read_train_from_file(VALIDATION_FILE);
	
	delete fe;
	delete di;
}

TrainingSet::TrainingSet( Configuration* configuration, int horatio, bool delFiles ) {	
	setConfiguration( configuration );	
    this->delFiles = delFiles;
    
	trainingData = fann_read_train_from_file(TRAIN_FILE);
	this->qtdValidationSamples = countValidationSamples(VALIDATION_FILE);
	validationData = fann_read_train_from_file(VALIDATION_FILE);
}

TrainingSet::~TrainingSet() {
	if ( delFiles ) {
		remove(TRAIN_FILE);
		remove(VALIDATION_FILE);
	}
	fann_destroy_train( trainingData );
	fann_destroy_train( validationData );
}

fann_train_data* TrainingSet::getTrainingData( ) {
	return trainingData;
}

fann_train_data* TrainingSet::getValidationData() {	
	return validationData;
}

void TrainingSet::setConfiguration(Configuration* config) {
	this->configuration = config;
}

long TrainingSet::getQtdValidationSamples() {
	return this->qtdValidationSamples;
}

long TrainingSet::countValidationSamples(const char* fileName) {
	FILE* file = fopen(fileName, "r");
	long result;
	fscanf( file, "%ld", &result );
	fclose(file);	
	return result;
}