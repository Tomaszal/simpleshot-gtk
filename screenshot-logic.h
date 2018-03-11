#ifndef SCREENSHOT_LOGIC_H
#define SCREENSHOT_LOGIC_H

#include <gtk/gtk.h>

void
screenshot (gint     area_index,
            gboolean include_cursor,
            gboolean include_decorations);

#endif // SCREENSHOT_LOGIC_H
