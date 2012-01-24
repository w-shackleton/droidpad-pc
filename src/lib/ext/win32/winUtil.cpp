#include <winUtil.hpp>

#include "js/targetver.h"

#include <windows.h>
#include <winnt.h>

// Taken from winnt.h. Wasn't working for some reason.
#define PROCESSOR_ARCHITECTURE_INTEL 0
#define PROCESSOR_ARCHITECTURE_MIPS 1
#define PROCESSOR_ARCHITECTURE_ALPHA 2
#define PROCESSOR_ARCHITECTURE_PPC 3
#define PROCESSOR_ARCHITECTURE_SHX 4
#define PROCESSOR_ARCHITECTURE_ARM 5
#define PROCESSOR_ARCHITECTURE_IA64 6
#define PROCESSOR_ARCHITECTURE_ALPHA64 7
#define PROCESSOR_ARCHITECTURE_MSIL 8
#define PROCESSOR_ARCHITECTURE_AMD64 9
#define PROCESSOR_ARCHITECTURE_IA32_ON_WIN64 10
#define PROCESSOR_ARCHITECTURE_UNKNOWN 0xFFFF

// Checks the CPU architecture. Return type is the same as GetSystemInfo type.
int cpuType() {
	SYSTEM_INFO info;
	GetNativeSystemInfo(&info);
	switch(info.wProcessorArchitecture) {
		case PROCESSOR_ARCHITECTURE_AMD64:
			return CPU_amd64;
		case PROCESSOR_ARCHITECTURE_INTEL:
			return CPU_x86;
		default:
			return CPU_UNKNOWN;
	}
}

