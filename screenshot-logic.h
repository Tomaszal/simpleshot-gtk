#ifndef SCREENSHOT_LOGIC_H
#define SCREENSHOT_LOGIC_H

#include <gtk/gtk.h>

#define SCREEN_AREA 0
#define WINDOW_AREA 1
#define REGION_AREA 2

void
screenshot (gint     area,
            gboolean include_cursor,
            gboolean include_decorations);

#endif // SCREENSHOT_LOGIC_H
