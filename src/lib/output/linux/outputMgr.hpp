#ifndef DP_LIN_OUTPUT_MGR_H
#define DP_LIN_OUTPUT_MGR_H

#include "dpinput.h"
#include <stdexcept>

namespace droidpad {
	class OutputManager {
		public:
			/**
			  Type is according to those in "types.hpp"
			  */
			OutputManager(const int type, const int numAxes, const int numButtons);
			~OutputManager();
		private:
			dpinfo *dpinput;
	};
}

#endif
