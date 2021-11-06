include $(top_srcdir)/build/vars.build.mk
plugin = tweaks_ui

geanyplugins_LTLIBRARIES = tweaks-ui.la

cf_datadir = $(datadir)/tweaks
cf_data_DATA = \
	../data/tweaks-ui.conf

tweaks_ui_la_SOURCES = \
	tkui_main.cc \
	tkui_main.h \
	tkui_settings.cc \
	tkui_settings.h \
	tkui_column_markers.cc \
	tkui_column_markers.h \
	auxiliary.cc \
	auxiliary.h \
	ao_colortip.c \
	ao_colortip.h \
	ao_markword.c \
	ao_markword.h

tweaks_ui_la_CXXFLAGS = $(AM_CFLAGS) \
	-DG_LOG_DOMAIN=\""Tweaks-UI"\" \
	-DTWEAKS_CONFIG=\""$(cf_datadir)/tweaks-ui.conf"\"

tweaks_ui_la_CFLAGS = $(AM_CFLAGS)
tweaks_ui_la_LIBADD = $(COMMONLIBS)
tweaks_ui_la_LDFLAGS = -module -avoid-version

include $(top_srcdir)/build/cppcheck.mk

