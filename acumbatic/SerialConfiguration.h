/*
 *  GAuSe - An Automatic Image Segmenter Generator
 *  Copyright (C) 2011 Fernando Cardoso, Jovan Andrade
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

#ifndef SERIALCONFIGURATION_H_
#define SERIALCONFIGURATION_H_

#include "Configuration.h"
#include "img.h"

class SerialConfiguration : public Configuration {
public:
    SerialConfiguration(int, int, int, bool, bool);
    SerialConfiguration( std::string);
    virtual int getWindowSize();
    virtual std::string getColorSpaceAndPositionDescription();
    virtual int getNumberOfLevels();
    virtual int getDimension();
    virtual void preprocess(Image * img);
    virtual void fillInput(float[], Image*, int, int);
    int getColorSpace();
    void save();
    std::string getPositionDescription();
    bool usesHorizontal();
    bool usesVertical();
    bool isCompatible(SerialConfiguration*);
protected:
    int windowSize;
    int colorSpace;
    int numberOfLevels;
    bool horizontalFlag;
    bool verticalFlag;
    void setColorSpace(int);
    void setNumberOfLevels(int);
    void setWindowSize(int);
};

#endif /* SERIALCONFIGURATION_H_ */
