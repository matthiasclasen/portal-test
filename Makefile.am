BUILT_SOURCES =
EXTRA_DIST =

GLIB_COMPILE_RESOURCES = glib-compile-resources

resource_files = $(shell $(GLIB_COMPILE_RESOURCES)  --sourcedir=$(srcdir) --generate-dependencies $(srcdir)/portal-test.gresource.xml)
EXTRA_DIST += $(resource_files)

resources.h: portal-test.gresource.xml $(resource_files)
	$(AM_V_GEN) $(GLIB_COMPILE_RESOURCES) $< 	\
		--target=$@ 				\
		--sourcedir=$(srcdir) 			\
		--c-name _portal_test 			\
		--generate-header

resources.c: portal-test.gresource.xml $(resource_files)
	$(AM_V_GEN) $(GLIB_COMPILE_RESOURCES) $< 	\
		--target=$@ 				\
		--sourcedir=$(srcdir) 			\
		--c-name _portal_test 			\
		--generate-source

BUILT_SOURCES += $(resource_files)

bin_PROGRAMS = portal-test
portal_test_CFLAGS = $(PORTAL_TEST_CFLAGS)
portal_test_LDADD = $(PORTAL_TEST_LIBS)
portal_test_SOURCES = 					\
	main.c						\
	portal-test-app.h				\
	portal-test-app.c				\
	portal-test-win.h				\
	portal-test-win.c				\
	resources.h					\
	resources.c

desktopdir = $(datadir)/applications
desktop_DATA = org.gnome.PortalTest.desktop
