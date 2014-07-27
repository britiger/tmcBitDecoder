/*
 * bitDecoder.cpp
 * Based on https://github.com/windytan/redsea
 * Copyright (c) 2007-2012, windytan (OH2EIQ)
 * rewrited in c++
 * Created on: 04.04.2014
 *      Author: Christopher Lorenz
 */

#include <iostream>
#include <stdlib.h>
#include <limits>
#include <map>
#include <math.h>
#include "bitDecoder.h"
#include "bitsetqueue.h"

#include "Events.h"
#include "Locations.h"
#include "tmcMsg.h"

using namespace std;

void processClass::processTMCMessage(tmcMsg msg) {
	cout << msg << endl;
}

int main() {
	Events* e = Events::getInstance();
	e->importFile("data/events.csv"); // Import Events
	e->importFile("data/forecast.csv"); // Import Forecasts
	Locations* loc = Locations::getInstance("data/location.csv"); // Import LCL

	decoder->addTMCListener(new processClass());
	get_grp();
}

long syndrome(long vector) {
	int k;
	int l;
	long bit;
	int SyndReg = 0x000;

	for (k = 25; k >= 0; k--) {
		bit = (vector & (1 << k));
		l = (SyndReg & 0x200);      // Store lefmost bit of register
		SyndReg = (SyndReg << 1) & 0x3FF; // Rotate register
		if (bit)
			SyndReg ^= 0x31B;         // Premultiply input by x^325 mod g(x)
		if (l)
			SyndReg ^= 0x1B9;           // Division mod 2 by g(x)
	}
	return SyndReg;
}

char lastByte;
int bitCnt = 0;

int get_bit() {
	/*
	 if(!bitCnt){
	 // read next byte
	 cin >> lastByte;
	 bitCnt = 8;
	 }

	 bitCnt--;
	 int ret = lastByte & 0x8;
	 lastByte = lastByte << 1;
	 return ret;
	 */
	return cin.get() - '0';
}

void get_grp() {

	map<int, int> ErrLookup;
	int err;
	int i;

	int bitcount = 0;
	int prevbitcount = 0;
	int lefttoread = 26;
	int wideblock = 0;
	int block = 0;
	int pi = 0;
	int SyndReg = 0;

	int prevsync = 0;
	int dist = 0;
	int message = 0;
	int expofs = 0; // not defined in perl

	bool* syncb = new bool[5];

	for (i = 0; i < 16; i++) {
		err = 1 << i;
		ErrLookup[syndrome(0x00004b9 ^ (err << 10))] = err;
	}

	// Weiterer Block für ErrLookup fehlt.
	/*for (char patt = 0x01; patt <= 0x1F; patt += 2) {
		for (i = 0; i <= 16 - log2(patt) + 1; i++) {
			err = patt << i;
			ErrLookup[syndrome(0x00005b9 ^ (err << 10))] = err;
		}
	}*/

	while (1) {

		// Compensate for clock slip corrections
		bitcount += 26 - lefttoread;

		// Read from radio
		for (i = 0; i < (insync ? lefttoread : 1); i++, bitcount++) {
			wideblock = (wideblock << 1) + get_bit();
		}

		lefttoread = 26;
		wideblock &= _28BIT;

		block = (wideblock >> 1) & _26BIT;

		// Find the offsets for which the syndrome is zero
		for (i = A; i <= D; i++)
			syncb[i] = (syndrome(block ^ offset[i]) == 0);

		// Acquire sync

		if (!insync) {
			if (syncb[A] | syncb[B] | syncb[C] | syncb[Ci] | syncb[D]) {

				for (i = A; i <= D; i++) {
					if (syncb[i]) {
						dist = bitcount - prevbitcount;

						if (dist % 26 == 0 && dist <= 156
								&& (ofs2block[prevsync] + dist / 26) % 4
										== ofs2block[i]) {
							insync = true;
							expofs = i;
							break;
						} else {
							prevbitcount = bitcount;
							prevsync = i;
						}
					}
				}
			}
		}

		// Synchronous decoding
		if (insync) {
			BlkPointer++;

			message = block >> 10;

			// If expecting C but we only got a Ci sync pulse, we have a Ci block
			if (expofs == C && !syncb[C] && syncb[Ci])
				expofs = Ci;

			// If this block offset won't give a sync pulse
			if (!syncb[expofs]) {

				// If it's a correct PI, the error was probably in the check bits and hence is ignored
				if (expofs == A && message == pi && pi != 0) {
					syncb[A] = true;
				} else if (expofs == C && message == pi && pi != 0) {
					syncb[Ci] = true;
				}
				// Detect & correct clock slips (C.1.2)

				else if (expofs == A && pi != 0
						&& ((wideblock >> 12) & _16BIT) == pi) {
					message = pi;
					wideblock >>= 1;
					syncb[A] = true;
				} else if (expofs == A && pi != 0
						&& ((wideblock >> 10) & _16BIT) == pi) {
					message = pi;
					wideblock = (wideblock << 1) + get_bit();
					syncb[A] = true;
					lefttoread = 25;
				}

				// Detect & correct burst errors (B.2.2)

				SyndReg = syndrome(block ^ offset[expofs]);

				if (ErrLookup.find(SyndReg) == ErrLookup.end()) {
					message = (block >> 10) ^ ErrLookup[SyndReg];
					syncb[expofs] = true;
				}

				// If still no sync pulse
				if (!syncb[expofs]) {
					blockerror();
				}
			}

			// Error-free block received
			if (syncb[expofs]) {

				GrpData[ofs2block[expofs]] = message;
				errblock[BlkPointer % 50] = false;
				rcvd[expofs] = true;

				if (expofs == A)
					pi = message;

				// If a complete group is received
				if (rcvd[A] && rcvd[B] && (rcvd[C] || rcvd[Ci]) && rcvd[D]) {
					decodegroup(GrpData);
				}
			}

			// The block offset we're expecting next
			expofs = (expofs == C ? D : (expofs + 1) % 5);

			if (expofs == A) {
				for (i = A; i <= D; i++)
					rcvd[i] = 0;
			}
		}
	}

}

void blockerror() {
	int i;

	errblock[BlkPointer % 50] = true;

	int erbloks = 0;

	for (i = 0; i < 50; i++)
		erbloks += errblock[i] ? 1 : 0;

// Sync is lost when >45 out of last 50 blocks are erroneous (C.1.2)
	if (insync && erbloks > 45) {
		insync = false;
		for (i = 0; i < 50; i++)
			errblock[i] = false;
	}
	for (i = A; i <= D; i++)
		rcvd[i] = false;
}

void decodegroup(int GrpData[]) {

	decoder->decodeRDS(GrpData);

}

