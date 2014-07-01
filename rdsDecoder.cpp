/*
 * rdsDecoder.cpp
 *
 *  Created on: 06.04.2014
 *      Author: clorenz
 */


#include "rdsDecoder.h"

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

bool rdsDecoder::decodeODA(int blocks[]){

	// check TMC
	if(tmcGroup == ((lastGrp << 1) | lastGrpB)){
		tmc->decodeMsg(blocks);
		return true;
	}

	return false;
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
