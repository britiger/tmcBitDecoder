/*
 * Events.h
 *
 *  Created on: 30.04.2014
 *      Author: clorenz
 */

#ifndef EVENTS_H_
#define EVENTS_H_

#include "Event.h"
#include <istream>
#include <map>

using namespace std;


class Events {
private:
	map<short, Event*> eventList;
	static Events* instance;

	Events(const char* filename);
public:

	static Events* getInstance(const char* filename = NULL);
	~Events();

	void importFile(const char* filename);
	Event* findEvent(const short code);
};


#endif /* EVENTS_H_ */
