AM_MAKEFLAGS = --no-print-directory
SUBDIRS = data src

EXTRA_DIST = configure-mingw

clean-local:
	rm -rf winexport/

MINGW_FOLDER = /usr/i586-mingw32msvc
MINGW_FILES =	\
		$(MINGW_FOLDER)/lib/wxbase28u_gcc_custom.dll		\
		$(MINGW_FOLDER)/lib/wxbase28u_xml_gcc_custom.dll	\
		$(MINGW_FOLDER)/lib/wxmsw28u_adv_gcc_custom.dll		\
		$(MINGW_FOLDER)/lib/wxmsw28u_core_gcc_custom.dll	\
		$(MINGW_FOLDER)/lib/wxmsw28u_html_gcc_custom.dll	\
		$(MINGW_FOLDER)/lib/wxmsw28u_xrc_gcc_custom.dll

winexport: all
	@echo "--- EXPORTING to 'winexport' for windows install ---"
	test -d "winexport" || mkdir winexport
	test -d "winexport/data" || mkdir winexport/data
	cp src/spaceedit.exe winexport/
	cp $(DATA_IMAGES) winexport/data
	cp $(MINGW_FILES) winexport/
