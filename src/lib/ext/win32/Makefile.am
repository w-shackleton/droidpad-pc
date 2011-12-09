#
# This file is part of DroidPad.
# DroidPad lets you use an Android mobile to control a joystick or mouse
# on a Windows or Linux computer.
#
# DroidPad is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# DroidPad is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with DroidPad, in the file COPYING.
# If not, see <http://www.gnu.org/licenses/>.
#

if OS_WIN32

lib_LTLIBRARIES = libppjoy.la

libppjoy_la_SOURCES =	\
			ppjoy/branding.h	\
			ppjoy/ConfigMan.cpp	\
			ppjoy/ConfigMan.h	\
			ppjoy/Debug.cpp		\
			ppjoy/Debug.h		\
			ppjoy/DeviceSetup.cpp	\
			ppjoy/DeviceSetup.h	\
			ppjoy/JoyBus.cpp	\
			ppjoy/JoyBus.h		\
			ppjoy/JoyDefs.cpp	\
			ppjoy/JoyDefs.h		\
			ppjoy/Mapping.cpp	\
			ppjoy/Mapping.h		\
			ppjoy/PPJGUID.h		\
			ppjoy/PPJIoctl.h	\
			ppjoy/PPJoyAPI.cpp	\
			ppjoy/PPJoyAPI.h	\
			ppjoy/PPJType.h		\
			ppjoy/README		\
			ppjoy/stdafx.h		\
			ppjoy/targetver.h	\
						\
			ppjoy/winheader/winioctl.h

# no warnings is because PPJoy makes LOTS of warnings.
libppjoy_la_CXXFLAGS = -Ippjoy -I/usr/i586-mingw32msvc/include/ddk -w

endif
