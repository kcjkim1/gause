#ifndef CONFIG_H_
#define CONFIG_H_

#include <iostream>

using namespace std;

class Configuration {
	public:
		Configuration( int, int, int, bool, bool );
		Configuration( string );
		
		int getWindowSize();
		int getColorSpace();
		int getNumberOfLevels();
		void save();
		string getPositionDescription();
		int getDimension();
		bool usesHorizontal();
		bool usesVertical();
		bool isCompatible(Configuration*);
	private:
		int colorSpace;
		void setColorSpace( int );
		int numberOfLevels;
		void setNumberOfLevels( int );
		int windowSize;
		void setWindowSize( int );
		bool horizontalFlag;
		bool verticalFlag;
};

#endif /*CONFIG_H_*/
