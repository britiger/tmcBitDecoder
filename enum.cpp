/*
 * enum.cpp
 *
 *  Created on: 01.05.2014
 *      Author: clorenz
 */

#include "enum.h"

istream& operator>>(std::istream& in, Directionality& d) {
	char c;
	if (in >> c) {
		if (c == '1')
			d = dir_monodirectional;
		else if (c == '2')
			d = dir_bidirectional;
		else if (c == ';') { // End of field
			d = dir_none;
			in.seekg(-1, ios_base::cur);
		} else
			in.setstate(std::ios_base::failbit);
	}
	return in;
}

ostream& operator<<(std::ostream& out, const Directionality& d) {
	switch (d) {
	case dir_bidirectional:
		out << "bidirectional";
		break;
	case dir_monodirectional:
		out << "monodirectional";
		break;
	default:
		out << "none";
		break;
	}

	return out;
}

Directionality operator!(const Directionality& d) {
	switch (d) {
	case dir_bidirectional:
		return dir_monodirectional;
		break;
	case dir_monodirectional:
		return dir_bidirectional;
		break;
	}
	return dir_none;
}

Directionality& operator&=(Directionality& d, const Directionality& add_dir){

	if(add_dir == dir_none){}
	else if(d == dir_none)
		d = add_dir;
	else if(d == dir_monodirectional || add_dir == dir_monodirectional)
		d = dir_monodirectional;

	return d;
}

istream& operator>>(istream& in, Urgent& u) {
	char c;
	if (in >> c) {
		if (c == 'U')
			u = ur_urgent;
		else if (c == 'X')
			u = ur_extremely;
		else if (c == ';') { // End of field
			u = ur_normal;
			in.seekg(-1, ios_base::cur);
		} else
			in.setstate(std::ios_base::failbit);
	}
	return in;
}

ostream& operator<<(ostream& out, const Urgent& u) {
	switch (u) {
	case ur_normal:
		out << "normal";
		break;
	case ur_urgent:
		out << "urgent";
		break;
	case ur_extremely:
		out << "extremely urgent";
		break;
	}
	return out;
}

istream& operator>>(istream& in, Nature& n) {
	char c;
	if (in >> c) {
		if (c == 'F')
			n = na_forecast;
		else if (c == 'S')
			n = na_silent;
		else if (c == ';') { // End of field
			n = na_none;
			in.seekg(-1, ios_base::cur);
		} else
			in.setstate(std::ios_base::failbit);
	}
	return in;
}

Urgent& operator++(Urgent& u){
	u = static_cast<Urgent>((u+1)%3);
	return u;
}

Urgent& operator--(Urgent& u){
	switch(u) {
	case ur_normal:
		u = ur_extremely;
		break;
	case ur_urgent:
			u = ur_normal;
			break;
	case ur_extremely:
				u = ur_urgent;
				break;
	}
	return u;
}

Urgent& operator+=(Urgent& u, const Urgent& add_u){

	if(add_u > u)
		u = add_u;

	return u;
}

ostream& operator<<(ostream& out, const Nature& n) {
	switch (n) {
	case na_forecast:
		out << "forecast";
		break;
	case na_silent:
		out << "silent";
		break;
	default:
		out << "none";
		break;
	}
	return out;
}

istream& operator>>(istream& in, Quantifiere& q) {
	short s;
	char c;

	q = q_none;

	if (in >> s) {
		if(s >= 0 && s <= 12)
			q = (enum Quantifiere) s;
	} else { // Not a Number
		in.clear();
		if(in >> c){
			if(c == ';') // End of Field
				in.seekg(-1, ios_base::cur);
			else
				in.setstate(std::ios_base::failbit);
		}
	}
	return in;
}

ostream& operator<<(ostream& out, const Quantifiere& q) {
	switch (q) {
	case q_none:
		out << "none";
		break;
	case q_smallnumber:
		out << "small number";
		break;
	case q_number:
		out << "number";
		break;
	case q_distance:
			out << "distance";
			break;
	case q_percent:
			out << "percent";
			break;
	case q_speed:
			out << "speed";
			break;
	case q_min_hour:
			out << "minute/hour";
			break;
	case q_degree:
			out << "degree";
			break;
	case q_time:
			out << "time";
			break;
	case q_tonnes:
			out << "tonnes";
			break;
	case q_meters:
			out << "meters";
			break;
	case q_millimeters:
			out << "millimeters";
			break;
	case q_mhz:
			out << "MHz";
			break;
	case q_khz:
			out << "kHz";
			break;
	default:
		out << "unknown";
		break;
	}
	return out;
}
