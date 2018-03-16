#include <gtk/gtk.h>

#include "screenshot-app.h"
#include "actions-menu.h"

struct _ActionsMenuPrivate
{
	GtkWidget *test;
};

typedef struct _ActionsMenuPrivate ActionsMenuPrivate;

struct _ActionsMenu
{
	GtkApplicationWindow parent;

	ActionsMenuPrivate *priv;
};

G_DEFINE_TYPE_WITH_PRIVATE (ActionsMenu,
                            actions_menu,
                            GTK_TYPE_APPLICATION_WINDOW);

static void
actions_menu_init (ActionsMenu *win)
{
	// Set private window instance
	win->priv = actions_menu_get_instance_private (win);

	// Initialize window from the template
	gtk_widget_init_template (GTK_WIDGET (win));
}

static void
actions_menu_class_init (ActionsMenuClass *class)
{
	// Set window template from a resource
	gtk_widget_class_set_template_from_resource (GTK_WIDGET_CLASS (class), "/com/tomaszal/simpleshot/actions-menu.ui");
}

ActionsMenu *
actions_menu_new (ScreenshotApp *app)
{
	return g_object_new (actions_menu_get_type (),
	                     "application",
	                     app, NULL);
}
