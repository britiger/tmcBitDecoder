/*
 * rdsDecoder.cpp
 *
 *  Created on: 06.04.2014
 *      Author: clorenz
 */


#include "rdsDecoder.h"

ptime rdsDecoder::rdsDateTime = second_clock::universal_time();
time_duration rdsDecoder::rdsDateTimeDiff(0,0,0,0);

rdsDecoder::rdsDecoder() : tmc(NULL) {
	this->resetData();
}

rdsDecoder::~rdsDecoder() {}

bool rdsDecoder::decodeRDS(int blocks[]){
	// analyse basic data of each message
	piCode = blocks[BLOCK_A];
	pty = (blocks[BLOCK_B] >> 5) & 0x1F;

	tp = (blocks[BLOCK_B] >> 10) & 0x1;

	lastGrpB = (blocks[BLOCK_B] >> 11) & 0x1;
	lastGrp = (blocks[BLOCK_B] >> 12) & 0xF;

	if(lastGrp == 3 && !lastGrpB)
		// 3A
		return this->decode3A(blocks);
	if(lastGrp == 4 && !lastGrpB)
		// 4A
		return this->decode4A(blocks);
	else if(lastGrp == 8 && !lastGrpB)
		return this->decodeODA(blocks);

	return false;
}

bool rdsDecoder::decode3A(int blocks[]) {
	if(blocks[BLOCK_D] == 0xCD46 || blocks[BLOCK_D] == 0xCD45){
		// ODA TMC
		// decode SystemMsg via tmcDecoder
		tmc->decodeSystemMsg(blocks[BLOCK_C]);
		tmcGroup = blocks[BLOCK_B] & 0x1F;
		return true;
	} else {
		// Unknown ODA
	}
	return false;
}

/**
 * Decodes Block 4A: Clock-time and date
 */
bool rdsDecoder::decode4A(int blocks[]) {
	bitsetqueue bitqueue;
	// adding Block B (last 2 bits) + Block C + Block D to bitqueue
	short blockb = blocks[BLOCK_B] >> 14 & 0x3;
	bitqueue.addValue(blockb, 2);
	bitqueue.addValue(blocks[BLOCK_C], 16);
	bitqueue.addValue(blocks[BLOCK_D], 16);

	int32_t mjd = bitqueue.getNextBits(17);
	int8_t hour = bitqueue.getNextBits(5);
	int8_t minute = bitqueue.getNextBits(6);
	int8_t utc_offset = (bitqueue.getNextBits(1) * -2 + 1) * // UTC-Offset (+/-)
						bitqueue.getNextBits(5);	// Hour

	int16_t year = (mjd - 15078.2) / 365.25;
	int16_t month = ( mjd - 14956.1 - (int) (year * 365.25) ) / 30.6001;
	int16_t day = mjd - 14956 - (int)(year * 365.25 ) - (int)(month * 30.6001);

	short k = (month == 14 || month == 15)?1:0;

	year += 1900 + k;
	month -= 1 + k * 12;

	if(year < 2013 || month > 12 || month < 1 || hour > 23 || hour < 0 || minute
			> 59 || minute < 0 || utc_offset < -12 || utc_offset > 12)
		return false; // invalid date (first check)

	// TODO: Check to disallow future dates (max. +2 for local time differences)
	try {
		date d(year, month, day+20);
		ptime dateTime(d);
		dateTime += hours(hour);
		dateTime += minutes(minute);

		// Save Date and difference in Object
		rdsDecoder::rdsDateTime = dateTime;
		rdsDecoder::rdsDateTimeDiff = dateTime - second_clock::universal_time();
	}catch(boost::exception const&  ex) {
		// realy invalid date
		return false;
	}
	/*
	cerr << rdsDecoder::rdsDateTime << endl;
	cerr << "Time: " << (int)year << "-" << month << "-" << day<< " " << (int)hour << ":"
		<< (int)minute << " Offset: " << (int)utc_offset
		<< " 0x" << ostream::hex << blocks[BLOCK_D] << endl;
	 */
	return true;
}

bool rdsDecoder::decodeODA(int blocks[]){

	// check TMC
	if(tmcGroup == ((lastGrp << 1) | lastGrpB)){
		tmc->decodeMsg(blocks);
		return true;
	}

	return false;
}

ptime rdsDecoder::getRDSTime() {
	return  second_clock::universal_time() + rdsDecoder::rdsDateTimeDiff;
}

void rdsDecoder::resetData(){
	piCode = 0;
	pty = 0;
	tp = false;
	lastGrp = 0;
	lastGrpB = false;
	tmcGroup = 0;

	// delete tmc;

	tmc = new tmcDecoder();
}

void rdsDecoder::printAll(ostream &out) {
	out <<  "PI: " << hex << piCode;
	out << " TP: " << boolalpha << tp;
	out << " PTY: " << dec << pty;
	out << " Group: " << lastGrp << (lastGrpB?"B":"A");
	out << endl;
}

void rdsDecoder::addTMCListener(ITMCListener* listener) {
	tmc->addListener(listener);
}
