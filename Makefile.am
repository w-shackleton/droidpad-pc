AM_MAKEFLAGS = --no-print-directory
SUBDIRS = data src

ACLOCAL_AMFLAGS = -I m4

EXTRA_DIST = configure-mingw

clean-local:
	rm -rf winexport/

DPDATA =
PIXMAPS =
if IF_GUI
DPDATA += data/layout.xrc

if OS_WIN32
DPDATA += data/icon.xpm
endif
if OS_LINUX
DPDATA += data/iconlarge.xpm
endif
endif

if OS_WIN32
ADB = data/adb/adb.exe data/adb/AdbWinApi.dll data/adb/AdbWinUsbApi.dll
endif
if OS_LINUX
ADB = data/adb/adb
endif

MINGW_FOLDER = /usr/i586-mingw32msvc
MINGW_FILES =	\
		$(MINGW_FOLDER)/lib/wxbase28u_gcc_custom.dll		\
		$(MINGW_FOLDER)/lib/wxbase28u_xml_gcc_custom.dll	\
		$(MINGW_FOLDER)/lib/wxmsw28u_adv_gcc_custom.dll		\
		$(MINGW_FOLDER)/lib/wxmsw28u_core_gcc_custom.dll	\
		$(MINGW_FOLDER)/lib/wxmsw28u_html_gcc_custom.dll	\
		$(MINGW_FOLDER)/lib/wxmsw28u_xrc_gcc_custom.dll		\
		$(MINGW_FOLDER)/bin/mingwm10.dll			\
		$(MINGW_FOLDER)/bin/zlib1.dll

winexport: all
	@echo "--- EXPORTING to 'winexport' for windows install ---"
	test -d "winexport" || mkdir winexport
	test -d "winexport/data" || mkdir winexport/data
	test -d "winexport/data/adb" || mkdir winexport/data/adb
	cp src/droidpad.exe winexport/
	cp $(DPDATA) winexport/data
	cp $(ADB) winexport/data/adb
	cp $(MINGW_FILES) winexport/
