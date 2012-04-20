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
#ifndef DP_DATADECODE_H
#define DP_DATADECODE_H

#include "types.hpp"
#include <stdint.h>
#include <vector>
#include <string>
#include <wx/string.h>

#define HEADER_FLAG_HAS_ACCEL 0x1
#define HEADER_FLAG_HAS_GYRO 0x2
#define HEADER_FLAG_STOP 0x4

#define ITEM_FLAG_BUTTON 0x1
#define ITEM_FLAG_TOGGLE_BUTTON (0x2 | ITEM_FLAG_BUTTON)
#define ITEM_FLAG_SLIDER 0x4
#define ITEM_FLAG_TRACKPAD 0x8
#define ITEM_FLAG_HAS_X_AXIS 0x10
#define ITEM_FLAG_HAS_Y_AXIS 0x20
#define ITEM_FLAG_IS_RESET 0x40

namespace droidpad {
	namespace decode {
		droidpad::Vec2 accelToAxes(float x, float y, float z);

		/**
		 * Raw data returned from connection. Is castable to the other data types,
		 * which contain data from it.
		 */
		class DPJSData {
			public:
				DPJSData();
				DPJSData(const DPJSData& old);
				std::vector<int> axes;
				std::vector<int> touchpadAxes;
				std::vector<bool> buttons;

				/**
				 * If true, the connection was closed normally.
				 */
				bool connectionClosed;

				/**
				 * If true, reset the axes / button (context specific)
				 */
				bool reset;

				/**
				 * Reorders the axes according to the given reordering data.
				 */
				void reorder(std::vector<int> bmap, std::vector<int> amap);
		};

		class DPMouseData {
			public:
				DPMouseData();
				DPMouseData(const DPMouseData& old);
				DPMouseData(const DPJSData& rawData, const DPJSData& prevData);

				/**
				 * scrollDelta: down is positive, same scale as incrementalScrollDelta.
				 * incrementalScrollDelta: increments are 120.
				 */
				int x, y, scrollDelta, incrementalScrollDelta;
				bool bLeft, bMiddle, bRight;
		};

		class DPSlideData {
			public:
				DPSlideData();
				DPSlideData(const DPSlideData& old);
				DPSlideData(const DPJSData& rawData, const DPJSData& prevData);
				// Note: white & black are toggle buttons.
				bool next, prev, start, finish, white, black, beginning, end;
		};
		

		typedef struct {
			char headerBytes[4];
			int32_t numElements;
			int32_t flags;

			union {
				struct {
					// Accel
					float ax, ay, az;
					// Gyro
					float gx, gy, gz;
					// Reserved
					float rx, ry, rz;
				} axis;
				struct {
					// Accel
					uint32_t ax, ay, az;
					// Gyro
					uint32_t gx, gy, gz;
					// Reserved
					uint32_t rx, ry, rz;
				} raw;
			};
		} RawBinaryHeader;

		typedef struct {
			int32_t flags;
			union {
				struct {
					uint32_t data1;
					uint32_t data2;
					uint32_t data3;
				} raw;
				struct {
					float data1;
					float data2;
					float data3;
				} floating;
				struct {
					int32_t data1;
					int32_t data2;
					int32_t data3;
				} integer;
			};
		} RawBinaryElement;

		/**
		 * Converts an input line to a DPJSData
		 */
		const DPJSData getTextData(wxString line);

		const RawBinaryHeader getBinaryHeader(const char *binaryHeader);
		const RawBinaryElement getBinaryElement(const char *binaryElement);

		const DPJSData getBinaryData(const RawBinaryHeader header, std::vector<RawBinaryElement> elems);
	};
};

#endif
