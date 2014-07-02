/*
 * bitsetqueue.h
 *
 *  Created on: 20.04.2014
 *      Author: clorenz
 */

#ifndef BITSETQUEUE_H_
#define BITSETQUEUE_H_

#include <bitset>
#include <iostream>
#include <inttypes.h>

#define BITSETQUEUE_SIZE 280

using namespace std;

class bitsetqueue {

private:
	bitset<BITSETQUEUE_SIZE> bits;
	short position;
	short size;

	void setBit(bool value);

public:
	bitsetqueue();
	~bitsetqueue();

	void addValue(int16_t value, const short length);
	int32_t getNextBits(short size);
	bool getBit();
	short getSize();

	friend ostream& operator<< (ostream& out, const bitsetqueue& q);
};


#endif /* BITSETQUEUE_H_ */
