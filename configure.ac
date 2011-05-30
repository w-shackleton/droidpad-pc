#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.64])
AC_INIT([droidpad], [0.1], [w.shackleton+droidpad@gmail.com])
AC_CONFIG_SRCDIR([src/if-gui/droidApp.cpp])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE

# AC_CONFIG_MACRO_DIR([m4])
AC_CANONICAL_HOST

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
LT_INIT

AM_SILENT_RULES([yes])

AC_CONFIG_MACRO_DIR([m4])

dnl this macro is used to get the arguments supplied
dnl to the configure script (./configure --enable-debug)
dnl Check if we have enable debug support.
AC_MSG_CHECKING(whether to enable debugging)
debug_default="no"
AC_ARG_ENABLE(debug, [  --enable-debug=[no/yes] turn on debugging
	                             [default=$debug_default]],, enable_debug=$debug_default)
dnl Yes, shell scripts can be used
if test "x$enable_debug" = "xyes"; then
	CFLAGS="$CFLAGS -g -DDEBUG"
	CXXFLAGS="$CXXFLAGS -g -DDEBUG"
	AC_MSG_RESULT(yes)
else
	AC_MSG_RESULT(no)
fi

# Checks for libraries.
AM_OPTIONS_WXCONFIG
reqwx=2.8.10
AM_PATH_WXCONFIG($reqwx, wxWin=1, wxWin=0)
if test "$wxWin" != 1; then
	AC_MSG_ERROR([
		wxWidgets must be installed on your system.
 
		Please check that wx-config is in path, the directory
		where wxWidgets libraries are installed (returned by
		'wx-config --libs' or 'wx-config --static --libs' command)
		is in LD_LIBRARY_PATH or equivalent variable and
		wxWidgets version is $reqwx or above.
		])
fi
 
AC_SUBST(WXCPPFLAGS, "$WX_CPPFLAGS")
AC_SUBST(WXCXXFLAGS, "$WX_CXXFLAGS_ONLY")
AC_SUBST(WXCFLAGS, "$WX_CFLAGS_ONLY")
AC_SUBST(WXLIBS, "$WX_LIBS")

AM_PATH_WXCONFIG($reqwx, wxWin=1, wxWin=0, base)
if test "$wxWin" != 1; then
	AC_MSG_ERROR([
		wxWidgets must be installed on your system.
 
		Please check that wx-config is in path, the directory
		where wxWidgets libraries are installed (returned by
		'wx-config --libs' or 'wx-config --static --libs' command)
		is in LD_LIBRARY_PATH or equivalent variable and
		wxWidgets version is $reqwx or above.
		])
fi

AC_SUBST(WXBASECPPFLAGS, "$WX_CPPFLAGS")
AC_SUBST(WXBASECXXFLAGS, "$WX_CXXFLAGS_ONLY")
AC_SUBST(WXBASECFLAGS, "$WX_CFLAGS_ONLY")
AC_SUBST(WXBASELIBS, "$WX_LIBS")

# Check for Windows (for res)
case "$host" in
	*mingw*)
		os="win32"
		;;
	*cygwin*)
		os="win32"
		;;
	*linux*)
		os="linux"
		;;
esac
AM_CONDITIONAL([OS_WIN32], [test $os = "win32"])
AM_CONDITIONAL([OS_LINUX], [test $os = "linux"])
AM_CONDITIONAL([OS_UNIX], [test $os = "linux"])

# TODO: Change this to a variable, to change the interface
iface="gui"
AM_CONDITIONAL([IF_GUI], [test $iface = "gui"])

# Checks for library functions.
AC_CONFIG_FILES([
Makefile
data/Makefile
data/adb/Makefile
src/Makefile
src/lib/Makefile
src/lib/usb/Makefile
src/lib/usb/all/Makefile
])
AC_OUTPUT
