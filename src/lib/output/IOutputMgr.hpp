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
#ifndef DP_I_OUTPUT_MGR_H
#define DP_I_OUTPUT_MGR_H

namespace droidpad {
	class DPJSData;
	class DPMouseData;
	class DPSlideData;
	class IOutputManager {
		public:
			/**
			  Type is according to those in "types.hpp"
			  */
			IOutputManager(const int type, const int numAxes, const int numButtons);
			virtual ~IOutputManager();

			virtual void BeginToStop();

			virtual void SendJSData(const DPJSData& data, bool firstIteration = true) = 0;
			virtual void SendMouseData(const DPMouseData& data, bool firstIteration = true) = 0;
			virtual void SendSlideData(const DPSlideData& data, bool firstIteration = true) = 0;
		protected:
			int type;
	};
}

#endif
