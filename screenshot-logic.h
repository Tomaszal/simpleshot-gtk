#ifndef SCREENSHOT_LOGIC_H
#define SCREENSHOT_LOGIC_H

#include <gtk/gtk.h>

#define AREA_SCREEN 0
#define AREA_WINDOW 1
#define AREA_REGION 2

GdkPixbuf *
screenshot (gint     area,
            gboolean include_cursor,
            gboolean include_decorations);

#endif // SCREENSHOT_LOGIC_H
