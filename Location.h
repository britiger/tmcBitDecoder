/*
 * Location.h
 *
 *  Created on: 30.04.2014
 *      Author: clorenz
 */

#ifndef LOCATION_H_
#define LOCATION_H_

#include <iostream>

using namespace std;

class Location {
private:
	int lcd;
	string name;
public:
	Location(int lcd= 0, string name = "");
	~Location();

	string const getName();
	int const getLcd();

	friend istream& operator>>(istream& in, Location& loc);
	friend ostream& operator<<(ostream& out, const Location& loc);
};

#endif /* LOCATION_H_ */
