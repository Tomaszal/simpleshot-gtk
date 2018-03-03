#ifndef SCREENSHOT_MENU_H
#define SCREENSHOT_MENU_H

#include <gtk/gtk.h>
#include "screenshot-app.h"

G_DECLARE_FINAL_TYPE (ScreenshotMenu,
                      screenshot_menu,
                      SCREENSHOT,
                      MENU,
                      GtkApplicationWindow)

ScreenshotMenu *screenshot_menu_new (ScreenshotApp *app);

#endif /* SCREENSHOT_MENU_H */
