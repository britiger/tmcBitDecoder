/*
 * tmcMsg.h
 *
 *  Created on: 13.04.2014
 *      Author: clorenz
 */

#ifndef TMCMSG_H_
#define TMCMSG_H_

#include <iostream>
#include <vector>
#include <inttypes.h>
#include <ctime>
#include "enum.h"
#include "tmcInfoBlock.h"
#include "bitsetqueue.h"


using namespace std;

class tmcMsg {
private:
	bool single;
	bool negDirection;
	short extend;
	int location;
	int* sourceLocation;
	short duration;
	bool* diversion;

	vector<tmcInfoBlock*> infoBlocks;
	tmcInfoBlock* currentInfoBlock;

	time_t* startTime;
	time_t* stopTime;

	Urgent urgent;
	Directionality directionaly;
	bool dynamic;
	bool spoken;

	bool invertDirectionaly;

	bitsetqueue optionalCache;

	// length of Optional message content by label 0 ... 14, 15 is reserved
	static const short labelLength[16];

	time_t getConvertedTime(const short time);
public:
	tmcMsg(bool single, bool negDirection, short extend, int event, int location);
	tmcMsg(const tmcMsg& msg);
	~tmcMsg();

	void setDuration(const short duration);
	void setDiversion(const bool diversion);
	void setSourceLocation(const int sourceLocation);
	void setStartTime(const short time);
	void setStopTime(const short time);

	void addOptional(const int32_t content);
	void processOptional();
	void processControlCode(char content);

	friend ostream& operator<<(ostream& out, const tmcMsg& msg);
};

#endif /* TMCMSG_H_ */
