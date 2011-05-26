SUBDIRS = if-gui lib
bin_PROGRAMS = droidpad

SRC_IF_GUI = \
		   if-gui/droidFrame.cpp	if-gui/droidFrame.hpp	\
		   if-gui/droidApp.cpp		if-gui/droidApp.hpp
droidpad_SOURCES =

if IF_GUI
  droidpad_SOURCES += $(SRC_IF_GUI)
  droidpad_LDADD = @WXLIBS@
  droidpad_CXXFLAGS = @WXCPPFLAGS@
else
  droidpad_LDADD = @WXBASELIBS@
  droidpad_CXXFLAGS = @WXBASECPPFLAGS@
endif

droidpad_CXXFLAGS += -Ilib
droidpad_LDADD += lib/libdroidpad.la


AM_CPPFLAGS = -DPREFIX='"$(prefix)"'

if OS_WIN32
if IF_GUI
droidpad_LDADD += if-gui/droidpad.res
droidpad_LDFLAGS = -Wl,--enable-auto-import
endif
endif
