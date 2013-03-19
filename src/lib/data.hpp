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
#ifndef _DATA_H
#define _DATA_H

#include <iostream>
#include <vector>
#include <stdint.h>
#include "config.h"

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <wx/string.h>
#include <wx/config.h>

#include "ext/b64/base64.hpp"

#define STD_TO_WX_STRING(_str) wxString(_str.c_str(), wxConvUTF8)
#define NUM_AXIS 6
#define NUM_BUTTONS 12

#define PSK_LEN 32

namespace droidpad
{
	// Describes the tweaks on an accelerometer
	class TiltTweak {
		public:
			// The total sweep-angle, in DEGREES
			int32_t totalAngle;
			// The 'gamma' - controls how sensitive the device is in small amounts.
			// Currently this is -100 to 100 because of wxSlider limitations
			int32_t gamma;
	};

	class RotationTweak {
		public:
			// X Rotation (gyro)
			int32_t totalAngle;
	};

	class OnScreenSliderTweak {
		public:
			// Gamma
			int32_t gamma;
	};

	class Tweaks {
		public:
			// X and Y tilts
			TiltTweak tilt[2];

			// Rotation
			RotationTweak rotation[1];

			// On screen sliders
			OnScreenSliderTweak onScreen[NUM_AXIS];
	};

	// General data storage - serialised to file for persistance
	class Data
	{
		public:
			static bool initialise();
			static wxString datadir;
			static wxString getFilePath(wxString file);

			static wxString version;

			// Custom host and custom port
			static wxString host;
			static int port;

			// Name of this computer (for user to ID against)
			static wxString computerName;
			static boost::uuids::uuid computerUuid;
			static inline std::string computerUuidString() {
				return boost::uuids::to_string(computerUuid);
			}

			static bool secureSupported;

			/**
			 * Maps buttons and axes to where the user wants them.
			 */
			static std::vector<int> buttonOrder;
			static std::vector<int> axisOrder;

			/**
			 * Tweaks
			 */
			static Tweaks tweaks;

			static void savePreferences();
		protected:
			static wxString confLocation;

			static wxConfig *config;

			static Tweaks createDefaultTweaks();

			static void loadPreferences();

			// The count variables declare how many there *should* be. This will pad out if not enough present.
			static std::vector<int> decodeOrderConf(wxString input, int count);
			static wxString encodeOrderConf(std::vector<int> input, int count);
		private:
			Data(); // To stop initialising static class

	};
	
	class Credentials {
		public:
			boost::uuids::uuid deviceId;
			wxString deviceName;
			std::string psk;

			inline std::string deviceIdString() {
				return boost::uuids::to_string(deviceId);
			}

			inline wxString psk64() {
				return STD_TO_WX_STRING(base64_encode((unsigned char * const)psk.c_str(), psk.length()));
			}
			inline std::string psk64_std() {
				return base64_encode((unsigned char * const)psk.c_str(), psk.length());
			}

			inline Credentials(boost::uuids::uuid deviceId, wxString deviceName, std::string psk) :
				deviceId(deviceId),
				deviceName(deviceName),
				psk(psk) { }
			inline Credentials(boost::uuids::uuid deviceId, wxString deviceName, wxString psk64) :
				deviceId(deviceId),
				deviceName(deviceName),
				psk(base64_decode((std::string)psk64.mb_str())) { }
	};
	class CredentialStore {
		friend class Data;
		private:
			static std::vector<Credentials> credentials;

			static boost::random::mt19937 gen;
			static boost::uuids::random_generator uuidGen;
		public:
			// Creates a new set of credentials (without a name)
			// and stores them into the preferences.
			static Credentials createNewSet();

			static inline std::vector<Credentials>::iterator begin() {
				return credentials.begin();
			}
			static inline std::vector<Credentials>::iterator end() {
				return credentials.end();
			}

			inline static size_t size() {
				return credentials.size();
			}
	};
}

#endif
