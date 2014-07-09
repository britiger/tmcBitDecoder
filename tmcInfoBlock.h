/*
 * tmcInfoBlock.h
 *
 *  Created on: 20.04.2014
 *      Author: clorenz
 */

#ifndef TMCINFOBLOCK_H_
#define TMCINFOBLOCK_H_

#include <inttypes.h>
#include <list>
#include <map>
#include "Event.h"

using namespace std;

class tmcInfoBlock {
private:
	char* speed;
	char* length;

	list<Event*> events;
	map<short, short> quantifiers; // Mapping the quantifiers to the events
	map<short, list<short> > suppInfos; // Mapping the SuppInfos to the events

	list<uint16_t> diversions;
	list<uint16_t> destinations;

	Urgent currentUgent;
	Urgent maxUrgent;

	Directionality directionaly;

public:
	tmcInfoBlock(short event = 0);
	tmcInfoBlock(const tmcInfoBlock& block);
	~tmcInfoBlock();

	void setSpeed(const char speed);
	void setLength(const char length);
	void addEvent(const short event);
	void addDiversion(const uint16_t diversion);
	void addDestination(const uint16_t destination);
	void addQuantifier(const short quantifier);
	void addSuppInfo(const short suppInfo);

	void increaseUrgency();
	void decreaseUrgency();

	Urgent const getUrgent();
	Directionality const getDirectionality();

	friend ostream& operator<<(ostream& out, const tmcInfoBlock& blk);
};

#endif /* TMCINFOBLOCK_H_ */
