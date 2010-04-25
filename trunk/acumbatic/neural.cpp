#include "neural.h"
#include "acumbaticshared.h"
#include "dirit.h"
#include "metrics.h"


#include <iostream>

using namespace std;

NeuralNetwork::NeuralNetwork( int hiddenNeurons, Configuration* config ){
	setHiddenNeurons( hiddenNeurons );
	setConfiguration( config );
}

NeuralNetwork::NeuralNetwork( string fileName, string thFile, Configuration* config ){
	this->nn = fann_create_from_file( fileName.data() );	
	setConfiguration( config );
	
	FILE* thf = fopen( thFile.data(), "r" );
	float t;
	int readResult = fscanf( thf, "%f", &t );
	if ( readResult != 1 ) {
		throw "Unable to read threshold information from configuration file.";
	}
	this->th = t;
	fclose( thf );
}

NeuralNetwork::~NeuralNetwork(){
	fann_destroy( this->nn );
}

void NeuralNetwork::train( TrainingSet* ts, int trainingIterations, int maxEpochs, int epochsBetweenReports, double thresholdStep ) {
	double bestThreshold = -10;
	double bestFMeasure = -10;
	initResults();
	this->ts = ts;
	fann_train_data *trainingData = this->ts->getTrainingData();
	fann_train_data *validationData = this->ts->getValidationData();
	for ( int i = 0; i < trainingIterations; i++ ) {
		cout << "Training Neural Network for "<< getColorSpaceString( this->configuration->getColorSpace() ) << " with " << this->configuration->getNumberOfLevels();
		cout << " levels of quantization and window size " << this->configuration->getWindowSize() << ". Hidden Neurons: " << this->hiddenNeurons << ". Iteration " << i << endl;	
		
		struct fann *ann = trainAnn( this->hiddenNeurons, trainingData, validationData, maxEpochs, epochsBetweenReports );
		double detectionRate = 0;
		double precision = 0;
		#ifdef NN_DEBUG 
			cout << "Entering threshold learning" << endl;
		#endif
		double threshold = this->learnThreshold( ann, thresholdStep, &detectionRate, &precision );
		double fMeasure = getFMeasure( detectionRate, precision );
		saveResults( i, detectionRate, precision, fMeasure );
		cout << "The threshold is: " << threshold << " ... The rates are: " << detectionRate << "\t" << precision << "\t" << fMeasure << endl;
		if ( fMeasure > bestFMeasure ) {
			bestFMeasure = fMeasure;
			bestThreshold = threshold;
			fann_save(ann, NETWORK_FILE_NAME); 
			
			this->saveMetrix( detectionRate, precision, fMeasure );
			
			FILE* thFile = fopen( THRESHOLD_FILE_NAME, "w" );
			fprintf(thFile, "%f\n", bestThreshold );
			fclose( thFile );
		}		
		fann_destroy(ann);
	}
	closeResults();
	this->nn = fann_create_from_file( NETWORK_FILE_NAME );
	this->th = bestThreshold;
	this->ts = NULL;
}

void NeuralNetwork::saveMetrix( double detectionRate, double precision, double fMeasure ) {
	string fileName = getMetrixFileName(this->configuration->getColorSpace(), this->configuration->getNumberOfLevels(), this->configuration->getWindowSize(), this->configuration->getPositionDescription(), this->hiddenNeurons);
	
	FILE* mFile = fopen( fileName.data(), "w" );
	fprintf(mFile, "%f\n%f\n%f\n", detectionRate, precision, fMeasure );
	fclose( mFile );
}

double NeuralNetwork::learnThreshold( struct fann* ann, double thresholdStep, double* detectionRate, double* precision ) {
	double bestThreshold = -10;
	double bestFMeasure = 0;
	double bestDr = 0;
	double bestPr = 0;
	for ( double threshold = 0; threshold <= STUFF_RESULT - NON_STUFF_RESULT; threshold = threshold + thresholdStep ) {
		double precision;
		double detectionRate = testThreshold( ann, threshold, precision );
		double fMeasure = getFMeasure( detectionRate, precision );
		if ( fMeasure >= bestFMeasure ) {
			bestDr = detectionRate;
			bestPr = precision;
			bestFMeasure = fMeasure;
			bestThreshold = threshold;
		} else {
			break;
		}
	}
	(*detectionRate) = bestDr;
	(*precision) = bestPr;
	return bestThreshold;
}

double NeuralNetwork::testThreshold( struct fann* ann, double threshold, double& precision ) {
	FILE* valData = fopen( VALIDATION_FILE, "r" );
	//Ignores first line
	int dim = this->getConfiguration()->getDimension();
	int foo;
	int readResult = 0;
	for ( int i = 0; i < 3; i++ ) {
		readResult = fscanf( valData, "%d", &foo );
	}
	long stuffSamples = 0;
	long nonStuffSamples = 0;
	long tp = 0;
	long fp = 0;
	long tn = 0;
	long fn = 0;
	for ( int i = 0; i < this->ts->getQtdValidationSamples(); i++ ) {
		float input[dim];
		float desired_output[2];		
		for ( int i = 0; i < dim; i++ ) {
			readResult = fscanf( valData, "%f", &input[i] );
			if ( readResult != 1 ) {
				throw "Unable to read input from validation data";
			}
		}
    	readResult = fscanf(valData, "%f", &desired_output[0]);
    	if ( readResult != 1 ) {
    		throw "Unable to read output from validation data";
    	}
    	readResult = fscanf(valData, "%f", &desired_output[1]);
    	if ( readResult != 1 ) {
    		throw "Unable to read output from validation data";
    	}
    	
    	int result = applyAnn( ann, input, threshold );
    	
    	if ( desired_output[0] == STUFF_RESULT ) {
    		stuffSamples++;
    		if ( result == STUFF_RESULT ) {
    			tp++;
    		} else {
    			fn++;
    		}
    	} else {
    		nonStuffSamples++;
    		if ( result == STUFF_RESULT ) {
    			fp++;
    		} else {
    			tn++;
    		}
    	}
	}
	fclose(valData);
	double detectionRate = ((double)tp) / ((double)stuffSamples);
	precision = ((double)tp) / ((double)tp+(double)fp);
	return detectionRate;
}

struct fann* NeuralNetwork::trainAnn( int h, fann_train_data *trainData, fann_train_data *validationData, int maxEpochs, int epochsBetweenReports ) {
	struct fann *ann = fann_create_standard( 3, this->getConfiguration()->getDimension(), h, 2 );
	fann_set_activation_function_hidden(ann, FANN_SIGMOID_SYMMETRIC	);
    fann_set_activation_function_output(ann, FANN_SIGMOID_SYMMETRIC );
	int epoch = 0;
	double MSE = 0;
	double bestMSE = 999999;
	while ( epoch < maxEpochs ) {
	    fann_train_epoch(ann, trainData);		
		epoch++;
		fann_reset_MSE( ann );
		fann_test_data(ann, validationData);
		MSE = fann_get_MSE( ann );
		if ( MSE < bestMSE ) {
			bestMSE = MSE;
			fann_save(ann, MINIMUM_MSE_NET_FILE_NAME);
		}
		if ( epoch%epochsBetweenReports == 0 || epoch==1 || epoch == maxEpochs ) {
			cout << "Epoch: "  << epoch << "\tMSE: " << MSE << "\tBest MSE: " << bestMSE << endl;
		}
	}
	fann_destroy( ann );
	ann = fann_create_from_file( MINIMUM_MSE_NET_FILE_NAME );
	remove(MINIMUM_MSE_NET_FILE_NAME);
	return ann;
}

int NeuralNetwork::applyAnn( struct fann* ann, float* input, double threshold ) {
	fann_type *calc_out;
	
	calc_out = fann_run(ann, input);
    #ifdef NN_DEBUG
     cout << m << endl;
     cout << "Results: " << calc_out[0] << "\t" << calc_out[1] << endl;
    #endif
    
    //First tests if the pixel is classifiable
    double diff = calc_out[0] - calc_out[1];
    if (( calc_out[0] >= 0 && calc_out[1] >= 0 ) || ( calc_out[0] < 0 && calc_out[1] < 0 ) || (abs(diff) <= threshold ) ) {
    	return NON_STUFF_RESULT;
    } else {
    	if ( calc_out[0] > calc_out[1] ) {
    		return STUFF_RESULT;
    	} else {
    		return NON_STUFF_RESULT;
    	}
    }			
}

void NeuralNetwork::setHiddenNeurons(int hiddenNeurons) {
	this->hiddenNeurons = hiddenNeurons;
}

void NeuralNetwork::setConfiguration(Configuration* config) {
	this->configuration = config;
}

int NeuralNetwork::getHiddenNeurons() {
	return this->hiddenNeurons;
}	

Configuration* NeuralNetwork::getConfiguration() {
	return this->configuration;
}

Image* NeuralNetwork::apply( Image* img ) {
	int dim = this->getConfiguration()->getDimension();
	Image* res = new Image( img->getWidth(), img->getHeight() );
	img->smooth( this->getConfiguration()->getWindowSize() );
	img->convert(this->configuration->getColorSpace());
	#ifdef NN_DEBUG
	  img->showAndWait("Imagem");
	#endif
	for(int i=0;i<img->getHeight();i++) {
		for(int j=0;j<img->getWidth();j++) {
			double ch[3];
			ch[0] = img->getNomalizedChannel(1, i,j);
			ch[1] = img->getNomalizedChannel(2, i,j);
			ch[2] = img->getNomalizedChannel(3, i,j);
			normalizeInputs( &ch[0], &ch[1], &ch[2] );
			
			float input[dim];
			input[0] = (float)ch[0];
			input[1] = (float)ch[1];
			input[2] = (float)ch[2];
			if ( this->getConfiguration()->usesHorizontal() && this->getConfiguration()->usesVertical() ) {
				input[3] = img->getNormalizedHorizontal(j);
				input[4] = img->getNormalizedVertical(i);
			} else if ( this->getConfiguration()->usesHorizontal() ) {
				input[3] = img->getNormalizedHorizontal(j);
			} else if ( this->getConfiguration()->usesVertical() ) {
				input[3] = img->getNormalizedVertical(i);
			}
			
			int result = applyAnn( this->nn, input, this->th );
			
			if ( result == STUFF_RESULT ) {
				res->setGray( i, j, IS_STUFF );
			} else {
				res->setGray( i, j, IS_NON_STUFF );
			}
		}
	}
	return res;
}

void NeuralNetwork::save() {
	string fileName = getNNFileName(this->configuration->getColorSpace(), this->configuration->getNumberOfLevels(), this->configuration->getWindowSize(), this->getConfiguration()->getPositionDescription(), this->hiddenNeurons);
	string thFileName = getThFileName(this->configuration->getColorSpace(), this->configuration->getNumberOfLevels(), this->configuration->getWindowSize(), this->getConfiguration()->getPositionDescription(), this->hiddenNeurons);
	fann_save( this->nn, fileName.data() );
	
	FILE* thFile = fopen( thFileName.data(), "w" );
	fprintf(thFile, "%f\n", this->th );
	fclose( thFile );
}

void NeuralNetwork::initResults() {
	string fileName = getResultFileName(this->configuration->getColorSpace(), this->configuration->getNumberOfLevels(), this->configuration->getWindowSize(), this->getConfiguration()->getPositionDescription(), this->hiddenNeurons);
	resultFile = fopen( fileName.data(), "w" );
	fprintf( resultFile, "%s\t%s\t%s\t%s\n", "Iteration", "Dr", "P", "Fm" );
}

void NeuralNetwork::saveResults( int i, double detectionRate, double precision, double fMeasure ) {
	cout << i << "\t" << detectionRate << "\t" << precision << "\t" << fMeasure << endl;
	if ( resultFile == NULL ) {
		cout << "file null" << endl;
	}
	fprintf( resultFile, "%d\t%f\t%f\t%f\n", i, detectionRate, precision, fMeasure );
}

void NeuralNetwork::closeResults() {
	fclose( resultFile );
}