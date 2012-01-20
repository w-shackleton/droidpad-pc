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
SUBDIRS = usb ext/win32
lib_LTLIBRARIES = libdroidpad.la

SRC_LINUX =	\
		output/linux/outputMgr.cpp	output/linux/outputMgr.hpp	\
		output/linux/dpinput.c		output/linux/dpinput.h		\
		output/linux/platformSettings.hpp
# Sources for all forms of Windows. Name is misleading (should be SRC_MSW)
SRC_WIN32 =	\
		output/win32/wOutputMgr.cpp	output/win32/wOutputMgr.hpp	\
		output/win32/winOutputs.cpp	output/win32/winOutputs.hpp	\
		output/win32/vJoyOutputs.cpp	output/win32/vJoyOutputs.hpp	\
		output/win32/wPlatformSettings.hpp				\
		msw/winSetup.cpp		msw/winSetup.hpp		\
		msw/updater.cpp			msw/updater.hpp
# *JUST* windows 64-bit.
SRC_WIN64 =	\
		msw/bootConf.cpp		msw/bootConf.hpp

libdroidpad_la_SOURCES = \
		   include/adb.hpp	\
		   include/outputMgr.hpp	\
		   include/platformSettings.hpp	\
		   log.hpp		\
		   types.hpp		\
		   			\
		   proc.cpp			proc.hpp			\
		   data.cpp			data.hpp			\
		   deviceManager.cpp		deviceManager.hpp		\
		   mainThread.cpp		mainThread.hpp			\
		   deviceManagerThreads.cpp	deviceManagerThreads.hpp	\
		   events.cpp			events.hpp			\
		   droidpadCallbacks.cpp	droidpadCallbacks.hpp		\
		   usb/all/adb.cpp		usb/all/adb.hpp			\
		   ext/1035.c			ext/1035.h			\
		   ext/mdnsd.c			ext/mdnsd.h			\
		   ext/md5/md5c.c		ext/md5/md5.h	ext/md5/global.h\
		   ext/b64/base64.cpp		ext/b64/base64.hpp		\
		   net/mdns.cpp			net/mdns.hpp			\
		   net/deviceDiscover.cpp	net/deviceDiscover.hpp		\
		   net/connection.cpp		net/connection.hpp		\
		   output/IOutputMgr.cpp	output/IOutputMgr.hpp		\
		   output/outputSmoothBuffer.cpp output/outputSmoothBuffer.hpp

if OS_LINUX
  libdroidpad_la_SOURCES += $(SRC_LINUX)
endif
if OS_WIN32
  libdroidpad_la_SOURCES += $(SRC_WIN32)
if OS_64BIT
  libdroidpad_la_SOURCES += $(SRC_WIN64)
endif
endif

libdroidpad_la_LIBADD = @WXBASELIBS@
libdroidpad_la_CXXFLAGS = @WXCPPFLAGS@ -I. -Iext
libdroidpad_la_CFLAGS = @WXCPPFLAGS@ -I. -Iext

if OS_WIN32
libdroidpad_la_LIBADD += -lws2_32 ext/win32/libvjoy.la
libdroidpad_la_LDFLAGS = -Wl,--enable-auto-import
libdroidpad_la_CXXFLAGS += -Imsw
libdroidpad_la_CFLAGS += -Imsw

endif

AM_CPPFLAGS = -DPREFIX='"$(prefix)"'

if OS_64BIT
# libdroidpad_la_LDFLAGS += -static-libgcc
AM_CPPFLAGS += -DOS_64BIT
endif

# For all unices
if OS_UNIX
AM_CPPFLAGS += -DOS_UNIX
endif
if OS_LINUX
AM_CPPFLAGS += -DOS_LINUX
endif
if OS_WIN32
AM_CPPFLAGS += -DOS_WIN32
endif
