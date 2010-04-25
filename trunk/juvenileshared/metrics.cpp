#include "metrics.h"
#include "cvutils.h"
#include "GaborFilter.h"
#include "juvenileutils.h"

#include <iostream>

using namespace std;

MetricsExtractor::MetricsExtractor(int orientations, int scales, int lower, int upper ) {
	this->orientations = orientations;
	this->scales = scales;
	this->lower = lower;
	this->upper = upper;
	this->init();
}

void MetricsExtractor::init() {
	stuff = 0;
	nonStuff = 0;
	tp = 0;
	fp = 0;
	tn = 0;
	fn = 0;
}

void MetricsExtractor::handle( const char* fileName, const char* maskName){	
	Image * img = new Image( fileName, 256 );
	Image * msk = new Image( maskName, 256 );
	Image * tex;	
	
	string strName = fileName; 
	string pureName = strName.substr( 0, strName.size() - 4);
    string colorResultName = pureName + "_color.bmp";
    string gaborResultName = getGaborResultFileName(pureName, orientations, scales);
	img->toGray();	
	
	if ( !fileExists(gaborResultName)) { 
		GaborFilter *gf = new GaborFilter( orientations, scales );
		tex = gf->apply(img);
		tex->save(gaborResultName.data());
		delete gf;
	} else {		
		tex = new Image( gaborResultName.data(), 256 );
		tex->toGray();		
	}
		
	tex->binarize(lower, upper);		
    
    Image* color = new Image( colorResultName.data(), 256 );
    color->toGray(); 
    Image* res = binaryAnd( tex, color );
   
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
		tex->show("texture");
		color->show("color");
		cvWaitKey(0);
		res->hide("Result");
		res->hide("Mask");
		prove->hide("Prove");
		tex->hide("texture");
		color->hide("color");
	#endif
	
	//fprintf( metrix, "%d\t%d\t%d\t%d\t%d\t%d\n", localStuff,localnonStuff,localTp, localFp, localTn, localFn );
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
	delete tex;
	delete color;	
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

void MetricsExtractor::close( double &fmeasure ) {
	if ( tp + fp == 0 ) {
		fmeasure = 0;
	} else {
		double dr = (double)this->tp/(double)this->stuff;
		double p  = (double)this->tp/((double)this->tp+(double)this->fp);
	//	cout << tp << "\t" << fp << "\t" << stuff << endl;
	//	cout << dr << "\t" << p << endl;
		fmeasure = 2*dr*p/(dr+p);
	} 
}
