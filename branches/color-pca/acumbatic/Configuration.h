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

#ifndef CONFIGURATION_H_
#define CONFIGURATION_H_

#include <iostream>
#include "img.h"

class Configuration {
public:
    Configuration();
    virtual ~Configuration();
    virtual std::string getColorSpaceAndPositionDescription() = 0;
    virtual int getWindowSize() = 0;
    virtual int getNumberOfLevels() = 0;
    virtual int getDimension() = 0;
    virtual void preprocess(Image *) = 0;
    virtual void fillInput(float[], Image*, int, int) = 0;
};

#endif /* CONFIGURATION_H_ */