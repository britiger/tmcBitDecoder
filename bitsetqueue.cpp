#include "bitsetqueue.h"
#include <iostream>
using namespace std;

bitsetqueue::bitsetqueue():position(0),size(0) {

}

bitsetqueue::~bitsetqueue() {

}

void bitsetqueue::addValue(int16_t value, const short length) {
	int i = length - 1;
	for(;i>-1;i--){
		bool bit = value & (1 << i);
		this->setBit(bit);
	}
}

void bitsetqueue::setBit(bool value){
	if(size >= BITSETQUEUE_SIZE){
		cerr << "Too much data in queue." << endl;
		return;
	}
	bits.set(size++, value);
}

int32_t bitsetqueue::getNextBits(short size) {
	int32_t returnInt = 0;


	int i;
	for(i=0;i<size;i++){
		returnInt = returnInt << 1;
		returnInt |= getBit()?0x1:0x0;
	}

	return returnInt;
}

bool bitsetqueue::getBit(){
	if(position>=size)
		return false;
	return bits[position++];
}

short bitsetqueue::getSize(){
	return size-position;
}

ostream& operator<< (ostream& out, const bitsetqueue& q){

	for(int i = 0;i < q.size;i++){
		out << q.bits[i];
	}
	out<< "(" << q.position << "/" << q.size << ")";

	return out;
}
