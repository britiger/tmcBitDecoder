/*
 * Location.cpp
 *
 *  Created on: 30.04.2014
 *      Author: clorenz
 */

#include "Location.h"

template<char C>
std::istream& Char(std::istream& in) {
	char c;
	if (in >> c && c != C)
		in.setstate(std::ios_base::failbit);
	return in;
}

Location::Location(int lcd, string name): lcd(lcd), name(name){

}
Location::~Location(){

}

string const Location::getName(){
	return name;
}

int const Location::getLcd(){
	return lcd;
}

istream& operator>>(istream& in, Location& loc){
	in >> loc.lcd >> Char<';'>;

	if(!in.fail()) {
		getline(in, loc.name);
		if(in.fail())
			loc.name = "none";
		in.clear(); // Ignore Reding-Errors
	}

	return in;
}

ostream& operator<<(ostream& out, const Location& loc){

	out << loc.lcd << " - " << loc.name;

	return out;
}


