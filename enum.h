/*
 * enum.h
 *
 *  Created on: 01.05.2014
 *      Author: clorenz
 */

#ifndef ENUM_H_
#define ENUM_H_

#include <iostream>

using namespace std;


enum Directionality {
	dir_none = -1, dir_monodirectional = 1, dir_bidirectional = 2
};
enum Urgent {
	ur_normal = 0, ur_urgent = 1, ur_extremely = 2
};
enum Nature {
	na_none, na_forecast, na_silent
};
enum Quantifiere {
	q_none = -1,
	q_smallnumber = 0,
	q_number = 1,
	q_distance = 2,
	q_percent = 3,
	q_speed = 4,
	q_min_hour = 5,
	q_degree = 6,
	q_time = 7,
	q_tonnes = 8,
	q_meters = 9,
	q_millimeters = 10,
	q_mhz = 11,
	q_khz = 12
};

istream& operator>>(istream& in, Directionality& d);
ostream& operator<<(ostream& out, const Directionality& d);
Directionality operator!(const Directionality& d);
Directionality& operator&=(Directionality& d, const Directionality& add_dir);

istream& operator>>(istream& in, Urgent& u);
ostream& operator<<(ostream& out, const Urgent& u);
Urgent& operator++(Urgent& u);
Urgent& operator--(Urgent& u);
Urgent& operator+=(Urgent& u, const Urgent& add_u);

istream& operator>>(istream& in, Nature& n);
ostream& operator<<(ostream& out, const Nature& n);

istream& operator>>(istream& in, Quantifiere& q);
ostream& operator<<(ostream& out, const Quantifiere& q);

#endif /* ENUM_H_ */
