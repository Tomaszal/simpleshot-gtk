#include <gtk/gtk.h>
#include <slop.hpp>

#include "screenshot-logic.h"

static GdkWindow *
find_current_window ()
{
	GdkScreen *screen;
	GdkWindow *window;
	GdkSeat *seat;
	GdkDevice *device;

	// Try to find an active window
	screen = gdk_screen_get_default ();
	window = gdk_screen_get_active_window (screen); // Deprecated, replace with custom function

	if (window == NULL)
	{
		// If no active window was found, try to get the window under the cursor
		seat = gdk_display_get_default_seat (gdk_display_get_default ());
		device = gdk_seat_get_pointer (seat);
		window = gdk_device_get_window_at_position (device, NULL, NULL);
	}

	if (window == NULL || gdk_window_get_type_hint (window) == GDK_WINDOW_TYPE_HINT_DESKTOP)
	{
		// If no window was found or the found window is desktop, return the root window (whole screen)
		return gdk_get_default_root_window ();
	}

	// Return the toplevel ancestor of the found window
	return gdk_window_get_toplevel (window);
}

static GdkRectangle
get_region_area ()
{
	GdkRectangle area;
	struct slop_options options;
	struct slop_selection selection;

	// Set slop options
	options = slop_options_default ();
	options.noopengl = 1;
	options.border = 4;
	options.r = 0.8;
	options.g = 0.2;
	options.b = 0.2;

	// Start slop selection
	selection = slop_select (&options);

	// Convert slop selection to rectangle
	area.x = selection.x;
	area.y = selection.y;
	area.width = selection.w;
	area.height = selection.h;

	return area;
}

static GdkRectangle
get_window_area (GdkWindow *window,
                 gboolean include_border)
{
	GdkRectangle area;
	GdkRectangle monitor_gemoetry;

	// Get the bounding box of the monitor at window
	gdk_monitor_get_geometry (gdk_display_get_monitor_at_window (gdk_display_get_default (), window), &monitor_gemoetry);

	if (include_border)
	{
		// Get the bounding box of the window with border
		gdk_window_get_frame_extents (window, &area);
	}
	else
	{
		// Get the bounding box of the window without border
		gdk_window_get_origin (window,
		                       &area.x,
		                       &area.y);

		area.width = gdk_window_get_width (window);
		area.height = gdk_window_get_height (window);
	}

	// Only return the visible area (that is on the monitor)

	if (area.x < 0)
	{
		area.width += area.x;
		area.x = 0;
	}

	if (area.y < 0)
	{
		area.height += area.y;
		area.y = 0;
	}

	if (area.x + area.width > monitor_gemoetry.width)
	{
		area.width = monitor_gemoetry.width - area.x;
	}

	if (area.y + area.height > monitor_gemoetry.height)
	{
		area.height = monitor_gemoetry.height - area.y;
	}

	return area;
}

GdkPixbuf *
screenshot (gint     area_index,
            gboolean include_cursor,
            gboolean include_decorations)
{
	GdkWindow *root;
	GdkPixbuf *screenshot;
	GdkRectangle area;

	root = gdk_get_default_root_window ();

	if (area_index == AREA_SCREEN)
	{
		area = get_window_area (root, FALSE);

		printf("Entire screen:\n");
	}

	if (area_index == AREA_WINDOW)
	{
		area = get_window_area (find_current_window (), include_decorations);

		printf("Current window:\n");
	}

	if (area_index == AREA_REGION)
	{
		area = get_region_area ();

		printf("Selected region:\n");
	}

	printf("x: %d y: %d w: %d h: %d\n", area.x, area.y, area.width, area.height);

	// Create pixbuf from window
	screenshot = gdk_pixbuf_get_from_window (root, area.x, area.y, area.width, area.height);

	if (include_cursor && area_index != AREA_REGION)
	{
		// Draw cursor on pixbuf
	}

	return screenshot;
}
