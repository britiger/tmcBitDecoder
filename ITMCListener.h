/*
 * ITMCListener.h
 *
 *  Created on: 06.07.2014
 *      Author: clorenz
 */

#ifndef ITMCLISTENER_H_
#define ITMCLISTENER_H_

#include "tmcMsg.h"

class ITMCListener {
public:
	virtual void processTMCMessage(tmcMsg msg) = 0;
};

#endif /* ITMCLISTENER_H_ */
