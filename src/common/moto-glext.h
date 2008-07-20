#ifdef _WIN32
    #include <windows.h>
#else
    #define GLX_GLXEXT_LEGACY
#endif

#include <GL/gl.h>
#include <GL/glext.h>

#ifdef _WIN32
    #include <wglext.h>
#else
    #include <GL/glx.h>
    #include <GL/glxext.h>
#endif

#include <glib.h>

void moto_glext_print_list(void);
void moto_glext_print_info(void);
gboolean moto_glext_is_supported(const gchar *name);
void moto_glext_init(void);
