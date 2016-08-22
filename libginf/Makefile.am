if HAVE_INTROSPECTION
-include $(INTROSPECTION_MAKEFILE)
INTROSPECTION_SCANNER_ARGS = --add-include-path=$(srcdir)
INTROSPECTION_COMPILER_ARGS = --includedir=$(srcdir)
endif

AM_CPPFLAGS =							\
	$(GLIB_CFLAGS)						\
	-I$(top_srcdir)/libginf					\
	-I$(top_builddir)/libginf				\
	-I.							\
	-DGINF_COMPILATION					\
	-DTESTDIRSRC=\""$(top_srcdir)/data/tests"\"		\
	-DTESTDIRBUILD=\""$(top_builddir)/data/tests"\"		\
	-DLOCALSTATEDIR=\""$(localstatedir)"\"			\
	-DG_LOG_DOMAIN=\"GInf\"

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = 						\
	libginf.pc

lib_LTLIBRARIES =						\
	libginf.la

libginf_includedir = $(includedir)/libginf
libginf_include_HEADERS =					\
	libginf.h						\
	ginf-common.h						\
	ginf-version.h

nodist_libginf_la_SOURCES =
libginf_la_SOURCES =						\
	ginf-common.c						\
	ginf-common.h						\
	ginf-version.h

libginf_la_LIBADD =						\
	$(GLIB_LIBS)

libginf_la_LDFLAGS =						\
	-version-info $(LT_CURRENT):$(LT_REVISION):$(LT_AGE)	\
	-export-dynamic						\
	-no-undefined						\
	-export-symbols-regex '^ginf_.*'

libginf_la_CFLAGS =						\
	$(WARN_CFLAGS)

check_PROGRAMS =						\
	ginf-self-test
ginf_self_test_SOURCES =					\
	ginf-self-test.c
ginf_self_test_LDADD =						\
	$(GLIB_LIBS)						\
	$(lib_LTLIBRARIES)
ginf_self_test_CFLAGS = $(WARN_CFLAGS)

TESTS = ginf-self-test

if HAVE_INTROSPECTION
introspection_sources =						\
	ginf-common.c						\
	ginf-common.h						\
	ginf-version.h

GInf-1.0.gir: libginf.la
GInf_1_0_gir_INCLUDES = GObject-2.0 Gio-2.0
GInf_1_0_gir_CFLAGS = $(AM_CPPFLAGS)
GInf_1_0_gir_SCANNERFLAGS = --identifier-prefix=GInf		\
				--symbol-prefix=ginf_		\
				--warn-all			\
				--add-include-path=$(srcdir)
GInf_1_0_gir_EXPORT_PACKAGES = libginf
GInf_1_0_gir_LIBS = libginf.la
GInf_1_0_gir_FILES = $(introspection_sources)
INTROSPECTION_GIRS = GInf-1.0.gir

girdir = $(datadir)/gir-1.0
gir_DATA = $(INTROSPECTION_GIRS)

typelibdir = $(libdir)/girepository-1.0
typelib_DATA = $(INTROSPECTION_GIRS:.gir=.typelib)

CLEANFILES = $(gir_DATA) $(typelib_DATA)
endif

DISTCLEANFILES =						\
	*.log							\
	*.trs

EXTRA_DIST =							\
	libginf.pc.in						\
	ginf-version.h.in

clean-local:
	rm -f *~
	rm -f $(CLEANFILES)

-include $(top_srcdir)/git.mk
