#include "TrainPattern.h"

TrainPattern::TrainPattern(double ch1, double ch2, double ch3) {
	this->ch1 = ch1;
	this->ch2 = ch2;
	this->ch3 = ch3;
	this->positive = 0;
	this->negative = 0;
}

TrainPattern::~TrainPattern() {
}

void TrainPattern::inc() {
	this->positive++;
}
void TrainPattern::dec() {
	this->negative++;
}

int TrainPattern::getScore() {
	return this->positive - this->negative;
}

void TrainPattern::setH(double h) {
	this->h = h;
}

void TrainPattern::setV(double v) {
	this->v = v;
}

int TrainPattern::compare(TrainPattern * other, bool usesh, bool usesv) {
    if ( this->ch1 < other->ch1 ) {
    	return -1;
    } else if ( this->ch1 > other->ch1 ) {
    	return +1;
    } else if ( this->ch2 < other->ch2 ) {
    	return -1;
    } else if ( this->ch2 > other->ch2 ) {
    	return +1;
    } else if ( this->ch3 < other->ch3 ) {
    	return -1;
    } else if ( this->ch3 > other->ch3 ) {
    	return +1;
    } else if ( usesh && this->h < other->h ) {
    	return -1;
    } else if ( usesh && this->h > other->h ) {
    	return +1;
    } else if ( usesv && this->v < other->v ) {
    	return -1;
    } else if ( usesv && this->v > other->v ) {
    	return +1;
    } else {
    	return 0;
    }
}

std::string TrainPattern::getText(bool usesh, bool usesv) {
	std::string text = "(";
	char buff[100];
	sprintf(buff,"%f, %f, %f",ch1, ch2, ch3);
	text = text + buff;
	if (usesh) {
    	sprintf(buff,", %f",h);
		text = text + buff;
    }
    if (usesv) {
    	sprintf(buff,", %f",v);
		text = text + buff;
    }
	text = text + ")";
	return text;
}

double TrainPattern::getCh1() {
	return this->ch1;
}

double TrainPattern::getCh2() {
	return this->ch2;
}

double TrainPattern::getCh3() {
	return this->ch3;
}

double TrainPattern::geth() {
	return this->h;
}

double TrainPattern::getv() {
	return this->v;
}

int TrainPattern::getPositive() {
	return this->positive;
}

int TrainPattern::getNegative() {
	return this->negative;
}