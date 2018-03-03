#include <gtk/gtk.h>

#include "screenshot-app.h"
#include "screenshot-menu.h"

struct _ScreenshotApp
{
	GtkApplication parent;
};

G_DEFINE_TYPE (ScreenshotApp, screenshot_app, GTK_TYPE_APPLICATION);

static void
screenshot_app_init (ScreenshotApp *app)
{

}

static void
screenshot_app_activate (GApplication *app)
{
	ScreenshotMenu *win;

	win = screenshot_menu_new (SCREENSHOT_APP (app));
	gtk_window_present (GTK_WINDOW (win));
}

static void
screenshot_app_class_init (ScreenshotAppClass *class)
{
	G_APPLICATION_CLASS (class)->activate = screenshot_app_activate;
}

ScreenshotApp *
screenshot_app_new (void)
{
	return g_object_new (screenshot_app_get_type (),
	                     "application-id",
	                     "com.tomaszal.simpleshot",
	                     "flags",
	                     G_APPLICATION_HANDLES_OPEN,
	                     NULL);
}
