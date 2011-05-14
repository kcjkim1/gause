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

#include "ArffTrainingSet.h"
#include <fstream>
#include <iostream>
#include <string>
#include "acumbaticshared.h"

ArffTrainingSet::ArffTrainingSet(char *arffFile) {
    readFromArff(arffFile);
    //initFannFiles();
}

ArffTrainingSet::~ArffTrainingSet() {
}

fann_train_data * trainingData;
long qtdValidationSamples;
fann_train_data * validationData;

void ArffTrainingSet::readFromArff(char *arffFile) {

    std::string line;
    int num_input = 0;
    int num_output = 2;
    long result = 0;
    bool flagData = false;

    std::ifstream arff;

    arff.open(arffFile);
    if (arff.is_open()) {
        while (arff.good()) {
            getline(arff, line);
            if (!flagData) {
                //Test @attribute
                if (line.substr(0, 2) == "@a") {
                    num_input++;
                    continue;
                } else
                //Teste se encontra o @data
                if (line.substr(0, 2) == "@d") {
                    flagData = true;
                }
            } else {
                result++;
            }
        }

    }

    arff.close();

    //Para tirar o atributo da classe(P/N) colocado indevidamente
    num_input--;

    //Criando arquivo .dat
    std::ofstream datFile;
    datFile.open(TRAIN_FILE);

    //colocando cabealho --->  Nœmero de amostras - numero de inputs - numero de outputs
    datFile << result << " " << num_input << " " << num_output << std::endl;

    std::string out;
    flagData = false;
    std::string separador = ",";
        arff.open(arffFile);
        if (arff.is_open()) {
            while (arff.good()) {
                getline(arff, line);
                if (!flagData) {

                    //Teste se encontra o @data
                    if (line.substr(0, 2) == "@d") {
                        flagData = true;
                    }
                } else {

                    for (int j = 0; j < (line.size() - 1); j++) {

                        if (  line.substr(j, 1).compare(separador)  == 0 ) {
                            line.replace(j,1, " ");
                        }
                    }
                    out = line.substr(line.size() - 1);
                    line.replace(line.size() - 1, 1, " ");
                    datFile << line << std::endl;
                    if (out == "P") {
                        datFile << "1.000000" << " -1.000000" << std::endl;
                    } else {
                        datFile << "-1.000000" << " 1.000000" << std::endl;
                    }

                }
            }

        }

        arff.close();

    //fim for
    datFile.close();

    this->trainingSet = fann_read_train_from_file(TRAIN_FILE);
    this->validationSet = fann_read_train_from_file(VALIDATION_FILE);
}

fann_train_data * ArffTrainingSet::getTrainingData() {
    return this->trainingSet;
}

long ArffTrainingSet::getQtdValidationSamples() {
    // TODO: Retornar a quantidade de pontos presentes em validation.dat
    return NULL;
}

fann_train_data * ArffTrainingSet::getValidationData() {
    return this->validationSet;
}
