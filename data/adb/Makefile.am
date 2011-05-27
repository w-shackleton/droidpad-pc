adbfiledir = $(datadir)/droidpad/adb

if OS_LINUX
adbfile_DATA = adb
endif
if OS_WIN32
adbfile_DATA = adb.exe AdbWinApi.dll AdbWinUsbApi.dll
endif

EXTRA_DIST = adb adb.exe AdbWinApi.dll AdbWinUsbApi.dll
