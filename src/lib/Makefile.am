SUBDIRS = usb
lib_LTLIBRARIES = libdroidpad.la

SRC_LINUX =

libdroidpad_la_SOURCES = \
		   proc.cpp			proc.hpp			\
		   data.cpp			data.hpp			\
		   deviceManager.cpp		deviceManager.hpp		\
		   deviceManagerThreads.cpp	deviceManagerThreads.hpp	\
		   events.cpp			events.hpp			\
		   droidpadCallbacks.cpp	droidpadCallbacks.hpp		\
		   usb/all/adb.cpp		usb/all/adb.hpp			\
		   ext/1035.c			ext/1035.h			\
		   ext/mdnsd.c			ext/mdnsd.h			\
		   ext/b64/base64.cpp		ext/b64/base64.hpp		\
		   net/mdns.cpp			net/mdns.hpp			\
		   net/deviceDiscover.cpp	net/deviceDiscover.hpp

if OS_LINUX
  libdroidpad_la_SOURCES += $(SRC_LINUX)
endif

libdroidpad_la_LIBADD = @WXBASELIBS@
libdroidpad_la_CXXFLAGS = @WXCPPFLAGS@ -I. -Iext
libdroidpad_la_CFLAGS = @WXCPPFLAGS@ -I. -Iext

AM_CPPFLAGS = -DPREFIX='"$(prefix)"'

if OS_LINUX
AM_CPPFLAGS += -DOS_LINUX
endif
if OS_WIN32
AM_CPPFLAGS += -DOS_WIN32
endif
