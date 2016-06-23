#include <gtk/gtk.h>
#include <gio/gdesktopappinfo.h>

#include "portal-test-app.h"
#include "portal-test-win.h"

struct _PortalTestWin
{
  GtkApplicationWindow parent;

  GtkWidget *sandbox_status;
  GtkWidget *network_status;
  GtkWidget *monitor_name;

  GNetworkMonitor *monitor;
};

struct _PortalTestWinClass
{
  GtkApplicationWindowClass parent_class;
};

G_DEFINE_TYPE (PortalTestWin, portal_test_win, GTK_TYPE_APPLICATION_WINDOW);

static void
update_network_status (PortalTestWin *win)
{
  g_autoptr(GString) s = g_string_new ("");
  GEnumClass *class;
  GEnumValue *value;

  if (g_network_monitor_get_network_available (win->monitor))
    g_string_append (s, "available");
  if (g_network_monitor_get_network_metered (win->monitor))
    {
      if (s->len > 0)
        g_string_append (s, ", ");
      g_string_append (s, "metered");
    }
  class = g_type_class_ref (G_TYPE_NETWORK_CONNECTIVITY);
  value = g_enum_get_value (class, g_network_monitor_get_connectivity (win->monitor));
  if (s->len > 0)
    g_string_append (s, ", ");
  g_string_append_printf (s, "connectivity=%s", value->value_nick);
  g_type_class_unref (class);

  gtk_label_set_label (GTK_LABEL (win->network_status), s->str);
}

static void
portal_test_win_init (PortalTestWin *win)
{
  const char *status;
  g_autofree char *name = NULL;

  gtk_widget_init_template (GTK_WIDGET (win));

  if (g_file_test ("/run/user/1000/flatpak-info", G_FILE_TEST_EXISTS))
    status = "confined";
  else
    status = "unconfined";
  gtk_label_set_label (GTK_LABEL (win->sandbox_status), status);

  win->monitor = g_network_monitor_get_default ();
  name = g_strdup_printf ("(%s)", G_OBJECT_TYPE_NAME (win->monitor));
  gtk_label_set_label (GTK_LABEL (win->monitor_name), name);
  g_signal_connect_swapped (win->monitor, "notify", G_CALLBACK (update_network_status), win);
  update_network_status (win);
}

static gboolean
activate_link (GtkLinkButton *button)
{
  GList uris;
  g_autoptr(GAppInfo) app = NULL;

  app = (GAppInfo *)g_desktop_app_info_new ("firefox.desktop");
  uris.data = (gpointer)gtk_link_button_get_uri (button);
  uris.next = NULL;

  g_app_info_launch_uris (app, &uris, NULL, NULL);
  return TRUE;
}

static void
save_dialog (GtkWidget *button)
{
  gint res;
  GtkFileChooserNative *dialog;
  GtkWindow *parent;

  parent = GTK_WINDOW (gtk_widget_get_toplevel (button));
  dialog = gtk_file_chooser_native_new ("File Chooser Portal",
                                        parent,
                                        GTK_FILE_CHOOSER_ACTION_SAVE,
                                        "_Save",
                                        "_Cancel");

  res = gtk_native_dialog_run (GTK_NATIVE_DIALOG (dialog));
  g_print ("Saving file / Response: %d\n", res);
  if (res == GTK_RESPONSE_OK)
    {
      char *filename;
      GtkFileChooser *chooser = GTK_FILE_CHOOSER (dialog);
      filename = gtk_file_chooser_get_filename (chooser);
      g_print ("Saving file: %s\n", filename);
      g_free (filename);
    }

  g_object_unref (dialog);
}

static void
portal_test_win_class_init (PortalTestWinClass *class)
{
  gtk_widget_class_set_template_from_resource (GTK_WIDGET_CLASS (class),
                                               "/org/gtk/portal-test/portal-test-win.ui");
  gtk_widget_class_bind_template_callback (GTK_WIDGET_CLASS (class), activate_link);
  gtk_widget_class_bind_template_callback (GTK_WIDGET_CLASS (class), save_dialog);
  gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), PortalTestWin, sandbox_status);
  gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), PortalTestWin, network_status);
  gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), PortalTestWin, monitor_name);
}

GtkApplicationWindow *
portal_test_win_new (PortalTestApp *app)
{
  return g_object_new (portal_test_win_get_type (), "application", app, NULL);
}
