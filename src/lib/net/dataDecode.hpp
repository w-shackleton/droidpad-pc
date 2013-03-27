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
		// Applies a gamma function, to make the middle parts of this axis
		// more sensitive to movement.
		// value - Input value, in the range [-1,1]
		// gamma - Input gamma, in the range [-1,1]
		// return value - value in the range [-1,1]
		float applyGamma(float value, float gamma);

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

				bool containsAccel;
				bool containsGyro;

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

		class DPTouchData : public Vec2 {
			public:
				DPTouchData();
				DPTouchData(const DPTouchData& old);
				DPTouchData(const DPJSData& rawData, const DPJSData& prevData, const DPTouchData& prevAbsData);

				/**
				 * scrollDelta: down is positive, same scale as incrementalScrollDelta.
				 * incrementalScrollDelta: increments are 120.
				 */
				int scrollDelta, incrementalScrollDelta;
				bool bLeft, bMiddle, bRight;

				float xOffset, yOffset;
		};

		class DPSlideData {
			public:
				DPSlideData();
				DPSlideData(const DPSlideData& old);
				DPSlideData(const DPJSData& rawData, const DPJSData& prevData);
				// Note: white & black are toggle buttons.
				bool next, prev, start, finish, white, black, beginning, end;
		};
		
		/**
		 * Each binary packet begins with four bytes,
		 * identifying the type of message.
		 */
		typedef struct {
			char h[4];
			
			inline bool isConnectionInfo() {
				printf("isConnectionInfo: %02x%02x%02x%02x == %s?\n",
						h[0], h[1], h[2], h[3], "DINF");
				return memcmp(h, "DINF", 4) == 0;
			}
			inline bool isBinaryHeader() {
				printf("isBinaryHeader: %02x%02x%02x%02x == %s?\n",
						h[0], h[1], h[2], h[3], "DPAD");
				return memcmp(h, "DPAD", 4) == 0;
			}
		} BinarySignature;

		typedef struct {
			// In this case, "DINF"
			BinarySignature sig;

			// One of:
			// * MODE_MOUSE
			// * MODE_SLIDE
			// * MODE_ABSMOUSE
			// * MODE_JS
			int32_t modeType;

			int32_t rawDevices;
			int32_t axes;
			int32_t buttons;
			char reserved[32];
		} BinaryConnectionInfo;

		typedef struct {
			// In this case, "DPAD"
			BinarySignature sig;
			int32_t numElements;
			int32_t flags;

			union {
				struct {
					// Accel
					float ax, ay, az;
					// Gyro, and normalised gyro
					float gx, gy, gz, gzn;
					// Reserved
					float rx, ry, rz;
				} axis;
				struct {
					// Accel
					uint32_t ax, ay, az;
					// Gyro, and normalised gyro
					uint32_t gx, gy, gz, gzn;
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

		const BinaryConnectionInfo getBinaryConnectionInfo(const char *binaryInfo);
		const RawBinaryHeader getBinaryHeader(const char *binaryHeader);
		const RawBinaryElement getBinaryElement(const char *binaryElement);

		const DPJSData getBinaryData(const RawBinaryHeader header, std::vector<RawBinaryElement> elems);
	};
};

#endif
