AM_CPPFLAGS = -DPKGDATADIR=\"$(pkgdatadir)\" -DG_LOG_DOMAIN=\"PortalTest\"
BUILT_SOURCES =
EXTRA_DIST =

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

dbus_built_sources = xdg-desktop-portal-dbus.c xdg-desktop-portal-dbus.h
BUILT_SOURCES += $(dbus_built_sources)

$(dbus_built_sources): Makefile.am
	$(AM_V_GEN) $(GDBUS_CODEGEN)                    			\
		--interface-prefix org.freedesktop.portal. 			\
		--c-namespace Xdp                       			\
		--generate-c-code xdg-desktop-portal-dbus               	\
		$(srcdir)/org.freedesktop.portal.Screenshot.xml 		\
		$(NULL)

bin_PROGRAMS = portal-test
portal_test_CFLAGS = $(PORTAL_TEST_CFLAGS)
portal_test_LDADD = $(PORTAL_TEST_LIBS)
portal_test_SOURCES = 					\
	main.c						\
	portal-test-app.h				\
	portal-test-app.c				\
	portal-test-win.h				\
	portal-test-win.c				\
	xdg-desktop-portal-dbus.h			\
	xdg-desktop-portal-dbus.c			\
	resources.h					\
	resources.c

pkgdata_DATA = test.txt

desktopdir = $(datadir)/applications
desktop_DATA = org.gnome.PortalTest.desktop

dbusservicedir = $(datadir)/dbus-1/services
dbusservice_DATA = org.gnome.PortalTest.service

systemduserunitdir = $(prefix)/lib/systemd/user
systemduserunit_DATA = portal-test.service
