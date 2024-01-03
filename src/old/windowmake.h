#include <gtk/gtk.h>

struct _ContactApp {
    
    GtkApplication parent;
}

G_DEFINE_TYPE(contactApp, contact_app, GTK_TYPE_APPLICATION);

static void example_app_init

ContactApp *contact_app_new (void) {

    return g_object_new (CONTACT_APP_TYPE,
			 "application-id", "com.com.com",
			 "flags", G_APPLICATION_HANDLES_OPEN,
			 NULL);
}
