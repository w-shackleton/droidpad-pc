#   This file is part of DroidPad.
# 
#   DroidPad is free software: you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation, either version 3 of the License, or
#   (at your option) any later version.
# 
#   DroidPad is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
# 
#   You should have received a copy of the GNU General Public License
#   along with DroidPad.  If not, see <http://www.gnu.org/licenses/>.

SUBDIRS = adb

if IF_GUI
  dpdatadir = $(datadir)/droidpad
  dpdata_DATA = layout.xrc iconlarge.xpm

  pixmapsdir = $(datadir)/pixmaps
  pixmaps_DATA = droidpad.png
endif

# menudir = $(datadir)/applications
# menu_DATA = droidpad.desktop

# menu_in_files = droidpad.desktop.in
# menu_DATA = $(menu_in_files:.desktop.in=.desktop)
# {at}INTLTOOL_DESKTOP_RULE{at}

EXTRA_DIST = $(dpdata_DATA) $(pixmaps_DATA) layout.xrc icon.ico icon.xpm iconlarge.xpm droidpad.png
