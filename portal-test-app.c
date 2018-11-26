#include <unistd.h>
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

static gboolean
name_lost_cb (GApplication *app)
{
  g_message ("Name lost, quitting");
  g_application_quit (app);
  return TRUE;
}

static void
portal_test_app_startup (GApplication *app)
{
  g_autoptr(GMenu) menu = NULL;

  G_APPLICATION_CLASS (portal_test_app_parent_class)->startup (app);

  menu = g_menu_new ();
  g_menu_append (menu, "Restart", "app.restart");
  g_menu_append (menu, "Quit", "app.quit");

  gtk_application_set_app_menu (GTK_APPLICATION (app), G_MENU_MODEL (menu));

  g_signal_connect (app, "name-lost", G_CALLBACK (name_lost_cb), NULL);
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
acktivate (GSimpleAction *action,
           GVariant *parameter,
           gpointer user_data)
{
  GtkApplication *app = user_data;
  GtkWindow *win;

  win = gtk_application_get_active_window (app);
  portal_test_win_ack (PORTAL_TEST_WIN (win));
}

static void
spawned (GObject *source,
         GAsyncResult *res,
         gpointer data)
{
  GDBusConnection *bus = G_DBUS_CONNECTION (source);
  g_autoptr(GError) error = NULL;
  g_autoptr(GVariant) ret = NULL;
  guint32 pid;

  ret = g_dbus_connection_call_finish (bus, res, &error);

  if (!ret)
    {
      g_warning ("Restart failed: %s", error->message);
      return;
    }

  g_variant_get (ret, "(u)", &pid);
  g_message ("Restarted with pid %d", pid);
}

void
portal_test_app_restart (PortalTestApp *app)
{
  GDBusConnection *bus;
  const char *strv[3] = { "portal-test", "--gapplication-replace",  NULL };

  bus = g_application_get_dbus_connection (G_APPLICATION (app));

  g_message ("Calling org.freedesktop.portal.Flatpak.Spawn");
  g_dbus_connection_call (bus,
                          "org.freedesktop.portal.Flatpak",
                          "/org/freedesktop/portal/Flatpak",
                          "org.freedesktop.portal.Flatpak",
                          "Spawn",
                          g_variant_new ("(@ay@aay@a{uh}@a{ss}u@a{sv})",
                                         g_variant_new_bytestring ("/"),
                                         g_variant_new_bytestring_array (strv, -1),
                                         g_variant_new_array (G_VARIANT_TYPE ("{uh}"), NULL, 0),
                                         g_variant_new_array (G_VARIANT_TYPE ("{ss}"), NULL, 0),
                                         2,
                                         g_variant_new_array (G_VARIANT_TYPE ("{sv}"), NULL, 0)),
                          G_VARIANT_TYPE ("(u)"),
                          0,
                          -1,
                          NULL,
                          spawned,
                          app);
}

static void
restart (GSimpleAction *action,
         GVariant *parameter,
         gpointer user_data)
{
  g_message ("Received a request to restart");
  portal_test_app_restart (PORTAL_TEST_APP (user_data));
}

static void
quit (GSimpleAction *action,
      GVariant *parameter,
      gpointer user_data)
{
  g_message ("Received a request to quit");
  g_application_quit (G_APPLICATION (user_data));
}

static GActionEntry entries[] = {
  { "ack", acktivate, NULL, NULL, NULL },
  { "restart", restart, NULL, NULL, NULL },
  { "quit", quit, NULL, NULL, NULL }
};

GApplication *
portal_test_app_new (void)
{
  GApplication *app;

  app = g_object_new (portal_test_app_get_type (),
                      "application-id", "org.gnome.PortalTest",
                      "flags", G_APPLICATION_ALLOW_REPLACEMENT,
                      NULL);

  g_action_map_add_action_entries (G_ACTION_MAP (app), entries, G_N_ELEMENTS (entries), app);

  return app;
}
