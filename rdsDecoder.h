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


	bool decode3A(int blocks[]);
	bool decodeODA(int blocks[]);

public:
	rdsDecoder();
	~rdsDecoder();

	bool decodeRDS(int blocks[]);

	void resetData();
	void printAll(ostream &out);
};


#endif /* RDSDECODER_H_ */
