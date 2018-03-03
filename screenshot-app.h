#ifndef SCREENSHOT_APP_H
#define SCREENSHOT_APP_H

#include <gtk/gtk.h>

G_DECLARE_FINAL_TYPE (ScreenshotApp,
                      screenshot_app,
                      SCREENSHOT,
                      APP,
                      GtkApplication)

ScreenshotApp *screenshot_app_new (void);

#endif // SCREENSHOT_APP_H
