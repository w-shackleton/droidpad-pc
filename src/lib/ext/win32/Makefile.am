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

lib_LTLIBRARIES = libvjoy.la

libvjoy_la_SOURCES =	\
	vjoy/vJoyInstallLib.cpp	vjoy/vJoyInstall.h	\
	vjoy/stdafx.h		vjoy/public.h		\
	vjoy/targetver.h	vJoy/winheader/salfix.h	\
	winUtil.cpp		winUtil.hpp		\
	vjoy.cpp		vjoy.hpp

libvjoy_la_CXXFLAGS = -Ivjoy -I${prefix}/include/ddk -w @WXCPPFLAGS@ -fpermissive
libvjoy_la_LIBADD = @WXBASELIBS@

if OS_64BIT
# libvjoy_la_LDFLAGS = -static-libgcc
endif

endif
