/*
 * Event.h
 *
 *  Created on: 30.04.2014
 *      Author: clorenz
 */

#ifndef EVENT_H_
#define EVENT_H_

#include <iostream>
#include "enum.h"

using namespace std;

class Event {
private:
	short code;
	string text;
	Nature nature;
	Quantifiere quantifier;
	Directionality directionality;
	Urgent urgent;
	short updateClass;
	string referenceCode;

public:
	Event(short code = 0, string text = "", Nature nature = na_none, Quantifiere quantifier = q_none,
			Directionality directionality = dir_none, Urgent urgent = ur_normal, short updateClass = 0,
			string referenceCode = "");
	~Event();

	short const getCode();
	string const getText();
	Urgent const getUrgent();
	Directionality const getDirectionality();

	friend istream& operator>>(istream& in, Event& e);
	friend ostream& operator<<(ostream& out, const Event& e);
};

#endif /* EVENT_H_ */
