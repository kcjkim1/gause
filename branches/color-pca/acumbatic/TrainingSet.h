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
#ifndef TRAININGSET_H_
#define TRAININGSET_H_

#include <fann.h>
#include <floatfann.h>

/**
 * Abstract class for the training sets that uses fann data.
 * This separates training set abstractions from detailed implementations,
 * such as ArrfTrainingSet and DirectoryTraningSet.
 */
class TrainingSet {
public:
    TrainingSet();
    virtual ~TrainingSet();

    virtual fann_train_data *getTrainingData();
    virtual long getQtdValidationSamples();
    virtual fann_train_data *getValidationData();
};

#endif /* TRAININGSET_H_ */
