#ifndef TRAINPATTERN_H_
#define TRAINPATTERN_H_

#include <string>

class TrainPattern {
private:
	int positive;
	int negative;
	double ch1;
	double ch2;
	double ch3;
	double h;
	double v;
public:
	TrainPattern(double, double, double);
	virtual ~TrainPattern();
	void inc();
	void dec();	
	void setH(double);
	void setV(double);
	int getScore();
	int getPositive();
	int getNegative();
	int compare(TrainPattern*, bool, bool);
	std::string getText(bool, bool);
	double getCh1();
	double getCh2();
	double getCh3();
	double geth();
	double getv();
};

#endif /*TRAINPATTERN_H_*/