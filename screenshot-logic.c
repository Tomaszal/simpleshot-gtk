#include <gtk/gtk.h>
#include <MagickWand/MagickWand.h>
#include <slop.hpp>
#include <X11/Xlib.h>
#include <stdio.h>

#include "screenshot-logic.h"

struct area {
	int x;
	int y;
	int w;
	int h;
};

struct area
region_select ()
{
	// Variables
	struct slop_options options;
	struct slop_selection selection;
	struct area area;

	// Set slop options
	options = slop_options_default ();
	options.noopengl = 1;
	options.border = 4;
	options.r = 0.8;
	options.g = 0.2;
	options.b = 0.2;

	// Start slop selection
	selection = slop_select (&options);

	// Transfer selection to area struct
	area.x = (int) selection.x;
	area.y = (int) selection.y;
	area.w = (int) selection.w;
	area.h = (int) selection.h;

	// Return selected region area
	return area;
}

struct area
active_window_select ()
{
	// Variables
	struct area area;

	// X11
	int revert = RevertToNone;
	Display *display;
	Window window;
	XWindowAttributes attr;

	// Get X11 window attributes
	display = XOpenDisplay(NULL);
	XGetInputFocus (display, &window, &revert);
	XGetWindowAttributes (display, window, &attr);

	// Transfer selection to area struct
	area.x = attr.x;
	area.y = attr.y;
	area.w = attr.width;
	area.h = attr.height;

	// Return active window area
	return area;
}

struct area
screen_select ()
{
	// Variables
	struct area area;

	// X11
	Display *display;
	XWindowAttributes attr;

	// Get X11 window attributes
	display = XOpenDisplay(NULL);
	XGetWindowAttributes (display, DefaultRootWindow(display), &attr);

	// Transfer selection to area struct
	area.x = 0;
	area.y = 0;
	area.w = attr.width;
	area.h = attr.height;

	// Return screen area
	return area;
}

void
screenshot (gint     area_index,
            gboolean include_cursor,
            gboolean include_decorations)
{
	// Variables
	int x, y;
	char hex[7];
	unsigned long nwands;
	struct area area;

	// X11
	Display *display;
	Window window;
	XWindowAttributes attr;
	XImage *image;

	// MagickWand
	MagickWand *wand;
	PixelWand *pwand;
	PixelIterator *pitr;
	PixelWand **wand_pixels;

	// Select area and get its geometry
	switch (area_index)
	{
		case 0:
			area = screen_select();
			break;
		case 1:
			area = active_window_select();
			break;
		case 2:
			area = region_select();
			break;
	}

	// Get X11 window attributes
	display = XOpenDisplay(NULL);
	window = DefaultRootWindow(display);
	XGetWindowAttributes (display, window, &attr);

	// Set-up Wand
	MagickWandGenesis ();
	pwand = NewPixelWand ();
	PixelSetColor (pwand, "white");
	wand = NewMagickWand ();
	MagickNewImage (wand, area.w, area.h, pwand);
	pitr = NewPixelIterator(wand);

	// Get image from display server
	image = XGetImage (display, window,
	                   0, 0,
	                   attr.width, attr.height,
	                   AllPlanes, ZPixmap);

	// Transfer image to the wand
	for (y = area.y; y < area.y + area.h; y++)
	{
		wand_pixels = PixelGetNextIteratorRow(pitr, &nwands);

		for (x = area.x; x < area.x + area.w; x++)
		{
			sprintf (hex, "#%06x", XGetPixel (image, x, y));
			PixelSetColor (wand_pixels[x - (int) area.x], hex);
		}

		(void) PixelSyncIterator (pitr);
	}

	// Write to disk
	MagickWriteImage (wand, "screen_test.png");

	// Cleanup
	XDestroyImage (image);
	DestroyPixelIterator (pitr);
	DestroyMagickWand (wand);
	MagickWandTerminus ();
	XCloseDisplay (display);
}
