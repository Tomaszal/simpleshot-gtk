#include <gtk/gtk.h>

#include "screenshot-app.h"

int
main (int argc, char **argv)
{
	return g_application_run (G_APPLICATION (screenshot_app_new ()), argc, argv);
}
