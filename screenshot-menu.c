#include <gtk/gtk.h>

#include "screenshot-app.h"
#include "screenshot-menu.h"
#include "screenshot-logic.h"
#include "actions-menu.h"

struct _ScreenshotMenuPrivate
{
	GSettings *settings;

	GtkWidget *delay_value;
	GtkWidget *delay_disable;

	GtkWidget *area_list;

	GtkWidget *include_cursor;
	GtkWidget *include_decorations;
};

typedef struct _ScreenshotMenuPrivate ScreenshotMenuPrivate;

struct _ScreenshotMenu
{
	GtkApplicationWindow parent;

	ScreenshotMenuPrivate *priv;

	gint     area;
	gint     delay;
	gboolean include_cursor;
	gboolean include_decorations;
};

G_DEFINE_TYPE_WITH_PRIVATE (ScreenshotMenu,
                            screenshot_menu,
                            GTK_TYPE_APPLICATION_WINDOW);

static gboolean
start_screenshot (gpointer data)
{
	ActionsMenu *win;
	GtkApplication *app;

	// Make a screenshot
	screenshot (gtk_combo_box_get_active (GTK_COMBO_BOX (SCREENSHOT_MENU (data)->priv->area_list)),
	            gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (SCREENSHOT_MENU (data)->priv->include_cursor)),
	            gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (SCREENSHOT_MENU (data)->priv->include_decorations)));

	// Show screenshot menu window
	// gtk_widget_show (GTK_WIDGET (data));

	app = gtk_window_get_application (GTK_WINDOW (data));

	// ScreenshotApp *app = SCREENSHOT_MENU (data)->app;
	win = actions_menu_new (SCREENSHOT_APP (app));
	gtk_application_add_window (app, GTK_WINDOW (win));

	gtk_window_present (GTK_WINDOW (win));
	gtk_window_close (GTK_WINDOW (data));
	// Remove GSource from the main loop
	return G_SOURCE_REMOVE;
}

static void
screenshot_button_clicked (GtkWidget      *button,
                           ScreenshotMenu *win)
{
	// Add slight delay to wait for the window to disappear (haven't found a better soltuion)
	guint delay = 200;

	// Get the screenshot delay
	if (! gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (win->priv->delay_disable)) && gtk_widget_get_sensitive (GTK_WIDGET (win->priv->delay_disable)))
	{
		delay = gtk_adjustment_get_value (gtk_spin_button_get_adjustment (GTK_SPIN_BUTTON (win->priv->delay_value)));
	}

	// Hide screenshot menu window
	gtk_widget_hide (GTK_WIDGET (win));
	// gtk_window_close (GTK_WINDOW (win));

	// Start async screenshot function
	g_timeout_add (delay, start_screenshot, win);
}

static void
delay_disable_toggled (GtkWidget      *toggle_button,
                       ScreenshotMenu *win)
{
	gboolean show_delay;

	// Set delay_value sensitivity
	show_delay = ! gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (toggle_button)) && gtk_widget_get_sensitive (GTK_WIDGET (toggle_button));
	gtk_widget_set_sensitive (GTK_WIDGET (win->priv->delay_value), show_delay);
}

static void
area_list_changed (GtkWidget      *area_list,
                   ScreenshotMenu *win)
{
	gint     index;
	gboolean options[3][3] = {
		{ TRUE,  TRUE,  FALSE },
		{ TRUE,  TRUE,  TRUE  },
		{ FALSE, FALSE, FALSE }
	};

	// Get selected area index (0 - entire screen; 1 - active window; 2 - region)
	index = gtk_combo_box_get_active (GTK_COMBO_BOX (area_list));
	win->area = index;

	// Set sensetivity for capture options (0 - delay, 1 - include cursor; 2 - show decorations)
	gtk_widget_set_sensitive (GTK_WIDGET (win->priv->delay_disable), options[index][0]);
	delay_disable_toggled(GTK_WIDGET (win->priv->delay_disable), win);
	gtk_widget_set_sensitive (GTK_WIDGET (win->priv->include_cursor), options[index][1]);
	gtk_widget_set_sensitive (GTK_WIDGET (win->priv->include_decorations), options[index][2]);
}

static void
screenshot_menu_dispose (GObject *object)
{
	// Dispose of settings object
	g_clear_object (&SCREENSHOT_MENU (object)->priv->settings);

	// Dispose of parent class
	G_OBJECT_CLASS (screenshot_menu_parent_class)->dispose (object);
}

static void
screenshot_menu_init (ScreenshotMenu *win)
{
	// Set private window instance
	win->priv = screenshot_menu_get_instance_private (win);

	// Initialize window from the template
	gtk_widget_init_template (GTK_WIDGET (win));

	// Initialize GSettings
	win->priv->settings = g_settings_new ("com.tomaszal.simpleshot");

	// Bind settings
	g_settings_bind (win->priv->settings, "area",
	                 win->priv->area_list, "active",
	                 G_SETTINGS_BIND_DEFAULT);

	g_settings_bind (win->priv->settings, "delay-value",
	                 win->priv->delay_value, "value",
	                 G_SETTINGS_BIND_DEFAULT);
	g_settings_bind (win->priv->settings, "delay-disable",
	                 win->priv->delay_disable, "active",
	                 G_SETTINGS_BIND_DEFAULT);

	g_settings_bind (win->priv->settings, "include-cursor",
	                 win->priv->include_cursor, "active",
	                 G_SETTINGS_BIND_DEFAULT);
	g_settings_bind (win->priv->settings, "include-decorations",
	                 win->priv->include_decorations, "active",
	                 G_SETTINGS_BIND_DEFAULT);

	// Update UI elements
	delay_disable_toggled (GTK_WIDGET (win->priv->delay_disable), win);
	area_list_changed (GTK_WIDGET (win->priv->area_list), win);
}

static void
screenshot_menu_class_init (ScreenshotMenuClass *class)
{
	// Assign GApplication entry points
	G_OBJECT_CLASS (class)->dispose = screenshot_menu_dispose;

	// Set window template from a resource
	gtk_widget_class_set_template_from_resource (GTK_WIDGET_CLASS (class), "/com/tomaszal/simpleshot/screenshot-menu.ui");

	// Bind widgets from the template
	gtk_widget_class_bind_template_child_private (GTK_WIDGET_CLASS (class), ScreenshotMenu, delay_value);
	gtk_widget_class_bind_template_child_private (GTK_WIDGET_CLASS (class), ScreenshotMenu, delay_disable);

	gtk_widget_class_bind_template_child_private (GTK_WIDGET_CLASS (class), ScreenshotMenu, area_list);

	gtk_widget_class_bind_template_child_private (GTK_WIDGET_CLASS (class), ScreenshotMenu, include_cursor);
	gtk_widget_class_bind_template_child_private (GTK_WIDGET_CLASS (class), ScreenshotMenu, include_decorations);

	// Bind signals from the template
	gtk_widget_class_bind_template_callback (GTK_WIDGET_CLASS (class), screenshot_button_clicked);

	gtk_widget_class_bind_template_callback (GTK_WIDGET_CLASS (class), delay_disable_toggled);

	gtk_widget_class_bind_template_callback (GTK_WIDGET_CLASS (class), area_list_changed);
}

ScreenshotMenu *
screenshot_menu_new (ScreenshotApp *app)
{
	return g_object_new (screenshot_menu_get_type (),
	                     "application",
	                     app, NULL);
}
