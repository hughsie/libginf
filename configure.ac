AC_PREREQ(2.63)

m4_define([ginf_major_version], [0])
m4_define([ginf_minor_version], [1])
m4_define([ginf_micro_version], [0])
m4_define([as_version],
          [ginf_major_version.ginf_minor_version.ginf_micro_version])

AC_INIT([libginf],[as_version])
AC_CONFIG_SRCDIR(libginf)
AM_INIT_AUTOMAKE([1.9 no-dist-gzip dist-xz tar-ustar foreign])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])
AC_SYS_LARGEFILE

GOBJECT_INTROSPECTION_CHECK([0.9.8])

GINF_MAJOR_VERSION=ginf_major_version
GINF_MINOR_VERSION=ginf_minor_version
GINF_MICRO_VERSION=ginf_micro_version
GINF_VERSION=as_version
AC_SUBST(GINF_MAJOR_VERSION)
AC_SUBST(GINF_MINOR_VERSION)
AC_SUBST(GINF_MICRO_VERSION)
AC_SUBST(GINF_VERSION)

# enable nice build output on automake1.11
m4_ifdef([AM_SILENT_RULES],[AM_SILENT_RULES([yes])])

# libtool versioning - this applies to libginf
#
# See http://sources.redhat.com/autobook/autobook/autobook_91.html#SEC91 for details
#
# - If interfaces have been changed or added, but binary compatibility
#   has been preserved, change:
#      CURRENT += 1
#      REVISION = 0
#      AGE += 1
# - If binary compatibility has been broken (eg removed or changed
#   interfaces), change:
#      CURRENT += 1
#      REVISION = 0
#      AGE = 0
# - If the interface is the same as the previous version, but bugs are
#   fixed, change:
#      REVISION += 1
LT_CURRENT=1
LT_REVISION=0
LT_AGE=0
AC_SUBST(LT_CURRENT)
AC_SUBST(LT_REVISION)
AC_SUBST(LT_AGE)

AC_PROG_CC
AC_PROG_INSTALL
AC_HEADER_STDC
LT_INIT
AM_PROG_CC_C_O

GNOME_COMPILE_WARNINGS([maximum],[
	-Wmissing-declarations
	-Wcast-align
	-Wwrite-strings
	-Wreturn-type
	-Wformat-nonliteral
	-Wmissing-format-attribute
	-Wclobbered
	-Wempty-body
	-Wignored-qualifiers
	-Wsign-compare
	-Wtype-limits
	-Wuninitialized
	-Waggregate-return
	-Wdeclaration-after-statement
	-Wshadow
	-Wno-strict-aliasing
	-Winline
	-Wmissing-parameter-type
	-Woverride-init
	-Wno-discarded-qualifiers
	-Wconversion
	-Wformat-signedness
])

# check for PIE (position independent executable) support
AX_CHECK_COMPILE_FLAG([-fPIE],
		      [AX_CHECK_LINK_FLAG([-fPIE -pie],
					  [PIE_CFLAGS="-fPIE" PIE_LDFLAGS="-pie"])])
AC_SUBST(PIE_CFLAGS)
AC_SUBST(PIE_LDFLAGS)

PKG_CHECK_MODULES(GLIB, glib-2.0 >= 2.45.8 gio-2.0 gobject-2.0 gthread-2.0 gio-unix-2.0 gmodule-2.0)

AC_CONFIG_FILES([
Makefile
libginf/Makefile
libginf/libginf.pc
libginf/ginf-version.h
data/Makefile
data/tests/Makefile
])
AC_OUTPUT
AC_MSG_RESULT([
    $PACKAGE_NAME $VERSION

    prefix:                  ${prefix}
    sysconf dir:             ${sysconfdir}
    datarootdir:             ${datarootdir}
    includedir:              ${includedir}
    lib dir:                 ${libdir}
              ])
