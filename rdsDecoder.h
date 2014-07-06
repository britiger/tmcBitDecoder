/*
 * rdsDecoder.h
 *
 *  Created on: 06.04.2014
 *      Author: clorenz
 */

#ifndef RDSDECODER_H_
#define RDSDECODER_H_

#include <iostream>
#include "tmcDecoder.h"
#include "ITMCListener.h"
#include "boost/date_time.hpp"

using namespace boost::posix_time;
using namespace boost::gregorian;
using namespace std;

#define BLOCK_A  0
#define BLOCK_B  1
#define BLOCK_C  2
#define BLOCK_D  3

class rdsDecoder {

private:
	int  piCode;
	int  pty;
	bool tp;
	int  lastGrp;
	bool lastGrpB;
	int  tmcGroup;
	tmcDecoder* tmc;

	static ptime rdsDateTime; // Time in UTC
	static time_duration rdsDateTimeDiff; // Difference between Local and rds time (UTC)

	bool decode3A(int blocks[]);
	bool decode4A(int blocks[]);
	bool decodeODA(int blocks[]);

public:
	rdsDecoder();
	~rdsDecoder();

	bool decodeRDS(int blocks[]);

	static ptime getRDSTime();

	void resetData();
	void printAll(ostream &out);

	void addTMCListener(ITMCListener* listener);
};


#endif /* RDSDECODER_H_ */
