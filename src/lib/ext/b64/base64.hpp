#ifndef _BASE_64_H
#define _BASE_64_H

#include <string>

#include <wx/string.h>

std::string base64_encode(unsigned char const* , unsigned int len);
std::string base64_decode(std::string const& s);

// Added for wxWidgets compatibility
inline wxString base64_encode(wxString in) {
	unsigned char *data = new unsigned char[in.Length()];
	const char* c_in = in.mb_str();
	strcpy((char*) data, c_in);
	wxString result(base64_encode(data, in.Length()).c_str(), wxConvUTF8);
	delete[] data;
	return result;
}
inline std::string base64_encode2(wxString in) {
	unsigned char *data = new unsigned char[in.Length()];
	const char* c_in = in.mb_str();
	strcpy((char*) data, c_in);
	std::string result(base64_encode(data, in.Length()));
	delete[] data;
	return result;
}

#endif
