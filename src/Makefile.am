bin_PROGRAMS = droidpad

droidpad_SOURCES = \
		   misc/data.cpp	misc/data.hpp	\
		   droidFrame.cpp	droidFrame.hpp	\
		   droidApp.cpp		droidApp.hpp

droidpad_LDADD = @WX_LIBS@
droidpad_CXXFLAGS = @WXCPPFLAGS@

AM_CPPFLAGS = -DPREFIX='"$(prefix)"'

if OS_WIN32

# http://www.gnu.org/software/hello/manual/automake/Options.html
pkg_verbose = $(pkg_verbose_$(V))
pkg_verbose_ = $(pkg_verbose_$(AM_DEFAULT_VERBOSITY))
pkg_verbose_0 = @echo "  CC     $@";

droidpad.res: droidpad.rc
	$(pkg_verbose)i586-mingw32msvc-windres droidpad.rc -O coff -o droidpad.res

droidpad_LDADD += droidpad.res
droidpad_LDFLAGS = -Wl,--enable-auto-import
endif

EXTRA_DIST = droidpad.rc
CLEANFILES = droidpad.res
