/*
 * bitDecoder.h
 *
 *  Created on: 04.04.2014
 *      Author: clorenz
 */

#ifndef BITDECODER_H_
#define BITDECODER_H_

#include "rdsDecoder.h"
#include "ITMCListener.h"

#define A  0
#define B  1
#define C  2
#define Ci 3
#define D  4

#define _5BIT 0x000001F
#define _10BIT 0x00003FF
#define _16BIT 0x000FFFF
#define _26BIT 0x3FFFFFF
#define _28BIT 0xFFFFFFF

bool insync = false;
int offset[] = {0x0FC, 0x198, 0x168, 0x350, 0x1B4};
int ofs2block[] = {0,1,2,2,3};
int* rcvd = new int[5];
int* GrpData = new int[5];
bool* errblock = new bool[50];
int BlkPointer;

rdsDecoder* decoder = new rdsDecoder();

long syndrome(long);
void get_grp();
int get_bit();
void blockerror();
void decodegroup(int GrpData[]);

class processClass : public ITMCListener {
	void processTMCMessage(tmcMsg msg);
};

#endif /* BITDECODER_H_ */
