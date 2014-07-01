/*
 * Locations.h
 *
 *  Created on: 02.05.2014
 *      Author: clorenz
 */

#ifndef LOCATIONS_H_
#define LOCATIONS_H_

#include <istream>
#include <map>
#include "Location.h"

using namespace std;

class Locations {
private:
	map<int, Location*> locList;
	static Locations* instance;

	Locations(const char* filename);
public:

	static Locations* getInstance(const char* filename = NULL);
	~Locations();

	void importFile(const char* filename);
	Location* findLocation(const int code);
};


#endif /* LOCATIONS_H_ */
