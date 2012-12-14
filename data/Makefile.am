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
SUBDIRS = adb

if IF_GUI
  dpdatadir = $(datadir)/droidpad
  dpdata_DATA = layout.xrc iconlarge.png
if OS_LINUX
  dpdata_DATA += iconlarge.xpm
endif
if OS_WIN32
  dpdata_DATA += icon.xpm
endif

docsdir = $(datadir)/droidpad/docs
docs_DATA = docs/intro.zip

  pixmapsdir = $(datadir)/pixmaps
  pixmaps_DATA = droidpad.png

if SPONSOR_MESSAGE
    dpdata_DATA += reactos-logo.png
endif
endif

adbdir = $(datadir)/droidpad/adb
if OS_LINUX
  adb_DATA = adb/adb
endif
if OS_WIN32
  adb_DATA = adb/adb.exe adb/AdbWinApi.dll adb/AdbWinUsbApi.dll
endif

# menudir = $(datadir)/applications
# menu_DATA = droidpad.desktop

# menu_in_files = droidpad.desktop.in
# menu_DATA = $(menu_in_files:.desktop.in=.desktop)
# {at}INTLTOOL_DESKTOP_RULE{at}

EXTRA_DIST = $(dpdata_DATA) $(pixmaps_DATA) $(docs_DATA) layout.xrc icon.ico icon.xpm iconlarge.xpm droidpad.png adb/adb adb/adb.exe adb/AdbWinApi.dll adb/AdbWinUsbApi.dll reactos-logo.png reactos-logo.svg
