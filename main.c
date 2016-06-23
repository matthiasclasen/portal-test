#include <gtk/gtk.h>

#include "portal-test-app.h"

int
main (int argc, char *argv[])
{
  return g_application_run (portal_test_app_new (), argc, argv);
}
