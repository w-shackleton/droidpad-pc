#include "proc.hpp"

#include <string>
#include <iostream>
#include <stdio.h>

using namespace droidpad;
using namespace std;

string droidpad::runProcess(string cmd) {
	FILE* pipe = popen(cmd.c_str(), "r");
	if (!pipe) throw PROC_FAIL;
	char buffer[128];
	string result = "";
	while(!feof(pipe)) {
		if(fgets(buffer, 128, pipe) != NULL)
			result += buffer;
	}
	pclose(pipe);
	return result;
}
