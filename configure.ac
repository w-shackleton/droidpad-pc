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
AC_PREREQ([2.64])
AC_INIT([droidpad], [1.9-beta], [w.shackleton+droidpad@gmail.com])
AC_CONFIG_SRCDIR([src/if-gui/droidApp.cpp])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE

AH_TEMPLATE(VERSION_CODE, "Version code")
AC_DEFINE_UNQUOTED(VERSION_CODE, 193)

AH_TEMPLATE(V_MAJOR, "Major number")
AC_DEFINE_UNQUOTED(V_MAJOR, 1)

AH_TEMPLATE(V_MINOR, "Minor number")
AC_DEFINE_UNQUOTED(V_MINOR, 9)

AH_TEMPLATE(V_REVISION, "Revision")
AC_DEFINE_UNQUOTED(V_REVISION, 3)

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

if test "x$enable_debug" = "xyes"; then
	CFLAGS="$CFLAGS -g -O0 -DDEBUG"
	CXXFLAGS="$CXXFLAGS -O0 -g -DDEBUG"
	AC_MSG_RESULT(yes)
else
	AC_MSG_RESULT(no)
fi

AC_MSG_CHECKING(whether to display sponsor message)
dnl This is only used on MSW
sponsor_default="no"
AC_ARG_ENABLE(sponsor-message, [  --enable-sponsor-message=[no/yes] show sponsor message on about page
	                             [default=$sponsor_default]],, enable_sponsor_message=$sponsor_default)

if test "x$enable_sponsor_message" = "xyes"; then
	CFLAGS="$CFLAGS -DSPONSOR_MESSAGE"
	CXXFLAGS="$CXXFLAGS -DSPONSOR_MESSAGE"
	AC_MSG_RESULT(yes)
else
	AC_MSG_RESULT(no)
fi
AM_CONDITIONAL([SPONSOR_MESSAGE], [test "$enable_sponsor_message" = "yes"])


dnl NOTE: Test mode support is NOT YET FINISHED. This is only included so that the code isn't deleted if it is ever needed again.
testmode_default="no"
AC_MSG_CHECKING(whether to enable windows test mode)
AC_ARG_ENABLE(testmode, [  --enable-testmode=[no/yes] enable windows testmode support
	                             [default=$testmode_default]],, enable_testmode=$testmode_default)

if test "x$enable_testmode" = "xyes"; then
	CFLAGS="$CFLAGS -DMSW_TESTMODE"
	CXXFLAGS="$CXXFLAGS -DMSW_TESTMODE"
	AC_MSG_RESULT(yes)
	echo "*************************************"
	echo "    ****    WARNING: Test mode support is currently not finished."
	echo "    ****    This option is only here if someone else wants to write a branch of DP which"
	echo "    ****    requires test signed drivers."
	echo "*************************************"
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

AM_PATH_WXCONFIG($reqwx, wxWin=1, wxWin=0, [base net])
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

case "$host" in
	*x86_64*)
		bits="64bit"
		;;
	*)
		bits=""
		;;
esac

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

AM_CONDITIONAL([OS_64BIT], [test $bits = "64bit"])
AM_CONDITIONAL([MSW_TESTMODE], [test $enable_testmode = "yes"])

# TODO: Change this to a variable, to change the interface
iface="gui"
AM_CONDITIONAL([IF_GUI], [test $iface = "gui"])

# Checks for library functions.
AC_CONFIG_FILES([
Makefile
data/Makefile
data/adb/Makefile
src/Makefile
src/lib/ext/win32/Makefile
src/lib/Makefile
src/lib/usb/Makefile
src/lib/usb/all/Makefile
])
AC_OUTPUT
