#include <gtk/gtk.h>

#include "portal-test-app.h"

int
main (int argc, char *argv[])
{
  if (g_file_test ("/run/user/1000/flatpak-info", G_FILE_TEST_EXISTS))
    g_print ("in a sandbox\n");
  return g_application_run (portal_test_app_new (), argc, argv);
}
