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
#include <wx/stopwatch.h>
#include "net/connection.hpp"
#include <deque>

#define TOUCHSCREEN_MOVING_AVG_NUM 10

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

			void SendJSData   (const decode::DPJSData& data, bool firstIteration = true);
			void SendMouseData(const decode::DPMouseData& data, bool firstIteration = true);
			void SendTouchData(const decode::DPTouchData& data, bool firstIteration = true);
			void SendSlideData(const decode::DPSlideData& data, bool firstIteration = true);
		private:
			IOutputManager* mgr;

			wxMutex callMutex;
			decode::DPJSData jsData;
			decode::DPMouseData mouseData;
			decode::DPSlideData slideData;
			std::deque<decode::DPTouchData> touchDataQueue;
			decode::DPTouchData touchCurrentSmoothed, touchCurrentSmoothed2, touchPrevSmoothed;
			Vec2 touchVelocity;
			bool firstIteration;
			wxStopWatch touchTimer;

			template<typename T> static T getMovingAverage(std::deque<T> values);
	};
}

#endif
