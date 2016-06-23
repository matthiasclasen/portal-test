#include <gtk/gtk.h>

#include "portal-test-app.h"
#include "portal-test-win.h"

struct _PortalTestApp
{
  GtkApplication parent;
};

struct _PortalTestAppClass
{
  GtkApplicationClass parent_class;
};

G_DEFINE_TYPE(PortalTestApp, portal_test_app, GTK_TYPE_APPLICATION)

static void
portal_test_app_init (PortalTestApp *app)
{
}

static void
portal_test_app_startup (GApplication *app)
{
  G_APPLICATION_CLASS (portal_test_app_parent_class)->startup (app);
}

static void
portal_test_app_activate (GApplication *app)
{
  GtkApplicationWindow *win;

  win = portal_test_win_new (PORTAL_TEST_APP (app));
  gtk_window_present (GTK_WINDOW (win));
}

static void
portal_test_app_class_init (PortalTestAppClass *class)
{
  G_APPLICATION_CLASS (class)->startup = portal_test_app_startup;
  G_APPLICATION_CLASS (class)->activate = portal_test_app_activate;
}

GApplication *
portal_test_app_new (void)
{
  return g_object_new (portal_test_app_get_type (), NULL);
}
