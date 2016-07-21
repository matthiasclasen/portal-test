#include <gtk/gtk.h>
#include <gst/gst.h>

#include "portal-test-app.h"

int
main (int argc, char *argv[])
{
  gst_init (&argc, &argv);

  g_message ("starting org.gnome.PortalTest");

  return g_application_run (portal_test_app_new (), argc, argv);
}
