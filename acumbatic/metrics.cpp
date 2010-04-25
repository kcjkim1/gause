#include "metrics.h"
#include "cvutils.h"

#include <iostream>

using namespace std;

MetricsExtractor::MetricsExtractor(NeuralNetwork* nn) {
	setNeuralNetwork( nn );
}

void MetricsExtractor::setNeuralNetwork(NeuralNetwork* nn) {
	this->nn = nn;
}

void MetricsExtractor::init() {
	stuff = 0;
	nonStuff = 0;
	tp = 0;
	fp = 0;
	tn = 0;
	fn = 0;
	
	string fileName = getMetrixFileName( nn->getConfiguration()->getColorSpace(), nn->getConfiguration()->getNumberOfLevels(), nn->getConfiguration()->getWindowSize(), nn->getConfiguration()->getPositionDescription(), nn->getHiddenNeurons() );
	
	metrix = fopen( fileName.data() , "w" );
	fprintf( metrix, "stuff\tnon\ttp\tfp\ttn\tfn\n" );
}

void MetricsExtractor::handle( const char* fileName, const char* maskName){
	Image *img = new Image( fileName, this->nn->getConfiguration()->getNumberOfLevels() );
	Image *msk = new Image( maskName, this->nn->getConfiguration()->getNumberOfLevels() );
	Image *res = nn->apply( img );
	
	#ifdef METRIX_DEBUG
	 	prove = new Image(img->getWidth(), img->getHeight());
	#endif
	
	#ifdef METRIX_DEBUG
		cout << "Created Objects" << endl;
	#endif
	
	long localStuff = getStuff(msk);
	#ifdef METRIX_DEBUG
		cout << "Extracted Stuff " << localStuff << endl;
	#endif
	long localnonStuff = getNonStuff(msk);
	#ifdef METRIX_DEBUG
		cout << "Extracted NonStuff " << localnonStuff << endl;
	#endif
	long localTp = getTp(res, msk);
	#ifdef METRIX_DEBUG
		cout << "Tp " << localTp << endl;
	#endif
	long localFp = getFp(res, msk);
	#ifdef METRIX_DEBUG
		cout << "Fp " << localFp << endl;
	#endif
	long localTn = getTn(res, msk);
	#ifdef METRIX_DEBUG
		cout << "Tn " << localTn << endl;
	#endif
	long localFn = getFn(res, msk);
	#ifdef METRIX_DEBUG
		cout << "Fn " << localFn << endl;
	#endif
	
	#ifdef METRIX_DEBUG
		cout << "Extracted" << endl;
	#endif
	
	#ifdef METRIX_DEBUG
		res->show("Result");
		msk->show("Mask");
		prove->show("Prove");
		cvWaitKey(0);
		res->hide("Result");
		res->hide("Mask");
		prove->show("Prove");
	#endif
	
	fprintf( metrix, "%ld\t%ld\t%ld\t%ld\t%ld\t%ld\n", localStuff,localnonStuff,localTp, localFp, localTn, localFn );
	
	stuff += localStuff;
	nonStuff += localnonStuff;
	tp += localTp;
	fp += localFp;
	tn += localTn;
	fn += localFn;
	
	#ifdef METRIX_DEBUG
		delete prove;
	#endif 
	delete img;
	delete msk;
	delete res;
}

long MetricsExtractor::getStuff(Image* msk) {
	long result = 0;
	for(int i=0;i<msk->getHeight();i++) {
		for(int j=0;j<msk->getWidth();j++) {
			if ( msk->getRedChannel(i, j) == IS_STUFF ) {
				result++;
			}
		}
	}
	return result;
}

long MetricsExtractor::getNonStuff(Image* msk) {
	long result = 0;
	for(int i=0;i<msk->getHeight();i++) {
		for(int j=0;j<msk->getWidth();j++) {
			if ( msk->getRedChannel(i, j) == IS_NON_STUFF ) {
				result++;
			}
		}
	}
	return result;
}

long MetricsExtractor::getTp(Image* res, Image* msk) {
	long result = 0;
	for(int i=0;i<msk->getHeight();i++) {
		for(int j=0;j<msk->getWidth();j++) {
			if ( msk->getRedChannel(i, j) == IS_STUFF && res->getRedChannel(i,j) == IS_STUFF ) {
				result++;
				#ifdef METRIX_DEBUG
	 				prove->setPixel(i,j,0,255,0);
				#endif
			}
		}
	}
	return result;
}

long MetricsExtractor::getFp(Image* res, Image* msk) {
	long result = 0;
	for(int i=0;i<msk->getHeight();i++) {
		for(int j=0;j<msk->getWidth();j++) {
			if ( msk->getRedChannel(i, j) == IS_NON_STUFF && res->getRedChannel(i,j) == IS_STUFF ) {
				result++;
				#ifdef METRIX_DEBUG
	 				prove->setPixel(i,j,255,0,0);
				#endif
			}
		}
	}
	return result;
}

long MetricsExtractor::getTn(Image* res, Image* msk) {
	long result = 0;
	for(int i=0;i<msk->getHeight();i++) {
		for(int j=0;j<msk->getWidth();j++) {
			if ( msk->getRedChannel(i, j) == IS_NON_STUFF && res->getRedChannel(i,j) == IS_NON_STUFF ) {
				result++;
				#ifdef METRIX_DEBUG
	 				prove->setPixel(i,j,0,255,255);
				#endif
			}
		}
	}
	return result;
}

long MetricsExtractor::getFn(Image* res, Image* msk) {
	long result = 0;
	for(int i=0;i<msk->getHeight();i++) {
		for(int j=0;j<msk->getWidth();j++) {
			if ( msk->getRedChannel(i, j) == IS_STUFF && res->getRedChannel(i,j) == IS_NON_STUFF ) {
				result++;
				#ifdef METRIX_DEBUG
	 				prove->setPixel(i,j,128,128,128);
				#endif
			}
		}
	}
	return result;
}

void MetricsExtractor::close() {
	double dr = ((double) tp )/((double) stuff );
	cout << "Test Directory Detection Rate: " << dr << endl;
	fclose(metrix);
}
