/***************************************************************************
 *   Copyright (C) 2007-2011 by Fernando Cardoso                           *
 *   fernandohbc@gmail.com / Twitter: @fernando_hbc                        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "juvenileutils.h"
#include "dirit.h"
#include "metrics.h"

#include <iostream>

void setInFile( std::string resultFileName, double fmeasure ) {
	FILE* resultFile = fopen( resultFileName.data(), "w" );
	fprintf( resultFile, "%f", fmeasure );
	fclose(resultFile);
}

/*
 * Gets the FMeasure result from the file with given file name.
 * If the read operation fails, throws an error message.
 */
double getFromFile( std::string resultFileName ) {
	FILE* resultFile = fopen( resultFileName.data(), "r" );
	float fmeasure;
	int scanRes = fscanf(resultFile, "%f", &fmeasure);
	if ( scanRes == EOF ) {
		throw "Could not read from " + resultFileName;
	}
	fclose(resultFile);
	return fmeasure;
}

void testRel( char* trainingDirectory, int o, int s, int l, int u, double &fmeasure ) {	
	if ( u <= l ) {		
		fmeasure = 0;
		//std::cout << "Result: " << fmeasure << std::endl;
		return;
	}	
	
	std::string resultFileName = getResultFileName(o, s, l, u);
	if ( !fileExists(resultFileName) ) {
		std::cout << "Testing combination: " << o << "\t" << s << "\t" << l << "\t" << u << std::endl;	
		DirectoryIterator *di = new DirectoryIterator(trainingDirectory);
		di->addSupportedImgExtension(".jpg");
		di->addSupportedMskExtension(".pgm");
		MetricsExtractor *me = new MetricsExtractor( o, s, l, u );
		
		di->iterate(me);
		me->close(fmeasure);	
		
		delete di;
		delete me;
		setInFile( resultFileName, fmeasure );
		std::cout << "Found: " << fmeasure << std::endl;
	} else {
		fmeasure = getFromFile(resultFileName);
	}
}

std::string getResultFileName( int o, int s, int l, int u ) {
	std::string result = TXT_FILE_PREFIX;
	char buff[10];
	sprintf(buff,"%d",o);
	result = result + TXT_FILE_SEP + buff;
	
	sprintf(buff,"%d",s);
	result = result + TXT_FILE_SEP + buff;

	sprintf(buff,"%d",l);
	result = result + TXT_FILE_SEP + buff;
	
	sprintf(buff,"%d",u);
	result = result + TXT_FILE_SEP + buff;
	
	result = result + TXT_FILE_EXT;
	
	return result;
}

std::string getGaborResultFileName( std::string pureName, int o, int s) {
	std::string result = pureName + TXT_FILE_SEP + TXT_FILE_PREFIX;
	char buff[10];
	sprintf(buff,"%d",o);
	result = result + TXT_FILE_SEP + buff;
	
	sprintf(buff,"%d",s);
	result = result + TXT_FILE_SEP + buff;
	
	result = result + TXT_IMG_EXT;
	
	return result;
}
