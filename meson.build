project('test app','c', version: '1', meson_version: '>= 0.36.0')

conf_data = configuration_data()
conf_data.set_quoted('G_LOG_DOMAIN', 'PortalTest')
conf_data.set_quoted('PACKAGE_NAME', 'portal-test')
conf_data.set_quoted('PKGDATADIR', join_paths(get_option('prefix'), get_option('datadir'), 'portal-test'))
configure_file(output : 'config.h', configuration : conf_data)

gnome = import('gnome')

gtk_dep = dependency('gtk+-3.0')
gst_dep = dependency('gstreamer-audio-1.0')

resources = gnome.compile_resources('resources',
                                    'portal-test.gresource.xml',
                                    c_name: '_portal_test')
screenshot = gnome.gdbus_codegen('screenshot-portal',
                            'org.freedesktop.portal.Screenshot.xml',
                            interface_prefix: 'org.freedesktop.portal.',
                            namespace: 'Xdp')
account = gnome.gdbus_codegen('account-portal',
                            'org.freedesktop.portal.Account.xml',
                            interface_prefix: 'org.freedesktop.portal.',
                            namespace: 'Xdp')
email = gnome.gdbus_codegen('email-portal',
                            'org.freedesktop.portal.Email.xml',
                            interface_prefix: 'org.freedesktop.portal.',
                            namespace: 'Xdp')

src = ['main.c',
       'portal-test-app.h',
       'portal-test-app.c',
       'portal-test-win.h',
       'portal-test-win.c',
       resources,
       screenshot,
       account,
       email]

executable('portal-test',
           src,
           dependencies: [gtk_dep, gst_dep],
           install : true)
