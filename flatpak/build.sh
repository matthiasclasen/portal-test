#!/bin/sh

flatpak-builder --force-clean --ccache --require-changes --repo=repo app org.gnome.PortalTest.json
