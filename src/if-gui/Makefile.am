if OS_WIN32
if IF_GUI

# http://www.gnu.org/software/hello/manual/automake/Options.html
pkg_verbose = $(pkg_verbose_$(V))
pkg_verbose_ = $(pkg_verbose_$(AM_DEFAULT_VERBOSITY))
pkg_verbose_0 = @echo "  CC     $@";

droidpad_res_CFLAGS = @WXCPPFLAGS@

droidpad.res: droidpad.rc
	$(pkg_verbose)i586-mingw32msvc-windres $(droidpad_res_CFLAGS) droidpad.rc -O coff -o droidpad.res

endif
endif

EXTRA_DIST = droidpad.rc
CLEANFILES = droidpad.res
