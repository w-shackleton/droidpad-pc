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
#ifndef DP_WIN_OUTPUT_MGR_H
#define DP_WIN_OUTPUT_MGR_H

#include <stdexcept>
#include "wPlatformSettings.hpp"
#include "output/IOutputMgr.hpp"

#include "jsOutputs.hpp"

namespace droidpad {
	class DPJSData;
	class DPMouseData;
	class DPSlideData;
	class OutputManager : public IOutputManager {
		public:
			OutputManager(const int type, const int numAxes, const int numButtons);
			~OutputManager();

			void SendJSData(const DPJSData& data, bool firstIteration = true);
			void SendMouseData(const DPMouseData& data, bool firstIteration = true);
			void SendSlideData(const DPSlideData& data, bool firstIteration = true);
		private:
			droidpad::win32::VJoyOutputs *joystick;
	};
}

#endif
