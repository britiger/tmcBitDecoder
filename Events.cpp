/*
 * Events.cpp
 *
 *  Created on: 30.04.2014
 *      Author: clorenz
 */

#include "Events.h"
#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <fstream>

using namespace std;

Events* Events::instance = NULL;

Events::Events(const char* filename) {
	if (filename)
		importFile(filename);
}

Events::~Events() {

}

Events* Events::getInstance(const char* filename) {
	if (!instance)
		instance = new Events(filename);
	return instance;
}

void Events::importFile(const char* filename) {
	// Open File
	ifstream eventFile(filename);

	// Check existance of file
	if (!eventFile) {
		cerr << "Error: Event-File not readable." << endl;
		return;
	}

	string line;
	Event* e;

	getline(eventFile, line); // Read first header-Line

	while (getline(eventFile, line)) {

		stringstream lineStream(line);
		e = new Event();

		lineStream >> *e;

		if (!lineStream.fail()) {
			eventList[e->getCode()] = e; // save in List
		} else {
			cerr << "Failed to read Line: " << line << endl;
		}

	}
}

Event* Events::findEvent(const short code) {
	return eventList[code];
}
