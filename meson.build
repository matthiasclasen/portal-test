project('test app','c', version: '1')

conf_data = configuration_data()
conf_data.set('G_LOG_DOMAIN', '"PortalTest"')
conf_data.set('PACKAGE_NAME', '"portal-test"')
conf_data.set('PKGDATADIR', '"' + get_option('prefix') + '/' + get_option('datadir') + '/portal-test"')
configure_file(output : 'config.h', configuration : conf_data)

gnome = import('gnome')

gtk_dep = dependency('gtk+-3.0')
gst_dep = dependency('gstreamer-audio-1.0')

resources = gnome.compile_resources('resources',
                                    'portal-test.gresource.xml',
                                    c_name: '_portal_test',
                                    source_dir: '.')
dbus = gnome.gdbus_codegen('xdg-desktop-portal-dbus',
                           'org.freedesktop.portal.Screenshot.xml',
                           interface_prefix: 'org.freedesktop.portal.',
                           namespace: 'Xdp')

src = ['main.c',
       'portal-test-app.h',
       'portal-test-app.c',
       'portal-test-win.h',
       'portal-test-win.c',
       resources,
       dbus]

executable('portal-test',
           src,
           dependencies: [gtk_dep, gst_dep])
