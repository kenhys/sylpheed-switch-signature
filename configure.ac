#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.68])
m4_define(SS_MAJOR, 0)
m4_define(SS_MINOR, 3)
m4_define(SS_MICRO, 1)
m4_define(SS_VERSION, [SS_MAJOR.SS_MINOR.SS_MICRO])
AC_INIT([switch_signatures], [SS_VERSION], [kenhys@gmail.com])
AC_CONFIG_AUX_DIR([config])
AC_CONFIG_SRCDIR([src/switch_signatures.h])
AC_CONFIG_HEADERS([src/config.h])
AC_CONFIG_MACRO_DIR([m4])

AC_SUBST(MAJOR_VERSION, [SS_MAJOR])
AC_SUBST(MINOR_VERSION, [SS_MINOR])
AC_SUBST(MICRO_VERSION, [SS_MICRO])
m4_define(SS_REVISION, m4_include(version))
AC_SUBST(BUILD_REVISION, [SS_REVISION])

AM_INIT_AUTOMAKE([foreign])

AC_DEFINE([SYLPF_ID], [PACKAGE_NAME], [""])
AC_DEFINE([SYLPF_OPTION], [switch_signatures_option], [""])

# Checks for programs.
AC_PROG_AWK
AC_PROG_CC
AC_PROG_MAKE_SET
AC_PROG_LIBTOOL

IT_PROG_INTLTOOL
# Checks for libraries.
AM_PATH_GLIB_2_0(2.8.0,, AC_MSG_ERROR(Test for GLib failed. See the 'INSTALL' for help.))
AM_PATH_GTK_2_0(2.10.14,, AC_MSG_ERROR(Test for Gtk failed. See the 'INSTALL' for help.))

AC_SUBST(GTK_CFLAGS)
AC_SUBST(GTK_LIBS)

ALL_LINGUAS="en ja"
GETTEXT_PACKAGE=switch_signatures
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE,"$GETTEXT_PACKAGE", [Define text domain.])

dnl AM_GLIB_GNU_GETTEXT
AM_GNU_GETTEXT([external])

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

# Checks for debug
AC_ARG_ENABLE(debug,
              [AS_HELP_STRING([--enable-debug],
                              [use debug flags (default=no)])],
  [plugin_debug="$enableval"],
  [plugin_debug="no"])
if test "x$plugin_debug" != "xno"; then
  plugin_debug="yes"
  if test "$CLANG" = "yes"; then
    CFLAGS="$CFLAGS -O0 -g"
    CXXFLAGS="$CXXFLAGS -O0 -g"
  elif test "$GCC" = "yes"; then
    CFLAGS="$CFLAGS -O0 -g3"
    CXXFLAGS="$CXXFLAGS -O0 -g3"
  fi
fi
AC_SUBST(plugin_debug)

m4_include(macros/sylplugin_factory.m4)

PKG_CHECK_MODULES([GLIB], [glib-2.0])
PKG_CHECK_MODULES([GTK], [gtk+-2.0])
PKG_CHECK_MODULES([SYLPF], [sylplugin-factory])

AC_CONFIG_FILES([Makefile
                 po/Makefile.in
                 src/Makefile
                 res/version.rc
])
AC_OUTPUT

dnl Output the configuration summary
echo ""
echo "$PACKAGE $VERSION"
echo ""
echo "Sylpheed build directory : $sylpheed_build_dir"
echo ""
echo "The binary will be installed in $sylplugin_dir"
echo ""
echo "Configure finished, type 'make' to build."

