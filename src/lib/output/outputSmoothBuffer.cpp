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
#include "outputSmoothBuffer.hpp"

#include "types.hpp"
#define SLEEP_TIME 15

using namespace droidpad;
using namespace droidpad::decode;

OutputSmoothBuffer::OutputSmoothBuffer(IOutputManager *mgr, const int type, const int numAxes, const int numButtons) :
	IOutputManager(type, numAxes, numButtons),
	mgr(mgr)
{
	Create();
	Run();
}

OutputSmoothBuffer::~OutputSmoothBuffer()
{
	delete mgr;
}

void* OutputSmoothBuffer::Entry()
{
	while(!TestDestroy()) {
		wxThread::Sleep(SLEEP_TIME);
		wxMutexLocker lock(callMutex);
		switch(type) {
			case MODE_JS:
				mgr->SendJSData(jsData, false);
				break;
			case MODE_MOUSE:
				mgr->SendMouseData(mouseData, false);
				break;
			case MODE_SLIDE:
				mgr->SendSlideData(slideData, false);
				break;
			case MODE_ABSMOUSE:
				mgr->SendTouchData(touchData, false);
				break;
		}
	}
}

void OutputSmoothBuffer::BeginToStop()
{
	Delete();
}

void OutputSmoothBuffer::SendJSData(const DPJSData& data, bool firstIteration)
{
	wxMutexLocker lock(callMutex);
	jsData = data;
	mgr->SendJSData(data);
}

void OutputSmoothBuffer::SendMouseData(const DPMouseData& data, bool firstIteration)
{
	wxMutexLocker lock(callMutex);
	mouseData = data;
	mouseData.x = mouseData.x * SLEEP_TIME / 60;
	mouseData.y = mouseData.y * SLEEP_TIME / 60;
	mgr->SendMouseData(mouseData);
}

// TODO: Do properly
void OutputSmoothBuffer::SendTouchData(const decode::DPTouchData& data, bool firstIteration) {
	wxMutexLocker lock(callMutex);
	touchData = data;
	mgr->SendTouchData(touchData);
}

void OutputSmoothBuffer::SendSlideData(const DPSlideData& data, bool firstIteration)
{
	wxMutexLocker lock(callMutex);
	slideData = data;
	mgr->SendSlideData(data);
}

