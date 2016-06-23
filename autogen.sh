#!/bin/sh

srcdir=`dirname $0`
test -z "$srcdir" && srcdir=.

aclocal --install || exit 1
autoreconf --verbose --force --install || exit 1
if [ "$NOCONFIGURE" = "" ]; then
  $srcdir/configure "$@" || exit 1
fi
