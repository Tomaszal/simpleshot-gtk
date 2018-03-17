#ifndef ACTIONS_MENU_H
#define ACTIONS_MENU_H

#include <gtk/gtk.h>
#include "screenshot-app.h"

G_DECLARE_FINAL_TYPE (ActionsMenu,
                      actions_menu,
                      ACTIONS,
                      MENU,
                      GtkApplicationWindow)

ActionsMenu *actions_menu_new (GdkPixbuf *screenshot,
                               ScreenshotApp *app);

#endif // ACTIONS_MENU_H
