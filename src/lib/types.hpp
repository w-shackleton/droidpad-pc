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
#ifndef DP_TYPES_H
#define DP_TYPES_H

// No includes!

namespace droidpad {
	enum {
		MODE_JS,
		MODE_MOUSE,
		MODE_SLIDE,
		MODE_ABSMOUSE,
	};

	/**
	 * Misc vector class
	 */
	class Vec2 {
		public:
			float x, y;
			Vec2(float x, float y);
			Vec2(const Vec2 &other);
			Vec2();

			Vec2 & operator+=(const Vec2 &rhs);
			const Vec2 operator+(const Vec2 &other) const;
			Vec2 & operator-=(const Vec2 &rhs);
			const Vec2 operator-(const Vec2 &other) const;

			Vec2 & operator*=(const float rhs);
			const Vec2 operator*(const float rhs) const;
			Vec2 & operator/=(const float rhs);
			const Vec2 operator/(const float rhs) const;
	};
}

#endif
