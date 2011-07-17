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
AM_MAKEFLAGS = --no-print-directory
SUBDIRS = data src

ACLOCAL_AMFLAGS = -I m4

EXTRA_DIST = configure-mingw

clean-local:
	rm -rf winexport/

DPDATA =
PIXMAPS =
if IF_GUI
DPDATA += data/layout.xrc

if OS_WIN32
DPDATA += data/icon.xpm
endif
if OS_LINUX
DPDATA += data/iconlarge.xpm
endif
endif

if OS_WIN32
ADB = data/adb/adb.exe data/adb/AdbWinApi.dll data/adb/AdbWinUsbApi.dll
endif
if OS_LINUX
ADB = data/adb/adb
endif

MINGW_FOLDER = /usr/i586-mingw32msvc
MINGW_FILES =	\
		$(MINGW_FOLDER)/lib/wxbase28u_gcc_custom.dll		\
		$(MINGW_FOLDER)/lib/wxbase28u_xml_gcc_custom.dll	\
		$(MINGW_FOLDER)/lib/wxbase28u_net_gcc_custom.dll	\
		$(MINGW_FOLDER)/lib/wxmsw28u_adv_gcc_custom.dll		\
		$(MINGW_FOLDER)/lib/wxmsw28u_core_gcc_custom.dll	\
		$(MINGW_FOLDER)/lib/wxmsw28u_html_gcc_custom.dll	\
		$(MINGW_FOLDER)/lib/wxmsw28u_xrc_gcc_custom.dll		\
		$(MINGW_FOLDER)/bin/mingwm10.dll			\
		$(MINGW_FOLDER)/bin/zlib1.dll

winexport: all
	@echo "--- EXPORTING to 'winexport' for windows install ---"
	test -d "winexport" || mkdir winexport
	test -d "winexport/data" || mkdir winexport/data
	test -d "winexport/data/adb" || mkdir winexport/data/adb
	cp src/droidpad.exe winexport/
	cp $(DPDATA) winexport/data
	cp $(ADB) winexport/data/adb
	cp $(MINGW_FILES) winexport/
