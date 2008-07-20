#ifdef _WIN32
    #include <windows.h>
#endif

#include <GL/gl.h>
#include <GL/glext.h>

#include <glib.h>

void moto_glext_print_list(void);
gboolean moto_glext_is_supported(const gchar *name);
