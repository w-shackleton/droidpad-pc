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
#ifndef DP_OUTPUT_SMOOTHER_H
#define DP_OUTPUT_SMOOTHER_H

#include "IOutputMgr.hpp"
#include <wx/thread.h>
#include "net/connection.hpp"

namespace droidpad {
	class OutputSmoothBuffer : public IOutputManager, private wxThread {
		public:
			/**
			  * Constructs a new buffer, which threads the process and outputs data more frequently.
			  * ownership is taken of mgr.
			  */
			OutputSmoothBuffer(IOutputManager* mgr, const int type, const int numAxes, const int numButtons);
			~OutputSmoothBuffer();
			void* Entry();

			void BeginToStop();

			void SendJSData   (const DPJSData& data);
			void SendMouseData(const DPMouseData& data);
			void SendSlideData(const DPSlideData& data);
		private:
			IOutputManager* mgr;

			wxMutex callMutex;
			DPJSData jsData;
			DPMouseData mouseData;
			DPSlideData slideData;
	};
}

#endif
