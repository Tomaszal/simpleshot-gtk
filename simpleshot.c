#include <gtk/gtk.h>

#include "screenshot-app.h"

int
main (int argc, char **argv)
{
	// Set schema location for testing
	g_setenv ("GSETTINGS_SCHEMA_DIR", ".", FALSE);

	return g_application_run (G_APPLICATION (screenshot_app_new ()), argc, argv);
}
