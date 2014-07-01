/*
 * tmcMsg.cpp
 *
 *  Created on: 13.04.2014
 *      Author: clorenz
 */

#include "tmcMsg.h"
#include "Locations.h"
#include "boost/date_time.hpp"

const short tmcMsg::labelLength[] = { 3, 3, 5, 5, 5, 8, 8, 8, 8, 11, 16, 16, 16,
		16, 0, -1 };

tmcMsg::tmcMsg(bool single, bool negDirection, short extend, int event,
		int location) :
		single(single), negDirection(negDirection), extend(extend), location(
				location), sourceLocation(NULL), duration(0), diversion(NULL), currentInfoBlock(
				NULL), startTime(NULL), stopTime(NULL), urgent(ur_normal), directionaly(
				dir_none), dynamic(true), spoken(false), invertDirectionaly(
				false) {
	// create first Info-Block
	currentInfoBlock = new tmcInfoBlock(event);
	infoBlocks.push_back(currentInfoBlock);
}

tmcMsg::~tmcMsg() {
	delete diversion;
	delete startTime;
	delete stopTime;
	for (vector<tmcInfoBlock*>::iterator it = infoBlocks.begin();
			it != infoBlocks.end(); ++it) {
		delete *it;
	}
}

void tmcMsg::setDuration(const short duration) {
	// this->duration = duration;
}

void tmcMsg::setDiversion(const bool diversion) {
	delete this->diversion;
	this->diversion = new bool;
	*this->diversion = diversion;
}

void tmcMsg::setSourceLocation(const int sourceLocation) {
	delete this->sourceLocation;
	this->sourceLocation = new int;
	*this->sourceLocation = sourceLocation;
}

void tmcMsg::setStartTime(const short time) {
	delete startTime;
	startTime = new time_t;
	*startTime = getConvertedTime(time);
}

void tmcMsg::setStopTime(const short time) {
	delete stopTime;
	stopTime = new time_t;
	*stopTime = getConvertedTime(time);
}

void tmcMsg::addOptional(const int32_t content) {
	optionalCache.addValue(content, 28);
	// processOptional();
}

void tmcMsg::processOptional() {

	int label = optionalCache.getNextBits(4);
	if (label == 15)
		return;
	int content = optionalCache.getNextBits(labelLength[label]);

	if (!single)
		while (label || content) {

			switch (label) {
			case 0: // Duration
				setDuration(content);
				break;
			case 1:
				processControlCode(content);
				break;
			case 2: // Length of route affected
				currentInfoBlock->setLength(content);
				break;
			case 3: // Speed Limit
				currentInfoBlock->setSpeed(content);
				break;
			case 4: // Additional quantifier 5bits
			case 5: // and 8 bits
				currentInfoBlock->addQuantifier(content);
				break;
			case 6:
				currentInfoBlock->addSuppInfo(content);
				break;
			case 7: // Begin of Message
				setStartTime(content);
				break;
			case 8: // Ende of Message
				setStopTime(content);
				break;
			case 9: // Additional Event Code
				currentInfoBlock->addEvent(content);
				break;
			case 10: // Location Codes for Diversersion
				currentInfoBlock->addDiversion(content);
				break;
			case 11: // Event only for this Destination
				currentInfoBlock->addDestination(content);
				break;
			case 13: // Source Location Code
				setSourceLocation(content);
				break;
			case 14: // Seperator
				currentInfoBlock = new tmcInfoBlock();
				infoBlocks.push_back(currentInfoBlock);
				break;
			default:
				cout << "Label: " << label << endl;
				cout << "Content: " << hex << content << dec << endl;
				break;
			}

			// load next value
			label = optionalCache.getNextBits(4);
			if (label == 15)
				return;
			content = optionalCache.getNextBits(labelLength[label]);
		}

	// Find maximum Urgent + Directionality
	for (vector<tmcInfoBlock*>::const_iterator it = infoBlocks.begin();
			it != infoBlocks.end(); ++it) {
		urgent += (**it).getUrgent();
		directionaly &= (**it).getDirectionality();
	}

	if(invertDirectionaly)
		directionaly = !directionaly;
}

void tmcMsg::processControlCode(char content) {
	switch (content) {
	case 0:
		currentInfoBlock->increaseUrgency();
		break;
	case 1:
		currentInfoBlock->decreaseUrgency();
		break;
	case 2:
		invertDirectionaly = true;
		break;
	case 3:
		dynamic = !dynamic;
		break;
	case 4:
		spoken = !spoken;
		break;
	case 5:
		setDiversion(true);
		break;
	case 6:
		extend += 8;
		break;
	case 7:
		extend += 16;
		break;
	}
}

time_t tmcMsg::getConvertedTime(const short time_value){
	using namespace boost::posix_time;
	using namespace boost::gregorian;

	ptime dateTime = second_clock::universal_time();

	if(time_value < 96) { // 0-95 Time of this Day in 15min Interval
		short min15 = time_value % 4; // Count of 15min Blocks
		short hour  = (time_value-min15)/4;
		dateTime = ptime(dateTime.date()); // Set Current date 00:00
		dateTime += hours(hour)+minutes(min15*15); // Set H:m
	} else if(time_value < 201) { // 96-200 hours adding today
		short hour = time_value - 96;
		dateTime = ptime(dateTime.date()); // Set Current date
		dateTime += hours(24 + hour); // Set to midnight + calculated hours
	} else if(time_value < 232) { // 201-231 Day Of month
		date currDate = dateTime.date();
		dateTime = ptime(date(currDate.year(), currDate.month(), time_value-200)); // Setting new day of month
		dateTime += hours(23)+minutes(59)+seconds(59); // Setting last hour and minute
	} else { // 232-255 half month interval within next 12 month 1=
		date currDate = dateTime.date();
		short new_value = time_value - 232; // 0 = 15th Jan, 1 = 31st Jan
		short month = new_value / 2 + 1;
		bool end_of_month = new_value % 2; // 0 = 15th, 1 = 28,29,30,31

		short day = 15;
		if(end_of_month) {
			// calculate Last Day
			date get_eom = date(currDate.year(), month, 1);
			day = get_eom.end_of_month().day().as_number();
		}

		date newDate = date(currDate.year(), month, day);
		if(newDate < currDate) // Check next year?
			newDate += years(1);

		dateTime = ptime(newDate);
		dateTime += hours(23)+minutes(59)+seconds(59); // Setting last hour and minute

	}

	tm dateTimeTm = to_tm(dateTime);

	return timegm(&dateTimeTm);
}

ostream& operator<<(ostream& out, const tmcMsg& msg) {
	Locations* locs = Locations::getInstance();
	Location* loc = locs->findLocation(msg.location);

	if (!loc)
		cerr << "Location " << msg.location << " not found." << endl;

	cout << endl << "TMC Message" << endl;
	cout << "Type: " << (msg.single ? "Single" : "Multi") << endl;
	cout << "Location: " << (loc ? (*loc) : msg.location) << endl;
	cout << "Extend: " << msg.extend << endl;
	cout << "Direction: " << (msg.negDirection ? "neg" : "pos") << endl;
	cout << "Directionality: " << msg.directionaly << endl;
	cout << "Urgent: " << msg.urgent << endl;
	if(msg.duration)
		cout << "Duration: " << msg.duration << endl;
	if(msg.startTime)
		cout << "Start: " << ctime(msg.startTime) << endl;
	if(msg.stopTime)
			cout << "Stop: " << ctime(msg.stopTime) << endl;

	// cout << "BitQue: " << msg.optionalCache << endl;

	for (vector<tmcInfoBlock*>::const_iterator it = msg.infoBlocks.begin();
			it != msg.infoBlocks.end(); ++it) {
		cout << "Info-Block:" << endl;
		cout << **it << endl;
	}

	return out;
}
