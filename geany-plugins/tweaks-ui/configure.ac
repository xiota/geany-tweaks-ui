AC_PREREQ([2.71])
dnl Do not forget when updating version number to do GP_CHECK_GEANY also

#AC_INIT (package, version, [bug-report], [tarname], [url])
AC_INIT([tweaks_ui], m4_esyscmd([./version.sh]), [], [], [https://github.com/xiota/tweaks_ui])

AM_INIT_AUTOMAKE([subdir-objects -Wall])
m4_ifdef([AM_SILENT_RULES],[AM_SILENT_RULES([yes])])

AC_CONFIG_SRCDIR([src/tkui_main.cc])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([build/cache])

AC_USE_SYSTEM_EXTENSIONS
AM_PROG_AR
LT_INIT

AC_PROG_CC
AM_PROG_CC_C_O
LT_INIT

AC_PROG_CXX
AC_PROG_CXXCPP
AC_PROG_CXX_C_O

GP_I18N

GP_CHECK_GEANY(1.29)
GP_CHECK_GTK_VERSION
GP_CHECK_CPPCHECK
GP_CHECK_CFLAGS
GP_CHECK_LDFLAGS
GP_CHECK_MINGW

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
GP_STATUS_PRINT
