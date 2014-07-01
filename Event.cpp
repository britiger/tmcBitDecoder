/*
 * Event.cpp
 *
 *  Created on: 30.04.2014
 *      Author: clorenz
 */

#include "Event.h"

template<char C>
std::istream& Char(std::istream& in) {
	char c;
	if (in >> c && c != C)
		in.setstate(std::ios_base::failbit);
	return in;
}

Event::Event(short code, string text, Nature nature, Quantifiere quantifier,
		Directionality directionality, Urgent urgent, short updateClass,
		string referenceCode) :
		code(code), text(text), nature(nature), quantifier(quantifier), directionality(
				directionality), urgent(urgent), updateClass(updateClass) {

}

Event::~Event() {

}

short const Event::getCode(){
	return code;
}
string const Event::getText(){
	return text;
}

Urgent const Event::getUrgent(){
	return urgent;
}

Directionality const Event::getDirectionality(){
	return directionality;
}

istream& operator>>(istream& in, Event& e) {
	string field;
	in >> e.code >> Char<';'>; // Read Code

	getline(in, e.text, ';'); // Msg Text
	in >> e.nature >> Char<';'>; // Nature
	in >> e.quantifier >> Char<';'>; // Quantifier

	getline(in, field, ';'); // TODO Duration Type

	in >> e.directionality >> Char<';'>; // Directionality
	in >> e.urgent >> Char<';'>;
	in >> e.updateClass >> Char<';'>;
	if(!in.fail()) {
		getline(in, e.referenceCode);
		in.clear(); // ignore errors of this field
	}
	return in;
}

ostream& operator<<(ostream& out, const Event& e) {

	out << e.code << " " << e.text << " N: " << e.nature << " Q: "
			<< e.quantifier << " D: " << e.directionality << " U: " << e.urgent
			<< " C: " << e.updateClass << " R: " << e.referenceCode;
	return out;
}
