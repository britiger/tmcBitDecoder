/*
 * tmcInfoBlock.cpp
 *
 *  Created on: 20.04.2014
 *      Author: clorenz
 */

#include "tmcInfoBlock.h"
#include "Events.h"

tmcInfoBlock::tmcInfoBlock(short event): speed(NULL), length(NULL), currentUgent(ur_normal),
	maxUrgent(ur_normal), directionaly(dir_none){
	if(event)
		addEvent(event);
}

tmcInfoBlock::~tmcInfoBlock(){

}

// Please submit the content-value, this function converts it
void tmcInfoBlock::setSpeed(const char speed){
	delete this->speed;
	this->speed = new char;
	*this->speed = speed * 5;
}

void tmcInfoBlock::setLength(const char length){
	delete this->length;
	this->length = new char;

	if(length == 0)
		*this->length = 150; // Length >100
	else if(length > 10 && length < 16)
		*this->length = 10 + (length-10)*2;
	else if(length > 15)
		*this->length = 20 + (length-15)*5;
	else
		*this->length = length;
}

void tmcInfoBlock::addEvent(const short event){
	Event* e = Events::getInstance()->findEvent(event);
	if(e) {
		events.push_back(e);
		maxUrgent += currentUgent;
		directionaly &= e->getDirectionality();
		currentUgent = e->getUrgent();
	} else
		cerr << "Unknown Event Code: " << event << endl;
}

void tmcInfoBlock::addDiversion(const uint16_t diversion){
	diversions.push_back(diversion);
}

void tmcInfoBlock::addDestination(const uint16_t destination){
	destinations.push_back(destination);
}

void tmcInfoBlock::addQuantifier(const short quantifier){
	if(events.size())
		quantifiers[events.back()->getCode()] = quantifier;
}

void tmcInfoBlock::addSuppInfo(const short suppInfo){
	if(events.size())
		suppInfos[events.back()->getCode()].push_back(suppInfo);
}

void tmcInfoBlock::increaseUrgency(){
	++currentUgent;
}

void tmcInfoBlock::decreaseUrgency(){
	--currentUgent;
}

Urgent const tmcInfoBlock::getUrgent(){
	maxUrgent += currentUgent;
	return maxUrgent;
}

Directionality const tmcInfoBlock::getDirectionality(){
	return directionaly;
}

ostream& operator<<(ostream& out, const tmcInfoBlock& blk){
	if(blk.speed)
		cout << "-Speed: " << (short)*blk.speed << endl;
	if(blk.length)
		cout << "-Length: " << (short)*blk.length << endl;
	cout << "-Events:" << endl;

	for (list<Event*>::const_iterator it = blk.events.begin();
				it != blk.events.end(); ++it) {
		cout << "\t" << **it << endl;

		// Display Quantifier
		blk.quantifiers.find((*it)->getCode());
		map<short, short>::const_iterator q_it = blk.quantifiers.find((*it)->getCode());
		if(q_it != blk.quantifiers.end())
			cout << "\t" << "Quantifier(hex): " << hex << q_it->second << dec << endl;

		// Display SupplInfo
		map<short, list<short> >::const_iterator s_it = blk.suppInfos.find((*it)->getCode());
				if(s_it != blk.suppInfos.end()) {
					list<short> supp = s_it->second;
					for (list<short>::iterator su_it=supp.begin(); su_it != supp.end(); ++su_it)
						cout << "\t" << "Suppl. Info: " <<  *su_it << endl;
				}
	}
	if(blk.destinations.size()) {
		cout << "-Destinations: " << endl;
		for (list<uint16_t>::const_iterator it = blk.destinations.begin();
						it != blk.destinations.end(); ++it) {
			cout << "\t" << *it << endl;
		}
	}
	if(blk.diversions.size()) {
			cout << "-Diversersion: " << endl;
			for (list<uint16_t>::const_iterator it = blk.diversions.begin();
							it != blk.diversions.end(); ++it) {
				cout << "\t" << *it << endl;
			}
		}

	return out;
}
