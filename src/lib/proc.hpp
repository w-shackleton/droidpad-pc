#ifndef DP_PROC_H
#define DP_PROC_H

#include <string>

#define PROC_FAIL 1

namespace droidpad {
	std::string runProcess(std::string cmd);
	inline std::string runProcess(std::string cmd, std::string args) {
		return runProcess(cmd + " " + args);
	}
}

#endif
