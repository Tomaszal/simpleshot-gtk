#include <gtk/gtk.h>

#include "screenshot-app.h"
#include "screenshot-menu.h"

struct _ScreenshotMenu
{
	GtkApplicationWindow parent;
};

struct _ScreenshotMenuPrivate
{
	GtkWidget *delay_value;
	GtkWidget *disable_delay;
};

typedef struct _ScreenshotMenuPrivate ScreenshotMenuPrivate;

G_DEFINE_TYPE_WITH_PRIVATE (ScreenshotMenu,
                            screenshot_menu,
                            GTK_TYPE_APPLICATION_WINDOW);

static void
screenshot_clicked (GtkWidget      *button,
                    gpointer        data,
                    ScreenshotMenu *win)
{
	// Get private window instance
	ScreenshotMenuPrivate *priv;
	priv = screenshot_menu_get_instance_private (win);

	// Get the screenshot delay
	int delay = 0;
	if (! gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (priv->disable_delay)))
		delay = gtk_adjustment_get_value (gtk_spin_button_get_adjustment (GTK_SPIN_BUTTON (priv->delay_value)));
}

static void
disable_delay_toggled (GtkWidget      *button,
                       ScreenshotMenu *win)
{
	// Get private window instance
	ScreenshotMenuPrivate *priv;
	priv = screenshot_menu_get_instance_private (win);

	// Set delay spin button sensitivity according to show_delay toggle button
	gboolean show_delay = ! gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button));
	gtk_widget_set_sensitive (GTK_WIDGET (priv->delay_value), show_delay);
}

static void
screenshot_menu_init (ScreenshotMenu *win)
{
	// Init window from the template
	gtk_widget_init_template (GTK_WIDGET (win));

	// Get private window instance
	ScreenshotMenuPrivate *priv;
	priv = screenshot_menu_get_instance_private (win);

	// Set delay spin button sensitivity according to show_delay toggle button
	gboolean show_delay = ! gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (priv->disable_delay));
	gtk_widget_set_sensitive (GTK_WIDGET (priv->delay_value), show_delay);
}

static void
screenshot_menu_class_init (ScreenshotMenuClass *class)
{
	// Set window template from a resource
	gtk_widget_class_set_template_from_resource (GTK_WIDGET_CLASS (class),
	                                             "/com/tomaszal/screenshot-app/screenshot-menu.ui");

	// Bind widgets from the template
	gtk_widget_class_bind_template_child_private (GTK_WIDGET_CLASS (class), ScreenshotMenu, delay_value);
	gtk_widget_class_bind_template_child_private (GTK_WIDGET_CLASS (class), ScreenshotMenu, disable_delay);

	// Bind signals from the template
	gtk_widget_class_bind_template_callback (GTK_WIDGET_CLASS (class), screenshot_clicked);
	gtk_widget_class_bind_template_callback (GTK_WIDGET_CLASS (class), disable_delay_toggled);
}

ScreenshotMenu *
screenshot_menu_new (ScreenshotApp *app)
{
	return g_object_new (screenshot_menu_get_type (),
	                     "application",
	                     app, NULL);
}
