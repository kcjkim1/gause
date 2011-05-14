/*
 *  GAuSe - An Automatic Image Segmenter Generator
 *  Copyright (C) 2011 Fernando Cardoso
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "extractor.h"
#include "img.h"
#include "acumbaticshared.h"

/*
 * kfold: specifies how many crossvalidation will be performed
 */
FeatureExtractor::FeatureExtractor( SerialConfiguration* config, int hor ) {
	this->config = config;
	this->hor = hor;
	this->start_ptr = NULL;
	this->size = 0;
}

FeatureExtractor::~FeatureExtractor() {
	node * ptr  = start_ptr;
	node * nxt  = NULL;
	while ( ptr ) {
		nxt = ptr->nxt;
		delete ptr->pattern;
		delete ptr;				
		ptr = nxt;
	}
	
	remove(TEMP_TRAIN_FILE);
	remove(TEMP_VALIDATION_FILE);
	
}

void FeatureExtractor::handle( const char* fileName, const char* maskName ) {
	cout << "Sampling file " << fileName << endl;
	Image *img = new Image( fileName, this->config->getNumberOfLevels() );
	img->smooth( config->getWindowSize() );
	Image *msk = new Image( maskName, 256 );
	
	//Performs the conversion:
	img->convert( this->config->getColorSpace() );
	
	for(int i=0;i<img->getHeight();i++) {
		for(int j=0;j<img->getWidth();j++) {
			int m = DONT_CARE;
			m = msk->getRedChannel(i,j);
			if ( m != IS_NON_STUFF && m != IS_STUFF) {
				continue;
			}
			
			double ch1 = img->getNomalizedChannel(1, i,j);
			double ch2 = img->getNomalizedChannel(2, i,j);
			double ch3 = img->getNomalizedChannel(3, i,j);
			
			//Normalizes to the desired input range
			normalizeInputs( &ch1, &ch2, &ch3 );
			TrainPattern * pattern = new TrainPattern(ch1, ch2, ch3);

			if ( this->config->usesHorizontal() ) {
				double h = img->getNormalizedHorizontal(j);
				normalizeInput( &h );
				pattern->setH(h);
			}
			if ( this->config->usesVertical() ) {
				double v = img->getNormalizedVertical(i);
				normalizeInput( &v );
				pattern->setV(v);
			}
			this->dealWith( pattern, m == IS_STUFF );
		}
	}
	
	delete img;
	delete msk;	
}

void FeatureExtractor::init() {
	tempTrainingFile = fopen( TEMP_TRAIN_FILE, "w" );
	tempValidationFile = fopen( TEMP_VALIDATION_FILE, "w" );
    this->qtdTrainingSamples = 0;
    this->qtdValidationSamples = 0;
}

long FeatureExtractor::createValidationData() {
	node * ptr  = start_ptr;
	long result = 0;
	int rest = 0;
	while ( ptr ) {
		int times = (rest + ptr->pattern->getPositive()) / (this->hor+1);
		rest = (rest + ptr->pattern->getPositive()) % (this->hor+1);
		for ( int i = 0; i < times; i++ ) {
			double ch1 = ptr->pattern->getCh1();
			double ch2 = ptr->pattern->getCh2();
			double ch3 = ptr->pattern->getCh3();
			
			fprintf(tempValidationFile, "%f %f %f", ch1, ch2, ch3);
		
			if ( this->config->usesHorizontal() ) {
				double h = ptr->pattern->geth();
				fprintf(tempValidationFile, " %f", h);
			}
			if ( this->config->usesVertical() ) {
				double v = ptr->pattern->getv();
				fprintf(tempValidationFile, " %f", v);
			}
			fprintf(tempValidationFile, "\n");
			fprintf(tempValidationFile, "%f %f\n", STUFF_RESULT, NON_STUFF_RESULT);
		}
		result += times;
		
		times = (rest + ptr->pattern->getNegative()) / (this->hor+1);
		rest = (rest + ptr->pattern->getNegative()) % (this->hor+1);
		for ( int i = 0; i < times; i++ ) {
			double ch1 = ptr->pattern->getCh1();
			double ch2 = ptr->pattern->getCh2();
			double ch3 = ptr->pattern->getCh3();
			
			fprintf(tempValidationFile, "%f %f %f", ch1, ch2, ch3);
		
			if ( this->config->usesHorizontal() ) {
				double h = ptr->pattern->geth();
				fprintf(tempValidationFile, " %f", h);
			}
			if ( this->config->usesVertical() ) {
				double v = ptr->pattern->getv();
				fprintf(tempValidationFile, " %f", v);
			}
			fprintf(tempValidationFile, "\n");
			fprintf(tempValidationFile, "%f %f\n", NON_STUFF_RESULT, STUFF_RESULT);
		}
		result += times;
		
		ptr = ptr->nxt;
	}
	cout << "Collected " << result << " samples for validation" << endl;
	return result;
}

void FeatureExtractor::preconsolidate() {
	//Initially, before filtering, creates validation data
	this->qtdValidationSamples =  this->createValidationData();
	
	//The first pass is to count the stuff and nonstuff pixels
	long noStuffPx    = 0;
	long noNonStuffPx = 0;
	long evenPx = 0;
	node * ptr  = start_ptr;
	while ( ptr ) {
		int score = ptr->pattern->getScore();
		if ( score > 0 ) {
			noStuffPx+=score;
		} else if ( score < 0 ) {
			noNonStuffPx-=score;
		} else {
			evenPx++;
		}
		ptr = ptr->nxt;
	}
	
	cout << "Collected " << this->size << " unique points" << endl;
	cout << "Collected " << noStuffPx << " stuff points (with repetitions)" << endl;
	cout << "Collected " << noNonStuffPx << " nonstuff points  (with repetitions)" << endl;
	cout << "Collected " << evenPx << " even points" << endl;
	
	long stuffToPick    = (noStuffPx<noNonStuffPx)?noStuffPx:noNonStuffPx;
	stuffToPick = (stuffToPick/(this->hor+1))*this->hor;
	
	long nonStuffToPick = stuffToPick;
		
	double stuffStep = 1.0 * noStuffPx / stuffToPick;
	double nonStuffStep = 1.0 * noNonStuffPx / nonStuffToPick;
	
	cout << "Will collect " << stuffToPick << " stuff points (including repeated). The step is " << stuffStep << ". Check " << (stuffStep * stuffToPick) << endl;
	cout << "Will collect " << nonStuffToPick << " nonstuff points (including repeated). The step is " << nonStuffStep << ". Check " << (nonStuffStep * nonStuffToPick) << endl;
	
	this->qtdTrainingSamples = stuffToPick + nonStuffToPick;
	ptr  = start_ptr;
	long stuffHop    = 1;
	long nonStuffHop = 1;
	long stuffPicked = 0;
	long nonStuffPicked = 0;
	while ( ptr ) {
		long hop = 0;
		double control = 0;
		double diff = 0;
		int score = DONT_CARE;
		score = ptr->pattern->getScore();
				
		long *mToPick, *mHop, *mPicked, *mNum;
		if ( score != 0 ) {
			double  *mStep;
			if ( score < 0 ) {
				mToPick = &nonStuffToPick;
				mStep = &nonStuffStep;
				mHop = &nonStuffHop;
				mPicked = &nonStuffPicked;
				mNum = &noNonStuffPx;
			} else if ( score > 0 ) {
				mToPick = &stuffToPick;
				mStep = &stuffStep;
				mHop = &stuffHop;
				mPicked = &stuffPicked;
				mNum = &noStuffPx;
			}
			int limit = score>0?score:-score;			
			for (int i = 0; i < limit; i++) {				
				hop = ((*mHop)++);
				control = (*mStep)*((*mPicked)+1);
				diff = control - hop;
				//Se for o ultimo hop, e ainda houver mToPick, força a amostragem.
				if ((diff <= EPSILON && (*mToPick) > 0) ||
					(hop == (*mNum) && (*mToPick) == 1 )) {
					double ch1 = ptr->pattern->getCh1();
					double ch2 = ptr->pattern->getCh2();
					double ch3 = ptr->pattern->getCh3();
					
					fprintf(tempTrainingFile, "%f %f %f", ch1, ch2, ch3);
		
	    			if ( this->config->usesHorizontal() ) {
						double h = ptr->pattern->geth();
						fprintf(tempTrainingFile, " %f", h);
					}
					if ( this->config->usesVertical() ) {
						double v = ptr->pattern->getv();
						fprintf(tempTrainingFile, " %f", v);
					}
					fprintf(tempTrainingFile, "\n");
					fprintf(tempTrainingFile, "%f %f\n", ((score>0)?STUFF_RESULT:NON_STUFF_RESULT), ((score>0)?NON_STUFF_RESULT:STUFF_RESULT));
	     			(*mToPick)--;
					(*mPicked)++;
				}
				if (diff > EPSILON && hop == (*mNum)) {
					cout << "The last sample was forced." << endl;
				}
			}
		}
		ptr = ptr->nxt;
	}
	
	cout << "Collected " << stuffPicked << " stuff points (including repeated)." << endl;
	cout << "Collected " << nonStuffPicked << " nonstuff points (including repeated)." << endl;
}

void FeatureExtractor::consolidate() {
	this->preconsolidate();	
	putHeader(tempTrainingFile, TEMP_TRAIN_FILE, TRAIN_FILE, qtdTrainingSamples);
	putHeader(tempValidationFile, TEMP_VALIDATION_FILE, VALIDATION_FILE, qtdValidationSamples);
}

void FeatureExtractor::putHeader(FILE *tempFile, const char * tempFileName, const char * fileName, long samples) {
	int dim = config->getDimension();
	fclose(tempFile);
	tempFile = fopen( tempFileName, "r" );
    
    FILE* file;
	file = fopen( fileName, "w" );
    
	fprintf( file, "%ld %d 2\n", samples, dim );
	for ( long i = 0; i < samples; i++ ) {
		int readResult = 0;
    	float input[dim];
		float desired_output[2];
		for ( int j = 0; j < dim; j++ ) {
			readResult = fscanf(tempFile, "%f", &input[j]);
			if ( readResult != 1 ) {
				cout << "Read " << readResult << " items in file " << tempFileName << endl;
				throw "Erro na leitura das entradas durante a consolidação de arquivo";
			}
		}
		
		readResult = fscanf(tempFile, "%f", &desired_output[0]);
		if ( readResult != 1 ) {
			cout << "Read " << readResult << " items in file " << tempFileName << endl;
			throw "Erro na leitura da primeira saida durante a consolidação de arquivo";
		}
    	readResult = fscanf(tempFile, "%f", &desired_output[1]);
    	if ( readResult != 1 ) {
    		cout << "Read " << readResult << " items in file " << tempFileName << endl;
			throw "Erro na leitura da segunda saida durante a consolidação de arquivo";
		}
		
		fprintf( file, "%f", input[0] );
    	for ( int j = 1; j < dim; j++ ) {
			fprintf( file, " %f", input[j]);
		}
	
    	fprintf( file, "\n%f %f\n", desired_output[0], desired_output[1]  );
	}
	
	fclose( tempFile );
    fclose( file );
}

void FeatureExtractor::dealWith(TrainPattern * pattern, bool isStuff) {
	//cout << endl<< "Dealing with pattern: " << pattern->getText(this->config->usesHorizontal(), this->config->usesVertical()) << "." << endl;
	node * ptr  = start_ptr;
	node * prev = NULL;
	while ( ptr ) {
		TrainPattern * curr = ptr->pattern;
		//cout << "Current Pattern: " << curr->getText(this->config->usesHorizontal(), this->config->usesVertical()) << "." << endl;
		int comparison = curr->compare(pattern, this->config->usesHorizontal(), this->config->usesVertical());
		if ( comparison == 0 ) {
			//cout << "Found !" << endl;			
			if ( isStuff ) {								
				curr->inc();
			} else {
				curr->dec();
			}
			//cout << "Current score is: " << curr->getScore() << endl;
			delete pattern;
			return;
		}
		if ( comparison < 0 ) {
			//cout << "Less !" << endl;		
			prev = ptr;
			ptr = ptr->nxt;
		} else {
			//cout << "More ! Will insert before !" << endl;
			break;
		}		
	}
		
	if ( isStuff ) {								
		pattern->inc();
	} else {
		pattern->dec();
	}
	
	//Não existe
	node * newnode = new node;
	newnode->pattern = pattern;
	newnode->nxt = ptr;
	if ( prev ) {
		prev->nxt = newnode;
	} else {
		start_ptr = newnode;
	}
	this->size++;
}

long FeatureExtractor::getQtdValidationSamples() {
	return this->qtdValidationSamples;
}
