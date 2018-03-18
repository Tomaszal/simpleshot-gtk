#include <gtk/gtk.h>
#include <glib/gi18n.h>

#include "screenshot-app.h"
#include "screenshot-menu.h"
#include "actions-menu.h"

struct _ActionsMenuPrivate
{
	GSettings *settings;

	GtkWidget *action_save;
	GtkWidget *action_copy;
	GtkWidget *action_open;
	GtkWidget *action_host;

	GtkWidget *png_application;
	GtkWidget *hosting_service;

	GtkWidget *preview_drawing_area;
	GtkWidget *preview_aspect_frame;
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

static gboolean
action_save (ActionsMenu *win)
{
	GtkWidget *dialog;
	GtkFileChooser *chooser;
	gchararray filename;
	gint result;

	// Create a new file chooser dialog
	dialog = gtk_file_chooser_dialog_new (_("Save Screenshot"),
	                                      GTK_WINDOW (win),
	                                      GTK_FILE_CHOOSER_ACTION_SAVE,
	                                      _("Cancel"),
	                                      GTK_RESPONSE_CANCEL,
	                                      _("Save"),
	                                      GTK_RESPONSE_ACCEPT,
	                                      NULL);

	chooser = GTK_FILE_CHOOSER (dialog);

	// Forbid saving in system folders and warn about overwriting
	gtk_file_chooser_set_local_only (chooser, TRUE);
	gtk_file_chooser_set_do_overwrite_confirmation (chooser, TRUE);

	// Generate and set the suggested filename
	filename = g_date_time_format (g_date_time_new_now_local (), "Screenshot_%Y-%m-%d_%H-%M-%S.png");
	gtk_file_chooser_set_current_name (chooser, filename);

	// Run a dialog box
	result = gtk_dialog_run (GTK_DIALOG (dialog));

	// Grab the filename and destroy the dialog
	filename = gtk_file_chooser_get_filename (chooser);
	gtk_widget_destroy (dialog);

	// Save the screenshot
	if (result == GTK_RESPONSE_ACCEPT)
	{
		gdk_pixbuf_save (win->screenshot, filename, "png", NULL, NULL);

		return TRUE;
	}

	return FALSE;
}

static gboolean
action_copy (ActionsMenu *win)
{
	GtkClipboard *clipboard;

	// Get the default clipboard object
	clipboard = gtk_clipboard_get (GDK_SELECTION_CLIPBOARD);

	// Copy screenshot pixbuf to clipboard
	gtk_clipboard_set_image (clipboard, win->screenshot);

	return FALSE;
}

static gboolean
action_open (ActionsMenu *win)
{
	GAppInfo *appinfo;
	gchararray filename;
	GList *file = NULL;

	// Get the selected appliaction information
	appinfo = gtk_app_chooser_get_app_info (GTK_APP_CHOOSER (win->priv->png_application));

	// Generate and set the suggested filename
	filename = g_date_time_format (g_date_time_new_now_local (), "/tmp/Screenshot_%Y-%m-%d_%H-%M-%S.png");

	// Save screenshot to a temporary file
	gdk_pixbuf_save (win->screenshot, filename, "png", NULL, NULL);

	// Launch the appliacation with screnshot URI
	file = g_list_append (file, g_filename_to_uri (filename, NULL, NULL));
	g_app_info_launch_uris (appinfo, file, NULL, NULL);

	return TRUE;
}

static void
action_host (ActionsMenu *win)
{
	gchararray hosting_service_name = "NaN";
	gint hosting_service;

	hosting_service = gtk_combo_box_get_active (GTK_COMBO_BOX (win->priv->hosting_service));

	if (hosting_service == HOST_IMGUR) {
		hosting_service_name = "Imgur";
	}

	printf("Hosting screenshot on %s...\n", hosting_service_name);
}

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
action_open_toggled (GtkWidget   *widget,
                     ActionsMenu *win)
{
	gtk_widget_set_sensitive (GTK_WIDGET (win->priv->png_application),
	                          gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (widget)));
}

static void
action_host_toggled (GtkWidget   *widget,
                     ActionsMenu *win)
{
	gtk_widget_set_sensitive (GTK_WIDGET (win->priv->hosting_service),
	                          gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (widget)));
}

static void
back_button_clicked (GtkWidget   *widget,
                     ActionsMenu *win)
{
	GtkApplication *app;
	ScreenshotMenu *new_win;

	// Create a new screenshot menu window and add it to the application
	app = gtk_window_get_application (GTK_WINDOW (win));
	new_win = screenshot_menu_new (SCREENSHOT_APP (app));
	gtk_application_add_window (app, GTK_WINDOW (new_win));

	// Close the actions menu window (which removes it from the application)
	gtk_window_close (GTK_WINDOW (win));

	// Show the screenshot menu window
	gtk_window_present (GTK_WINDOW (new_win));
}

static void
ok_button_clicked (GtkWidget   *widget,
                   ActionsMenu *win)
{
	gboolean close_application;

	if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (win->priv->action_save)))
	{
		close_application = action_save (win);
	}

	if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (win->priv->action_copy)))
	{
		close_application = action_copy (win);
	}

	if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (win->priv->action_open)))
	{
		close_application = action_open (win);
	}

	if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (win->priv->action_host)))
	{
		action_host (win);
	}

	if (close_application)
	{
		g_application_quit (G_APPLICATION (gtk_window_get_application (GTK_WINDOW (win))));
	}
}

static void
actions_menu_dispose (GObject *object)
{
	// Dispose of settings object
	g_clear_object (&ACTIONS_MENU (object)->priv->settings);

	// Dispose of parent class
	G_OBJECT_CLASS (actions_menu_parent_class)->dispose (object);
}

static void
actions_menu_init (ActionsMenu *win)
{
	// Set private window instance
	win->priv = actions_menu_get_instance_private (win);

	// Initialize window from the template
	gtk_widget_init_template (GTK_WIDGET (win));

	// Initialize GSettings
	win->priv->settings = g_settings_new ("com.tomaszal.simpleshot");

	// Bind settings
	g_settings_bind (win->priv->settings, "action-save",
	                 win->priv->action_save, "active",
	                 G_SETTINGS_BIND_DEFAULT);
	g_settings_bind (win->priv->settings, "action-copy",
	                 win->priv->action_copy, "active",
	                 G_SETTINGS_BIND_DEFAULT);
	g_settings_bind (win->priv->settings, "action-open",
	                 win->priv->action_open, "active",
	                 G_SETTINGS_BIND_DEFAULT);
	g_settings_bind (win->priv->settings, "action-host",
	                 win->priv->action_host, "active",
	                 G_SETTINGS_BIND_DEFAULT);

	g_settings_bind (win->priv->settings, "png-application",
	                 win->priv->png_application, "active",
	                 G_SETTINGS_BIND_DEFAULT);
	g_settings_bind (win->priv->settings, "hosting-service",
	                 win->priv->hosting_service, "active",
	                 G_SETTINGS_BIND_DEFAULT);

	// Update UI elements
	action_open_toggled (GTK_WIDGET (win->priv->action_open), win);
	action_host_toggled (GTK_WIDGET (win->priv->action_host), win);
}

static void
actions_menu_class_init (ActionsMenuClass *class)
{
	// Assign GApplication entry points
	G_OBJECT_CLASS (class)->dispose = actions_menu_dispose;

	// Set window template from a resource
	gtk_widget_class_set_template_from_resource (GTK_WIDGET_CLASS (class), "/com/tomaszal/simpleshot/actions-menu.ui");

	// Bind widgets from the template
	gtk_widget_class_bind_template_child_private (GTK_WIDGET_CLASS (class), ActionsMenu, action_save);
	gtk_widget_class_bind_template_child_private (GTK_WIDGET_CLASS (class), ActionsMenu, action_copy);
	gtk_widget_class_bind_template_child_private (GTK_WIDGET_CLASS (class), ActionsMenu, action_open);
	gtk_widget_class_bind_template_child_private (GTK_WIDGET_CLASS (class), ActionsMenu, action_host);

	gtk_widget_class_bind_template_child_private (GTK_WIDGET_CLASS (class), ActionsMenu, png_application);
	gtk_widget_class_bind_template_child_private (GTK_WIDGET_CLASS (class), ActionsMenu, hosting_service);

	gtk_widget_class_bind_template_child_private (GTK_WIDGET_CLASS (class), ActionsMenu, preview_drawing_area);
	gtk_widget_class_bind_template_child_private (GTK_WIDGET_CLASS (class), ActionsMenu, preview_aspect_frame);

	// Bind signals from the template
	gtk_widget_class_bind_template_callback (GTK_WIDGET_CLASS (class), back_button_clicked);
	gtk_widget_class_bind_template_callback (GTK_WIDGET_CLASS (class), ok_button_clicked);

	gtk_widget_class_bind_template_callback (GTK_WIDGET_CLASS (class), action_open_toggled);
	gtk_widget_class_bind_template_callback (GTK_WIDGET_CLASS (class), action_host_toggled);

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

	// Set preview aspect frame ratio
	gtk_aspect_frame_set (GTK_ASPECT_FRAME (win->priv->preview_aspect_frame), 0.5, 0.5,
	                      (gfloat) width / (gfloat) height,
	                      FALSE);

	return win;
}
