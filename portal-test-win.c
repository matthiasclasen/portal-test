#include <gtk/gtk.h>
#include <gio/gdesktopappinfo.h>

#include "portal-test-app.h"
#include "portal-test-win.h"
#include "xdg-desktop-portal-dbus.h"

struct _PortalTestWin
{
  GtkApplicationWindow parent;

  GtkWidget *sandbox_status;
  GtkWidget *network_status;
  GtkWidget *monitor_name;
  GtkWidget *resolver_name;
  GtkWidget *proxies;
  GtkWidget *encoding;

  GNetworkMonitor *monitor;
  GProxyResolver *resolver;

  GtkWidget *image;
  XdpScreenshot *screenshot;
  char *screenshot_handle;
  guint screenshot_response_signal_id;
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
  g_auto(GStrv) proxies = NULL;
  g_autofree char *proxy = NULL;
  g_autofree char *path = NULL;

  gtk_widget_init_template (GTK_WIDGET (win));

  path = g_strdup_printf ("/run/user/%d/flatpak-info", getpid ());
  if (g_file_test (path, G_FILE_TEST_EXISTS))
    status = "confined";
  else
    status = "unconfined";
  gtk_label_set_label (GTK_LABEL (win->sandbox_status), status);

  win->monitor = g_network_monitor_get_default ();
  gtk_label_set_label (GTK_LABEL (win->monitor_name), G_OBJECT_TYPE_NAME (win->monitor));
  g_signal_connect_swapped (win->monitor, "notify", G_CALLBACK (update_network_status), win);
  update_network_status (win);

  win->resolver = g_proxy_resolver_get_default ();
  gtk_label_set_label (GTK_LABEL (win->resolver_name), G_OBJECT_TYPE_NAME (win->resolver));

  proxies = g_proxy_resolver_lookup (win->resolver, "http://www.flatpak.org", NULL, NULL);
  proxy = g_strjoinv (", ", proxies);
  gtk_label_set_label (GTK_LABEL (win->proxies), proxy);

  win->screenshot = xdp_screenshot_proxy_new_for_bus_sync (G_BUS_TYPE_SESSION,
                                                           G_DBUS_PROXY_FLAGS_NONE,
                                                           "org.freedesktop.portal.Desktop",
                                                           "/org/freedesktop/portal/desktop",
                                                           NULL, NULL);
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
save_dialog (GtkWidget *button, PortalTestWin *win)
{
  gint res;
  GtkFileChooserNative *dialog;
  GtkWindow *parent;
  const char *options[] = {
    "current",
    "iso8859-15",
    "utf-16",
    NULL
  };
  const char *labels[] = {
    "Current Locale (UTF-8)",
    "Western (ISO-8859-15)",
    "Unicode (UTF-16)",
    NULL,
  };
  const char *encoding;
  const char *label;
  g_autofree char *text = NULL;
  gboolean canonicalize;
  int i;

  parent = GTK_WINDOW (gtk_widget_get_toplevel (button));
  dialog = gtk_file_chooser_native_new ("File Chooser Portal",
                                        parent,
                                        GTK_FILE_CHOOSER_ACTION_SAVE,
                                        "_Save",
                                        "_Cancel");
  gtk_file_chooser_native_add_choice (dialog,
                                      "encoding", "Character Encoding:",
                                      options, labels);
  gtk_file_chooser_native_set_choice (dialog, "encoding", "current");

  gtk_file_chooser_native_add_option (dialog, "canonicalize", "Canonicalize");
  gtk_file_chooser_native_set_option (dialog, "canonicalize", TRUE);

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

  encoding = gtk_file_chooser_native_get_choice (dialog, "encoding");
  canonicalize = gtk_file_chooser_native_get_option (dialog, "canonicalize");

  label = "";
  for (i = 0; options[i]; i++)
    {
      if (g_strcmp0 (encoding, options[i]) == 0)
        label = labels[i];
    }

  text = g_strdup_printf ("%s%s", label, canonicalize ? " (canon)" : "");
  gtk_label_set_label (GTK_LABEL (win->encoding), text);

  g_object_unref (dialog);
}

static void
screenshot_response (GDBusConnection *connection,
                     const char *sender_name,
                     const char *object_path,
                     const char *interface_name,
                     const char *signal_name,
                     GVariant *parameters,
                     gpointer user_data)
{
  PortalTestWin *win = user_data;
  guint32 response;
  GVariant *options;

  g_variant_get (parameters, "(u@a{sv})", &response, &options);

  if (response == 0)
    {
      g_autoptr(GdkPixbuf) pixbuf = NULL;
      g_autoptr(GError) error = NULL;
      const char *uri;
      g_autoptr(GFile) file = NULL;
      g_autofree char *path = NULL;

      g_variant_lookup (options, "uri", "&s", &uri);
      file = g_file_new_for_uri (uri);
      path = g_file_get_path (file);

      pixbuf = gdk_pixbuf_new_from_file_at_scale (path, 60, 40, TRUE, &error);
      if (error)
        g_print ("failed to load screenshot: %s\n", error->message);
      else
        gtk_image_set_from_pixbuf (GTK_IMAGE (win->image), pixbuf);
    }
  else
    g_print ("canceled\n");

  if (win->screenshot_response_signal_id != 0)
    g_dbus_connection_signal_unsubscribe (connection,
                                          win->screenshot_response_signal_id);
}

static void
screenshot_called (GObject *source,
                   GAsyncResult *result,
                   gpointer data)
{
  PortalTestWin *win = data;
  g_autoptr(GError) error = NULL;
  g_autofree char *handle = NULL;

  if (!xdp_screenshot_call_screenshot_finish (win->screenshot, &handle, result, &error))
    {
      g_print ("error: %s\n", error->message);
      return;
    }

  win->screenshot_response_signal_id =
    g_dbus_connection_signal_subscribe (g_dbus_proxy_get_connection (G_DBUS_PROXY (win->screenshot)),
                                        "org.freedestkop.portal.Desktop",
                                        "org.freedesktop.portal.Request",
                                        "Response",
                                        handle,
                                        NULL,
                                        G_DBUS_SIGNAL_FLAGS_NO_MATCH_RULE,
                                        screenshot_response,
                                        win, NULL);
}

static void
take_screenshot (GtkWidget *button, PortalTestWin *win)
{
  GVariantBuilder opt_builder;
  GVariant *options;

  g_variant_builder_init (&opt_builder, G_VARIANT_TYPE_VARDICT);
  options = g_variant_builder_end (&opt_builder);

  xdp_screenshot_call_screenshot (win->screenshot,
                                  "",
                                  options,
                                  NULL,
                                  screenshot_called,
                                  win);
}

static void
portal_test_win_class_init (PortalTestWinClass *class)
{
  gtk_widget_class_set_template_from_resource (GTK_WIDGET_CLASS (class),
                                               "/org/gtk/portal-test/portal-test-win.ui");
  gtk_widget_class_bind_template_callback (GTK_WIDGET_CLASS (class), activate_link);
  gtk_widget_class_bind_template_callback (GTK_WIDGET_CLASS (class), save_dialog);
  gtk_widget_class_bind_template_callback (GTK_WIDGET_CLASS (class), take_screenshot);
  gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), PortalTestWin, sandbox_status);
  gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), PortalTestWin, network_status);
  gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), PortalTestWin, monitor_name);
  gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), PortalTestWin, proxies);
  gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), PortalTestWin, resolver_name);
  gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), PortalTestWin, image);
  gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), PortalTestWin, encoding);
}

GtkApplicationWindow *
portal_test_win_new (PortalTestApp *app)
{
  return g_object_new (portal_test_win_get_type (), "application", app, NULL);
}
