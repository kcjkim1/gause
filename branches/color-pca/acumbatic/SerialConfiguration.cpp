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
 
#include "acumbaticshared.h"
#include "cvutils.h"
#include "SerialConfiguration.h"

SerialConfiguration::SerialConfiguration(int colorSpace, int numberOfLevels,
        int windowSize, bool horizontalFlag, bool verticalFlag) {
    setColorSpace(colorSpace);
    setNumberOfLevels(numberOfLevels);
    setWindowSize(windowSize);
    this->horizontalFlag = horizontalFlag;
    this->verticalFlag = verticalFlag;
}

SerialConfiguration::SerialConfiguration(string fileName) {
    int readResult = 0;
    FILE* file = fopen(fileName.data(), "r");
    int colorSpace;
    readResult += fscanf(file, "%d", &colorSpace);
    setColorSpace(colorSpace);
    int numberOfLevels;
    readResult += fscanf(file, "%d", &numberOfLevels);
    setNumberOfLevels(numberOfLevels);
    int windowSize;
    readResult += fscanf(file, "%d", &windowSize);
    setWindowSize(windowSize);
    int flag;
    readResult += fscanf(file, "%d", &flag);
    horizontalFlag = (flag == 1);
    readResult += fscanf(file, "%d", &flag);
    verticalFlag = (flag == 1);
    fclose(file);
    if (readResult != 5) {
        throw "Invalid SerialConfiguration file!";
    }
}

int SerialConfiguration::getWindowSize() {
    return windowSize;
}

int SerialConfiguration::getColorSpace() {
    return this->colorSpace;
}

int SerialConfiguration::getNumberOfLevels() {
    return this->numberOfLevels;
}

void SerialConfiguration::setColorSpace(int colorSpace) {
    this->colorSpace = colorSpace;
}

void SerialConfiguration::setNumberOfLevels(int numberOfLevels) {
    this->numberOfLevels = numberOfLevels;
}

void SerialConfiguration::setWindowSize(int windowSize) {
    this->windowSize = windowSize;
}

void SerialConfiguration::save() {
    string fileName = CONFIG_FILE_NAME;
    FILE* cfg = fopen(CONFIG_FILE_NAME, "w");
    fprintf(cfg, "%d\n%d\n%d\n%d\n%d\n", colorSpace, numberOfLevels,
            windowSize, (horizontalFlag ? 1 : 0), (verticalFlag ? 1 : 0));
    fclose(cfg);
}

string SerialConfiguration::getPositionDescription() {
    if (horizontalFlag && verticalFlag) {
        return "hor_vert";
    }

    if (horizontalFlag) {
        return "hor";
    }

    if (verticalFlag) {
        return "vert";
    }

    return "no_pos";
}

int SerialConfiguration::getDimension() {
    if (horizontalFlag && verticalFlag) {
        return 5;
    }

    if (horizontalFlag || verticalFlag) {
        return 4;
    }

    return 3;
}

bool SerialConfiguration::usesHorizontal() {
    return horizontalFlag;
}

bool SerialConfiguration::usesVertical() {
    return verticalFlag;
}

bool SerialConfiguration::isCompatible(SerialConfiguration * other) {
    return (this->colorSpace) == (other->getColorSpace())
            && (this->numberOfLevels) == (other->getNumberOfLevels())
            && (this->windowSize) == (other->getWindowSize())
            && (this->horizontalFlag) == (other->usesHorizontal())
            && (this->verticalFlag) == (other->usesVertical());
}

void SerialConfiguration::preprocess(Image * img) {
    img->smooth(this->getWindowSize());
    img->convert(this->getColorSpace());
}

void SerialConfiguration::fillInput(float * input, Image * img, int i, int j) {
    double ch[3];
    ch[0] = img->getNomalizedChannel(1, i, j);
    ch[1] = img->getNomalizedChannel(2, i, j);
    ch[2] = img->getNomalizedChannel(3, i, j);
    normalizeInputs(&ch[0], &ch[1], &ch[2]);

    input[0] = (float) ch[0];
    input[1] = (float) ch[1];
    input[2] = (float) ch[2];
    if (this->usesHorizontal() && this->usesVertical()) {
        input[3] = img->getNormalizedHorizontal(j);
        input[4] = img->getNormalizedVertical(i);
    } else if (this->usesHorizontal()) {
        input[3] = img->getNormalizedHorizontal(j);
    } else if (this->usesVertical()) {
        input[3] = img->getNormalizedVertical(i);
    }
}

std::string SerialConfiguration::getColorSpaceAndPositionDescription() {
    std::string description = getColorSpaceString(this->getColorSpace());
    description += "_" + this->getPositionDescription();
    return description;
}
