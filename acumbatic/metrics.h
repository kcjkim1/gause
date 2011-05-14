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
 
#include "handle.h"
#include "Configuration.h"
#include "neural.h"
#include "img.h"
#include "acumbaticshared.h"

#include <iostream>

#ifndef METRICS_H_
#define METRICS_H_

class MetricsExtractor : public CallbackHandler {
	public:
		MetricsExtractor( NeuralNetwork*);
		virtual void handle( const char*, const char* );
		void init();
		void close();
	private:
		#ifdef METRIX_DEBUG
			Image* prove;
		#endif
	    FILE* metrix;
		long stuff, nonStuff, tp, fp, tn, fn;
		NeuralNetwork* nn;
		void setNeuralNetwork(NeuralNetwork*);
		long getStuff(Image*);
		long getNonStuff(Image*);
		long getTp(Image*, Image*);
		long getFp(Image*, Image*);
		long getTn(Image*, Image*);
		long getFn(Image*, Image*);
};

#endif /*METRICS_H_*/