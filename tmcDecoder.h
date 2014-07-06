/*
 * tmcDecoder.h
 *
 *  Created on: 06.04.2014
 *      Author: clorenz
 */

#ifndef TMCDECODER_H_
#define TMCDECODER_H_

#include <iostream>
#include "tmcMsg.h"
#include "ITMCListener.h"

using namespace std;

struct msgScope {
	bool international;
	bool national;
	bool rural;
	bool urban;
};

class tmcDecoder {
private:
	int ltn;
	int ltnbe;
	int sid;
	int afi;
	int mode;
	short encid;
	msgScope scope;
	char* providerName;
	short providerNameComplete;
	short lastMsgNumber;
	short lastCI;
	short totalMsgNr;

	tmcMsg* lastMsg;

	list<ITMCListener*> listeners;

	void decodeEncryptionInfo(int blocks[]);
	void decodeTuningInfo(int blocks[]);
	void decodeUserMsg(int blocks[]);

	void processMessage();
public:
	tmcDecoder();
	~tmcDecoder();

	void decodeSystemMsg(int msg);
	void decodeMsg(int blocks[]);

	bool isProviderNameComplete();

	const char* getProviderName();

	void addListener(ITMCListener* listener);
};

#endif /* TMCDECODER_H_ */
