#include <gtk/gtk.h>
#include <gio/gdesktopappinfo.h>

#include "portal-test-app.h"
#include "portal-test-win.h"

struct _PortalTestWin
{
  GtkApplicationWindow parent;
};

struct _PortalTestWinClass
{
  GtkApplicationWindowClass parent_class;
};

G_DEFINE_TYPE (PortalTestWin, portal_test_win, GTK_TYPE_APPLICATION_WINDOW);


static void
portal_test_win_init (PortalTestWin *win)
{
  gtk_widget_init_template (GTK_WIDGET (win));
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
portal_test_win_class_init (PortalTestWinClass *class)
{
  gtk_widget_class_set_template_from_resource (GTK_WIDGET_CLASS (class),
                                               "/org/gtk/portal-test/portal-test-win.ui");
  gtk_widget_class_bind_template_callback (GTK_WIDGET_CLASS (class), activate_link);
}

GtkApplicationWindow *
portal_test_win_new (PortalTestApp *app)
{
  return g_object_new (portal_test_win_get_type (), "application", app, NULL);
}
