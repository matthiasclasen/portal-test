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
  GtkWindow *win;

  win = gtk_application_get_active_window (GTK_APPLICATION (app));
  if (!win)
    win = GTK_WINDOW (portal_test_win_new (PORTAL_TEST_APP (app)));
  gtk_window_present (win);
}

static void
portal_test_app_class_init (PortalTestAppClass *class)
{
  G_APPLICATION_CLASS (class)->startup = portal_test_app_startup;
  G_APPLICATION_CLASS (class)->activate = portal_test_app_activate;
}

static void
acktivate (GSimpleAction *aciton,
           GVariant *parameter,
           gpointer user_data)
{
  GtkApplication *app = user_data;
  GtkWindow *win;

  win = gtk_application_get_active_window (app);
  portal_test_win_ack (PORTAL_TEST_WIN (win));
}

static GActionEntry entries[] = {
  { "ack", acktivate, NULL, NULL, NULL }
};

GApplication *
portal_test_app_new (void)
{
  GApplication *app;

  app = g_object_new (portal_test_app_get_type (),
                      "application-id", "org.gnome.PortalTest",
                      NULL);

  g_action_map_add_action_entries (G_ACTION_MAP (app), entries, G_N_ELEMENTS (entries), app);

  return app;
}
