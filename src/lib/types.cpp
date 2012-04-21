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
#include "types.hpp"

#include <cmath>

using namespace droidpad;

Vec2::Vec2() :
	x(0),
	y(0) { }

Vec2::Vec2(float x, float y) :
	x(x),
	y(y) { }

Vec3::Vec3(float x, float y, float z) :
	x(x),
	y(y),
	z(z) { }
Vec3::Vec3() :
	x(0),
	y(0),
	z(0) { }

Vec3 Vec3::unit() {
	float m = magnitude();
	return Vec3(x / m, y / m, z / m);
}
float Vec3::dot(Vec3 other) {
	return	x * other.x +
		y * other.y +
		z * other.z;
}

float Vec3::magnitude() {
	return sqrt(x*x + y*y + z*z);
}
