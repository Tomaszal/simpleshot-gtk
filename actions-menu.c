#include <gtk/gtk.h>

#include "screenshot-app.h"
#include "actions-menu.h"

struct _ActionsMenuPrivate
{
	GtkWidget *preview_drawing_area;
	// GtkWidget *preview_aspect_frame;
};

typedef struct _ActionsMenuPrivate ActionsMenuPrivate;

struct _ActionsMenu
{
	GtkApplicationWindow parent;

	ActionsMenuPrivate *priv;

	GdkPixbuf *screenshot;
};

G_DEFINE_TYPE_WITH_PRIVATE (ActionsMenu,
                            actions_menu,
                            GTK_TYPE_APPLICATION_WINDOW);

static void
on_preview_draw (GtkWidget *widget,
                 cairo_t   *cr,
                 gpointer  *data)
{
	GtkStyleContext *context;
	GdkPixbuf *screenshot;
	guint width, height;

	// Get preview drawing area's context and dimensions
	context = gtk_widget_get_style_context (widget);
	width = gtk_widget_get_allocated_width (widget);
	height = gtk_widget_get_allocated_height (widget);

	// Scale the screenshot to fit inside of the drawing area;
	screenshot = gdk_pixbuf_scale_simple (ACTIONS_MENU (data)->screenshot,
	                                      width, height,
	                                      GDK_INTERP_BILINEAR);

	// Render the pixbuf inside of the drawing area
	gtk_render_icon (context, cr, screenshot, 0, 0);
}

static void
actions_menu_startup (ActionsMenu *win)
{
	printf("test\n");
}

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

	// Bind widgets from the template
	gtk_widget_class_bind_template_child_private (GTK_WIDGET_CLASS (class), ActionsMenu, preview_drawing_area);
	// gtk_widget_class_bind_template_child_private (GTK_WIDGET_CLASS (class), ActionsMenu, preview_aspect_frame);

	// Bind signals from the template
	gtk_widget_class_bind_template_callback (GTK_WIDGET_CLASS (class), on_preview_draw);
}

ActionsMenu *
actions_menu_new (GdkPixbuf *screenshot,
                  ScreenshotApp *app)
{
	ActionsMenu *win;
	gint width, height, scale;

	win = g_object_new (actions_menu_get_type (),
	                             "application",
	                             app, NULL);

	win->screenshot = screenshot;

	// Screenshot preview scale 1:5 relative to its real size
	scale = 5 * gtk_widget_get_scale_factor (GTK_WIDGET (win));

	// Get screenshot dimensions and scale them
	width = gdk_pixbuf_get_width (screenshot) / scale;
	height = gdk_pixbuf_get_height (screenshot) / scale;

	// Set drawing preview area dimensions
	gtk_widget_set_size_request (GTK_WIDGET (win->priv->preview_drawing_area),
	                             width, height);

	return win;
}
