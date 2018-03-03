#include <gtk/gtk.h>

#include "screenshot-app.h"
#include "screenshot-menu.h"

struct _ScreenshotMenu
{
	GtkApplicationWindow parent;
};

G_DEFINE_TYPE (ScreenshotMenu,
               screenshot_menu,
               GTK_TYPE_APPLICATION_WINDOW);

static void
screenshot_menu_init (ScreenshotMenu *win)
{
	gtk_widget_init_template (GTK_WIDGET (win));
}

static void
screenshot_menu_class_init (ScreenshotMenuClass *class)
{
	gtk_widget_class_set_template_from_resource (GTK_WIDGET_CLASS (class),
	                                             "/com/tomaszal/screenshot-app/screenshot-menu.ui");
}

ScreenshotMenu *
screenshot_menu_new (ScreenshotApp *app)
{
	return g_object_new (screenshot_menu_get_type (),
	                     "application",
	                     app,
	                     NULL);
}
