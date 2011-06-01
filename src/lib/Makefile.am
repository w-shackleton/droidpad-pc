SUBDIRS = usb
lib_LTLIBRARIES = libdroidpad.la

SRC_LINUX =

libdroidpad_la_SOURCES = \
		   include/adb.hpp	\
		   log.hpp		\
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
		   ext/b64/base64.cpp		ext/b64/base64.hpp		\
		   net/mdns.cpp			net/mdns.hpp			\
		   net/deviceDiscover.cpp	net/deviceDiscover.hpp		\
		   net/connection.cpp		net/connection.hpp

if OS_LINUX
  libdroidpad_la_SOURCES += $(SRC_LINUX)
endif

libdroidpad_la_LIBADD = @WXBASELIBS@
libdroidpad_la_CXXFLAGS = @WXCPPFLAGS@ -I. -Iext
libdroidpad_la_CFLAGS = @WXCPPFLAGS@ -I. -Iext

if OS_WIN32
libdroidpad_la_LIBADD += -lws2_32
libdroidpad_la_LDFLAGS = -Wl,--enable-auto-import
endif

AM_CPPFLAGS = -DPREFIX='"$(prefix)"'

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
