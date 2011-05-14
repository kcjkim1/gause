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

#ifndef ARFFTRAININGSET_H_
#define ARFFTRAININGSET_H_

#include "TrainingSet.h"

class ArffTrainingSet : public TrainingSet {
public:
	ArffTrainingSet(char *);
	virtual ~ArffTrainingSet();
	virtual fann_train_data *getTrainingData();
	virtual long getQtdValidationSamples();
    virtual fann_train_data *getValidationData();
private:
	void readFromArff(char*);
	fann_train_data * trainingSet;
	fann_train_data * validationSet;
};

#endif /* ARFFTRAININGSET_H_ */
