#!/bin/sh

flatpak-builder --force-clean --ccache --require-changes --repo=repo app portal-test.json
