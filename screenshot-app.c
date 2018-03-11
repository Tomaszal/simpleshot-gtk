#include <gtk/gtk.h>
#include <glib/gi18n.h>

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
show_about (GSimpleAction *action,
            GVariant      *parameter,
            gpointer       data)
{
	// Variables
	const gchar *authors[] = { "Tomas Zaluckij (@Tomaszal)", NULL };

	// Create and show about dialog
	GList *windows = gtk_application_get_windows (GTK_APPLICATION (data));
	gtk_show_about_dialog (GTK_WINDOW (g_list_nth_data (windows, 0)),
	                       "authors", authors,
	                       "program-name", "Simpleshot",
	                       "title", _("About Simpleshot"),
	                       "comments", _("A simple GTK screenshot application (WIP)."),
	                       "translator-credits", _("translator-credits"),
	                       "logo-icon-name", "applets-screenshooter",
	                       "license-type", GTK_LICENSE_GPL_3_0,
	                       NULL);
}

static void
quit_activated (GSimpleAction *action,
                GVariant      *parameter,
                gpointer       data)
{
	g_application_quit (G_APPLICATION (data));
}

static GActionEntry app_entries[] =
{
	{ "about", show_about, NULL, NULL, NULL },
	{ "quit", quit_activated, NULL, NULL, NULL }
};

static void
screenshot_app_activate (GApplication *app)
{
	// Variables
	ScreenshotMenu *win;

	// Activate parent class
	G_APPLICATION_CLASS (screenshot_app_parent_class)->activate (app);

	// Create and activate the screenshot menu window
	win = screenshot_menu_new (SCREENSHOT_APP (app));
	gtk_window_present (GTK_WINDOW (win));
}

static void
screenshot_app_startup (GApplication *app)
{
	// Variables
	GtkBuilder *builder;
	GMenuModel *app_menu;

	// Startup parent class
	G_APPLICATION_CLASS (screenshot_app_parent_class)->startup (app);

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

	// Add application menu
	builder = gtk_builder_new_from_resource ("/com/tomaszal/simpleshot/app-menu.ui");
	app_menu = G_MENU_MODEL (gtk_builder_get_object (builder, "appmenu"));
	gtk_application_set_app_menu (GTK_APPLICATION (app), app_menu);

	// Cleanup
	g_object_unref (builder);
	g_object_unref (app_menu);
}

static void
screenshot_app_init (ScreenshotApp *app)
{
}

static void
screenshot_app_class_init (ScreenshotAppClass *class)
{
	// Assign GApplication entry points
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
