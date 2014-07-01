/*
 * Locations.cpp
 *
 *  Created on: 02.05.2014
 *      Author: clorenz
 */

#include "Locations.h"
#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <fstream>

using namespace std;

Locations* Locations::instance = NULL;

Locations::Locations(const char* filename) {
	if (filename)
		importFile(filename);
}

Locations::~Locations() {

}

Locations* Locations::getInstance(const char* filename) {
	if (!instance)
		instance = new Locations(filename);
	return instance;
}

void Locations::importFile(const char* filename) {
	// Open File
	ifstream locFile(filename);

	// Check existance of file
	if (!locFile) {
		cerr << "Error: Location-File not readable." << endl;
		return;
	}

	string line;
	Location* loc;

	getline(locFile, line); // Read first header-Line

	while (getline(locFile, line)) {

		stringstream lineStream(line);
		loc = new Location();

		lineStream >> *loc;

		if (!lineStream.fail()) {
			locList[loc->getLcd()] = loc; // save in List
		} else {
			cerr << "Failed to read Line: " << line << endl;
		}

	}
}

Location* Locations::findLocation(const int code) {
	return locList[code];
}

