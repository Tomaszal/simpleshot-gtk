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
	GtkWidget *delay_disable;

	GtkWidget *area_list;

	GtkWidget *include_cursor;
	GtkWidget *include_decorations;
};

typedef struct _ScreenshotMenuPrivate ScreenshotMenuPrivate;

G_DEFINE_TYPE_WITH_PRIVATE (ScreenshotMenu,
                            screenshot_menu,
                            GTK_TYPE_APPLICATION_WINDOW);

static void
screenshot_button_clicked (GtkWidget      *button,
                           ScreenshotMenu *win)
{
	// Get private window instance
	ScreenshotMenuPrivate *priv;
	priv = screenshot_menu_get_instance_private (win);

	// Get the screenshot delay
	int delay = 0;
	if (! gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (priv->delay_disable)))
		delay = gtk_adjustment_get_value (gtk_spin_button_get_adjustment (GTK_SPIN_BUTTON (priv->delay_value)));
}

static void
delay_disable_toggled (GtkWidget      *toggle_button,
                       ScreenshotMenu *win)
{
	// Get private window instance
	ScreenshotMenuPrivate *priv;
	priv = screenshot_menu_get_instance_private (win);

	// Set delay_value sensitivity
	gboolean show_delay = ! gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (toggle_button)) && gtk_widget_get_sensitive (GTK_WIDGET (toggle_button));
	gtk_widget_set_sensitive (GTK_WIDGET (priv->delay_value), show_delay);
}

static void
area_list_changed (GtkWidget      *area_list,
                   ScreenshotMenu *win)
{
	// Get private window instance
	ScreenshotMenuPrivate *priv;
	priv = screenshot_menu_get_instance_private (win);

	// Get selected area index (-1 - none; 0 - entire screen; 1 - active window; 2 - region)
	gint index = gtk_combo_box_get_active (GTK_COMBO_BOX (area_list));

	// Set sensetivity for capture options (0 - delay, 1 - include cursor; 2 - show decorations)
	gint options[3];
	switch (index)
	{
		case 0:
			options[0] = 1;
			options[1] = 1;
			options[2] = 1;
			break;

		case 1:
			options[0] = 1;
			options[1] = 1;
			options[2] = 0;
			break;

		case 2:
			options[0] = 0;
			options[1] = 0;
			options[2] = 0;
			break;
	}

	gtk_widget_set_sensitive (GTK_WIDGET (priv->delay_disable), options[0]);
	delay_disable_toggled(GTK_WIDGET (priv->delay_disable), win);
	gtk_widget_set_sensitive (GTK_WIDGET (priv->include_cursor), options[1]);
	gtk_widget_set_sensitive (GTK_WIDGET (priv->include_decorations), options[2]);
}

static void
screenshot_menu_init (ScreenshotMenu *win)
{
	// Init window from the template
	gtk_widget_init_template (GTK_WIDGET (win));

	// Get private window instance
	ScreenshotMenuPrivate *priv;
	priv = screenshot_menu_get_instance_private (win);

	delay_disable_toggled (GTK_WIDGET (priv->delay_disable), win);
}

static void
screenshot_menu_class_init (ScreenshotMenuClass *class)
{
	// Set window template from a resource
	gtk_widget_class_set_template_from_resource (GTK_WIDGET_CLASS (class),
	                                             "/com/tomaszal/screenshot-app/screenshot-menu.ui");

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
