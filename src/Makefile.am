AUTOMAKE_OPTIONS = subdir-objects

lib_LTLIBRARIES = switch_signatures.la

switch_signatures_la_SOURCES = \
	switch_signatures.c \
	switch_signatures.h

nodist_switch_signatures_la_SOURCES = \
	$(sylplugin_factory_source_dir)/src/sylplugin_factory.c \
	$(sylplugin_factory_source_dir)/src/sylplugin_factory.h

switch_signatures_la_LDFLAGS = -module -no-undefined -avoid-version


INCLUDES = \
	$(GTK_CFLAGS) \
	-I$(includedir) \
	-I$(sylpheed_build_dir)/libsylph \
	-I$(sylpheed_build_dir)/src \
	-I$(top_srcdir) \
	-I$(top_srcdir)/src \
	-I$(sylplugin_factory_source_dir)/src

switch_signatures_la_LIBADD = \
	$(GTK_LIBS) \
	$(sylpheed_build_dir)/src/libsylpheed-plugin-0.la \
	$(sylpheed_build_dir)/libsylph/libsylph-0.la

install:
install-plugin: install-am
