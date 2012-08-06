/*
 * This file is part of DroidPad.
 * DroidPad lets you use an Android mobile to control a joystick or mouse
 * on a Windows or Linux computer.
 *
 * DroidPad is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * DroidPad is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with DroidPad, in the file COPYING.
 * If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef DP_MATH_UTIL_H
#define DP_MATH_UTIL_H

#define NTOH(_x) _x = ntohl((_x))

namespace droidpad {
	template <class T>
	inline void trim(T &num, T min, T max) {
		if(num < min) num = min;
		if(num > max) num = max;
	}

	// An example of this case is trim(float value, -1, 1)
	template <class T, class U>
	inline void trim(T &num, U min, U max) {
		if(num < min) num = min;
		if(num > max) num = max;
	}

	// fabs, redefined generically and inline. Not much point really
	template <class T>
	inline T abs(T num) {
		if(num < 0) return -num;
		return num;
	}

	// Couldn't find this one when with no internet, so rewrote it.
	template <class T>
	inline T sign(T num) {
		if(num < 0) return -1;
		if(num > 0) return 1;
		return 0;
	}

};

#endif
