#ifndef _BASE_64_H
#define _BASE_64_H

#include <string>
#include <algorithm>

#include <wx/string.h>
//#include "ext/hexdump.h"

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
	wxCharBuffer c_in = in.mb_str();
	for(int i = 0; i < in.size(); i++)
		data[i] = c_in[i];
//	hexdump(data, in.Length());
	std::string result(base64_encode(data, in.Length()));
	delete[] data;
	return result;
}

/**
 * Encodes as URI-safe B64.
 */
inline std::string base64_encode2uri(wxString in) {
	std::string result = base64_encode2(in);
	std::replace(result.begin(), result.end(), '/', '-');
	return result;
}

/**
 * Encodes as URI-safe B64.
 */
inline std::string base64_encode_uri(unsigned char const* data, unsigned int len) {
	std::string result = base64_encode(data, len);
	std::replace(result.begin(), result.end(), '/', '-');
	return result;
}

#endif
