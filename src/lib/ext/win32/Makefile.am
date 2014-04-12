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

lib_LTLIBRARIES = libjs.la

libjs_la_SOURCES =	\
	js/vJoyInstallLib.cpp	js/vJoyInstall.h	\
	js/stdafx.h		js/defs.h		\
	js/targetver.h		js/winheader/salfix.h	\
	winUtil.cpp		winUtil.hpp		\
	jsDriver.cpp		jsDriver.hpp

libjs_la_CXXFLAGS = -Ijs -I${prefix}/include/ddk -w @WXCPPFLAGS@ -fpermissive
libjs_la_LIBADD = @WXBASELIBS@

AM_CPPFLAGS = -DOS_WIN32

if OS_64BIT
# libjs_la_LDFLAGS = -static-libgcc
AM_CPPFLAGS += -DOS_64BIT
endif
endif
