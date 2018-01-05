# portal-test

A simple test application for [Flatpak](http://www.flatpak.org) portals.

The portal interfaces are defined in [xdg-desktop-portal](https://github.com/flatpak/xdg-desktop-portal).

A GTK+ implementation can be found in [xdg-desktop-portal-gtk](https://github.com/flatpak/xdg-desktop-portal-gtk).

To use this test, use the build script in flatpak/ to produce a flatpak of portal-test, then install it with

    flatpak remote-add --user --no-gpg-verify portal-test file:///path/to/repo
    flatpak install --user portal-test org.gnome.PortalTest

and run it with

    flatpak run org.gnome.PortalTest

The test expects the xdg-desktop-portal service (and a backend, such as xdg-desktop-portal-gtk) to be available on the session bus.

