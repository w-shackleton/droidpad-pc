#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.64])
AC_INIT([droidpad], [0.1], [w.shackleton+droidpad@gmail.com])
AC_CONFIG_SRCDIR([src/droidApp.cpp])
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

# Checks for libraries.
AM_OPTIONS_WXCONFIG
reqwx=2.8.10
AM_PATH_WXCONFIG($reqwx, wxWin=1)
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
 
#CPPFLAGS="$CPPFLAGS $WX_CPPFLAGS"
AC_SUBST(WXCPPFLAGS, "$WX_CPPFLAGS")
#CXXFLAGS="$CXXFLAGS $WX_CXXFLAGS_ONLY"
AC_SUBST(WXCXXFLAGS, "$WX_CXXFLAGS_ONLY")
#CFLAGS="$CFLAGS $WX_CFLAGS_ONLY"
AC_SUBST(WXCFLAGS, "$WX_CFLAGS_ONLY")
#LIBS="$LIBS $WX_LIBS"
AC_SUBST(WXLIBS, "$WX_LIBS")

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

# Checks for library functions.
AC_CONFIG_FILES([
Makefile
data/Makefile
src/Makefile
])
AC_OUTPUT
