#include "outputMgr.hpp"

#include "types.hpp"

using namespace std;
using namespace droidpad;

OutputManager::OutputManager(const int type, const int numAxes, const int numButtons) {
	dpinput = new dpinfo;

	dpinput->axisMin = -AXIS_SIZE;
	dpinput->axisMax = AXIS_SIZE;
	dpinput->buttonNum = numButtons;
	dpinput->axisNum = numAxes;
	int ret;
	switch(type) {
		case MODE_JS:
			ret = dpinput_setup(dpinput,TYPE_JS);
			break;
		case MODE_MOUSE:
			ret = dpinput_setup(dpinput,TYPE_MOUSE);
			break;
		case MODE_SLIDE:
			ret = dpinput_setup(dpinput,TYPE_KEYBD);
			break;
		default:
			throw invalid_argument("Invalid type");
	}
	if(ret < 0) {
		throw runtime_error("Couldn't setup uinput");
	}
}

OutputManager::~OutputManager() {
	dpinput_close(dpinput);
	delete dpinput;
}
