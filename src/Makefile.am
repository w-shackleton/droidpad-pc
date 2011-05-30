SUBDIRS = lib
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
droidpad_LDFLAGS = -Wl,--enable-auto-import --enable-stdcall-fixup
if IF_GUI
# http://www.gnu.org/software/hello/manual/automake/Options.html
pkg_verbose = $(pkg_verbose_$(V))
pkg_verbose_ = $(pkg_verbose_$(AM_DEFAULT_VERBOSITY))
pkg_verbose_0 = @echo "  RES    $@";

droidpad_res_CFLAGS = @WXCPPFLAGS@

if-gui/droidpad.res: if-gui/droidpad.rc
	$(pkg_verbose)i586-mingw32msvc-windres $(droidpad_res_CFLAGS) if-gui/droidpad.rc -O coff -o if-gui/droidpad.res

droidpad_LDADD += if-gui/droidpad.res -lws2_32

EXTRA_DIST = if-gui/droidpad.rc
CLEANFILES = if-gui/droidpad.res
endif
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
