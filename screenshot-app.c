#include <gtk/gtk.h>

#include "screenshot-app.h"
#include "screenshot-menu.h"

struct _ScreenshotApp
{
	GtkApplication parent;
};

G_DEFINE_TYPE (ScreenshotApp,
               screenshot_app,
               GTK_TYPE_APPLICATION);

static void
quit_activated (GSimpleAction *action,
                GVariant      *parameter,
                gpointer       app)
{
	g_application_quit (G_APPLICATION (app));
}

static GActionEntry app_entries[] =
{
	// Add a quit action
	{ "quit", quit_activated, NULL, NULL, NULL }
};

static void
screenshot_app_init (ScreenshotApp *app)
{

}

static void
screenshot_app_activate (GApplication *app)
{
	// Create and activate the screenshot menu window
	ScreenshotMenu *win;
	win = screenshot_menu_new (SCREENSHOT_APP (app));
	gtk_window_present (GTK_WINDOW (win));
}

static void
screenshot_app_startup (GApplication *app)
{
	// Add action entries (GActionEntry) to action map (GActionMap)
	g_action_map_add_action_entries (G_ACTION_MAP (app),
	                                 app_entries,
	                                 G_N_ELEMENTS (app_entries),
	                                 app);

	// Add quit key accelerators
	const gchar *quit_accels[3] = { "<Ctrl>Q", "Escape", NULL };
	gtk_application_set_accels_for_action (GTK_APPLICATION (app),
	                                       "app.quit",
	                                       quit_accels);



	G_APPLICATION_CLASS (screenshot_app_parent_class)->startup (app);
}

static void
screenshot_app_class_init (ScreenshotAppClass *class)
{
	G_APPLICATION_CLASS (class)->startup = screenshot_app_startup;
	G_APPLICATION_CLASS (class)->activate = screenshot_app_activate;
}

ScreenshotApp *
screenshot_app_new (void)
{
	return g_object_new (screenshot_app_get_type (),
	                     "application-id",
	                     "com.tomaszal.simpleshot",
	                     "flags",
	                     G_APPLICATION_HANDLES_OPEN, NULL);
}
