#AC_INIT (package, version, [bug-report], [tarname], [url])
AC_INIT([tweaks_ui],[m4_esyscmd(./version.sh)],[],[],[https://github.com/xiota/geany-tweaks-ui])

AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_SRCDIR([src/tkui_main.cc])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE([subdir-objects -Wall -Werror foreign])
AM_SILENT_RULES([yes])
AM_PROG_AR
LT_INIT([disable-static pic-only])

#: ${CXX="clang-13"}
#: ${CXXFLAGS="--std=gnu++17"}
AC_PROG_CXX

PKG_CHECK_MODULES([GEANY], [geany >= 1.23])
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
