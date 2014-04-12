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
#ifndef DP_PLATFORM_SETTINGS_H
#define DP_PLATFORM_SETTINGS_H

// Size of the axis, in each direction.
#define AXIS_SIZE 16384
// For analogue axes, the 3 means that the axis rotates around a third of the axis.
#define AXIS_CUTOFF_MULTIPLIER (AXIS_SIZE * 3)

// Sort of hack.
#define JS_OFFSET 16384

#endif
