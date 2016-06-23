AC_INIT([portal-test], [1])
AC_CONFIG_MACRO_DIR([m4])

AC_PROG_CC

LT_INIT([disable-static])

AC_CONFIG_SRCDIR([./main.c])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE([1.11 no-dist-gzip dist-xz foreign])

AM_SILENT_RULES([yes])
AM_MAINTAINER_MODE([enable])

PKG_PROG_PKG_CONFIG([0.24])

PKG_CHECK_MODULES([PORTAL_TEST], [gtk+-3.0])

AC_CONFIG_FILES([
Makefile
])
AC_OUTPUT
