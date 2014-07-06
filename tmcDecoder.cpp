/*
 * tmcDecoder.cpp
 *
 *  Created on: 06.04.2014
 *      Author: clorenz
 */

#include "tmcDecoder.h"
#include "rdsDecoder.h"
#include <string.h>

tmcDecoder::tmcDecoder() : ltn(0),ltnbe(0),sid(0),afi(0),mode(0),encid(0),providerNameComplete(0)
,lastMsgNumber(0), lastCI(0),totalMsgNr(0) {
	providerName = new char[8];
	lastMsg = NULL;
}

tmcDecoder::~tmcDecoder() {
	delete[] providerName;
	delete lastMsg;
}

void tmcDecoder::decodeSystemMsg(int msg){
	char variant = (msg >> 14) & 0x3;

	if(variant == 0x0){
		ltn = (msg >> 6) & 0x3F;
		afi = (msg >> 5) & 0x1;
		mode = (msg >> 4) & 0x1;
		scope.international = (msg >> 3) & 0x1;
		scope.national = (msg >> 2) & 0x1;
		scope.rural = (msg >> 1) & 0x1;
		scope.urban =  msg & 0x1;
	}else if(variant == 0x1){
		sid = (msg >> 6) & 0x3F;
		// TODO: Gap implementation
	}
}
void tmcDecoder::decodeMsg(int blocks[]){
	bool tuningInfo = (blocks[BLOCK_B] >> 4) & 0x1;
	int encryptionNullBlock = blocks[BLOCK_B] & 0x1F;

	if(!encryptionNullBlock)
		decodeEncryptionInfo(blocks);
	if(tuningInfo)
		decodeTuningInfo(blocks);
	else
		decodeUserMsg(blocks);
}
void tmcDecoder::decodeEncryptionInfo(int blocks[]){
	char adminGroup = blocks[BLOCK_C] >> 13 & 0x7;

	if(adminGroup == 0){
		// The only valid Encryption Administration group
		sid = blocks[BLOCK_C] >> 5 & 0x3F;
		encid = blocks[BLOCK_C] & 0x1F;
		ltnbe = blocks[BLOCK_D] >> 10 & 0x3F;

		// cerr << "ENCID: " << encid << endl;
	}
}

void tmcDecoder::decodeTuningInfo(int blocks[]){
	char address = blocks[BLOCK_B] & 0xF;

	if(address == 4){
		// Provider-Name char 1 - 4
		providerName[0] = (blocks[BLOCK_C] >> 8) & 0xFF;
		providerName[1] = blocks[BLOCK_C] & 0xFF;
		providerName[2] = (blocks[BLOCK_D] >> 8) & 0xFF;
		providerName[3] = blocks[BLOCK_D] & 0xFF;
		providerNameComplete |= 0x1;
	}else if(address == 5){
		// Provider-Name char 5 - 8
		providerName[4] = (blocks[BLOCK_C] >> 8) & 0xFF;
		providerName[5] = blocks[BLOCK_C] & 0xFF;
		providerName[6] = (blocks[BLOCK_D] >> 8) & 0xFF;
		providerName[7] = blocks[BLOCK_D] & 0xFF;
		providerNameComplete |= 0x2;
	}
}

void tmcDecoder::decodeUserMsg(int blocks[]){
	bool singleGroup = blocks[BLOCK_B] >> 3 & 0x1; // F in specification
	int CI = blocks[BLOCK_B] & 0x7; // CI for MultiPart; Duration and Persistence value for Single
	bool firstGroup = blocks[BLOCK_C] >> 15 & 0x1; // indicate first group in MultiMsg; diversion Bit in Single
	bool secondGroup = blocks[BLOCK_C] >> 14 & 0x1;
	int gsi = blocks[BLOCK_C] >> 12 & 0x3; // Group Seq Idx: Only for MsgNr > 1 of multipart
	// cout << hex << blocks[BLOCK_A] << " " << blocks[BLOCK_B] << " " << blocks[BLOCK_C] << " " << blocks[BLOCK_D] << dec << endl;
	if(singleGroup || firstGroup){
		// cleanup last uncomplete Msg
		delete lastMsg;

		// OK, lets start with a new Msg
		int loc = blocks[BLOCK_D];
		short ext = blocks[BLOCK_C] >> 14 & 0x7;
		int evt = blocks[BLOCK_C] & 0x7FF;
		bool negDirection = blocks[BLOCK_C] >> 14 & 0x1;

		// cerr << "Start New Msg Loc: " << loc << endl;

		// Create new Msg
		lastMsg = new tmcMsg(singleGroup, negDirection, ext, evt, loc);

		if(!singleGroup){
			// Await the next number in Multipart Msg
			lastMsgNumber = 1;
			lastCI = CI;
			totalMsgNr = 0;
		}else{
			// Single Grp Msg
			lastMsg->setDuration(CI);
			lastMsg->setDiversion(firstGroup);
			// process Single Msgs
			processMessage();
			lastMsg = NULL; // prepare for next Msg
			lastMsgNumber = 0; // prevent Bug: first => single => second
		}
	}else if(lastMsgNumber >= 1 && !singleGroup && lastCI == CI){
		if(secondGroup && !totalMsgNr) // Calculate Total Nr of Msgs
			totalMsgNr = 2 + gsi;
		if(!totalMsgNr)
			// Oh 2nd Group was missing, error
			return;

		short msgNr = totalMsgNr - gsi;

		if(msgNr != lastMsgNumber+1)
			// Doubled or missing Msg
			return;

		// cout << "Total: " << totalMsgNr << " Nr: " << msgNr << endl;

		int32_t content = (blocks[BLOCK_D] & 0xFFFF) | ((blocks[BLOCK_C] & 0xFFF) << 16);

		// bitsetqueue test;
		// test.addValue(content, 28);
		// cout << "Content: " << test << endl;

		lastMsg->addOptional(content);

		if(gsi)
			lastMsgNumber++;
		else {
			// Last Msg of Multigroup
			lastMsgNumber = 0;
			processMessage();
		}
	}else{
		// Oh an error
		lastMsgNumber = 0;
	}
}

bool tmcDecoder::isProviderNameComplete(){
	return providerNameComplete == 0x3;
}

const char* tmcDecoder::getProviderName(){
	return providerName;
}

void tmcDecoder::processMessage(){
	lastMsg->processOptional();
	cout << *lastMsg << endl;
	// TODO: Process Message into List
	list<ITMCListener*>::iterator iter = this->listeners.begin();
	while(iter != listeners.end()) {
		(*iter)->processTMCMessage(*lastMsg);
		iter++;
	}
}

void tmcDecoder::addListener(ITMCListener* listener) {
	this->listeners.push_back(listener);
}
