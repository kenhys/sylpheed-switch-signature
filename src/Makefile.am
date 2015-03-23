AUTOMAKE_OPTIONS = subdir-objects

lib_LTLIBRARIES = switch_signatures.la

switch_signatures_la_SOURCES = \
	switch_signatures.c \
	switch_signatures.h

switch_signatures_la_LDFLAGS = -module -no-undefined -avoid-version


AM_CPPFLAGS = \
	$(GTK_CFLAGS) \
	-I$(includedir) \
	-I$(sylpheed_build_dir)/libsylph \
	-I$(sylpheed_build_dir)/src \
	-I$(top_srcdir) \
	-I$(top_srcdir)/src \
	$(SYLPF_CFLAGS)

switch_signatures_la_LIBADD = \
	$(GTK_LIBS) \
	$(sylpheed_build_dir)/src/libsylpheed-plugin-0.la \
	$(sylpheed_build_dir)/libsylph/libsylph-0.la \
	$(SYLPF_LIBS)

install:
install-plugin: install-am
