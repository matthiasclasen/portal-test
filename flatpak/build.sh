#!/bin/sh

flatpak-builder --force-clean --ccache --install --repo=repo app org.gnome.PortalTest.json
