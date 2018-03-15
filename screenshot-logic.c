#include <gtk/gtk.h>

#include "screenshot-logic.h"

static void
get_window_coordinates (GdkWindow *window,
                        gboolean include_border,
                        GdkRectangle *real_coordinates_out,
                        GdkRectangle *visible_coordinates_out)
{
	// Window coordinates that are within the screen
	GdkRectangle visible_coordinates;
	GdkRectangle real_coordinates;
	GdkRectangle monitor_gemoetry;

	if (include_border)
	{
		// Get real window coordinates with border
		gdk_window_get_frame_extents (window, &real_coordinates);
	}
	else
	{
		// Get real window coordinates without border
		real_coordinates.width = gdk_window_get_width (window);
		real_coordinates.height = gdk_window_get_height (window);

		gdk_window_get_origin (window,
		                       &real_coordinates.x,
		                       &real_coordinates.y);
	}

	if (real_coordinates_out != NULL)
	{
		// Return real coordinates
		*real_coordinates_out = real_coordinates;
	}

	// Calculate coordinates of the window that are visible
	visible_coordinates = real_coordinates;

	if (visible_coordinates.x < 0)
	{
		visible_coordinates.width += visible_coordinates.x;
		visible_coordinates.x = 0;
	}

	if (visible_coordinates.y < 0)
	{
		visible_coordinates.height += visible_coordinates.y;
		visible_coordinates.y = 0;
	}

	gdk_monitor_get_geometry (gdk_display_get_monitor_at_window (gdk_display_get_default (), window), &monitor_gemoetry);

	if (visible_coordinates.x + visible_coordinates.width > monitor_gemoetry.width)
	{
		visible_coordinates.width = monitor_gemoetry.width - visible_coordinates.x;
	}

	if (visible_coordinates.y + visible_coordinates.height > monitor_gemoetry.height)
	{
		visible_coordinates.height = monitor_gemoetry.height - visible_coordinates.y;
	}

	if (visible_coordinates_out != NULL)
	{
		// Return screenshot coordinates
		*visible_coordinates_out = visible_coordinates;
	}
}

static GdkWindow *
find_current_window ()
{
	GdkWindow *window = NULL;
	GdkSeat *seat;
	GdkDevice *device;
	GdkScreen *screen;

	// Try to find an active window
	screen = gdk_screen_get_default ();
	window = gdk_screen_get_active_window (screen); // Deprecated, replace with custom function
	seat = gdk_display_get_default_seat (gdk_display_get_default ());
	device = gdk_seat_get_pointer (seat);

	if (window == NULL)
	{
		// If there is no active window, try to get the window under the cursor
		window = gdk_device_get_window_at_position (device, NULL, NULL);
	}

	if (window != NULL)
	{
		if (gdk_window_get_type_hint (window) == GDK_WINDOW_TYPE_HINT_DESKTOP)
		{
			// If selected window is destkop, get the whole screen
			window = NULL;
		}
		else
		{
			// Get the toplevel ancestor of selected window
			window = gdk_window_get_toplevel (window);
		}
	}

	if (window == NULL)
	{
		// If no window was found, get the root window (whole screen)
		window = gdk_get_default_root_window ();
	}

	return window;
}

void
screenshot (gint     area,
            gboolean include_cursor,
            gboolean include_decorations)
{
	GdkWindow *window;

	window = find_current_window ();

	GdkRectangle test1, test2;
	get_window_coordinates (window, include_decorations, &test1, &test2);

	printf("Printing window coordinates...\n");
	printf("Real:    x: %d y: %d w: %d h: %d\n", test1.x, test1.y, test1.width, test1.height);
	printf("Visible: x: %d y: %d w: %d h: %d\n", test2.x, test2.y, test2.width, test2.height);
}
