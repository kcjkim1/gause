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
#include <queue>
#include "acumbaticshared.h"





ArffTrainingSet::ArffTrainingSet(char *arffFile,ArffConfiguration* config , int horatio ) {
	this->configuration = config;
    readFromArff(arffFile,horatio);
    //initFannFiles();
}

ArffTrainingSet::~ArffTrainingSet() {
}

fann_train_data * trainingData;
long qtdValidationSamples;
fann_train_data * validationData;

void ArffTrainingSet::setConfiguration(ArffConfiguration* config){
	this->configuration = config;
}


void ArffTrainingSet::readFromArff(char *arffFile,int holdoutRatio) {

    std::string line;
    int num_input = 0;
    int num_output = 2;
    long result = 0;
    bool flagData = false;
    queue<std::string> p;
    queue<std::string> n;

    std::ifstream arff;
std::cout << "lendo arquivo pca .arff";
    arff.open(arffFile);
    if (arff.is_open()) {
        while (arff.good()) {
            getline(arff, line);
            if (!flagData) {
                //Pegando os canais
                if (line.substr(0, 2) == "@a") {
                    num_input++;
                    this->configuration->setColorPCA(line,num_input);
                    continue;
                } else
                //Teste se encontra o @data
                if (line.substr(0, 2) == "@d") {
                    flagData = true;
                }
            } else {
                result++;
                if (line.substr(line.size() - 1) == "P") {
                					p.push(line);
                				} else {
                					n.push(line);
                				}
            }
        }

    }

    arff.close();

    //Para tirar o atributo da classe(P/N) colocado indevidamente
    num_input--;
    //Criando arquivo .dat
    std::ofstream datFile;
    std::ofstream valDatFile;
    datFile.open(TRAIN_FILE);
    valDatFile.open(VALIDATION_FILE);
    //colocando cabe√ßalho --->  N√∫mero de amostras - numero de inputs - numero de outputs
    std::string out;
    	flagData = false;
    	//int proporcao = 0;
    	queue<std::string>* menor;
    	queue<std::string>* maior;
    	if (p.size() > n.size()) {
    		maior = &p;
    		menor = &n;
    	} else {
    		maior = &n;
    		menor = &p;
    	}
    	int numValDatFile = (menor->size()*2)/(holdoutRatio+1);
    	int numDatFile = ((menor->size()*2)/(holdoutRatio+1))*holdoutRatio;

    	datFile << numDatFile << " " << num_input << " " << num_output << std::endl;
    	valDatFile << numValDatFile << " " << num_input << " " << num_output << std::endl;
    	this->qtdValidationSamples = numValDatFile;

    	std::string separador = ",";
    	int count = numDatFile + numValDatFile;
    	std::cout << "Amostras usadas: " <<count << std::endl;
    	int countProp = holdoutRatio;
    	while (count > 0) {

    		//Pego uma vez um elemento P outra um elemento N
    		if((count % 2) == 0){
    			line =  maior->front();
    			maior->pop();
    			count--;
    		}else{
    			line = menor->front();
    			menor->pop();
    			count--;
    		}

    		//contator responsável por decidir a hora de colocar no arquivo de validação
    		if (countProp == 0) {
    			flagData = true;
    		} else {
    			countProp--;
    		}

    		//Tirando as vírgulas da linha
    		for (int j = 0; j < (line.size() - 1); j++) {

    			if (line.substr(j, 1).compare(separador) == 0) {
    				line.replace(j, 1, " ");
    			}
    		}
    		out = line.substr(line.size() - 1);
    		line.replace(line.size() - 1, 1, " ");
    		//Verificando se é hora de colocar no arquivo de validação
    		if (flagData) {
    			valDatFile << line << std::endl;
    			if (out == "P") {
    				valDatFile << "1.000000" << " -1.000000" << std::endl;
    			} else {
    				valDatFile << "-1.000000" << " 1.000000" << std::endl;
    			}
    			countProp = holdoutRatio;
    			flagData = false;
    		} else {
    			datFile << line << std::endl;
    			if (out == "P") {
    				datFile << "1.000000" << " -1.000000" << std::endl;
    			} else {
    				datFile << "-1.000000" << " 1.000000" << std::endl;
    			}

    		}



    	}


    //fim for
    	datFile.close();
    	valDatFile.close();

    this->trainingSet = fann_read_train_from_file(TRAIN_FILE);
    this->validationSet = fann_read_train_from_file(VALIDATION_FILE);
}

fann_train_data * ArffTrainingSet::getTrainingData() {
    return this->trainingSet;
}

long ArffTrainingSet::getQtdValidationSamples() {
    // TODO: Retornar a quantidade de pontos presentes em validation.dat
    return this->qtdValidationSamples;
}

fann_train_data * ArffTrainingSet::getValidationData() {
    return this->validationSet;
}
