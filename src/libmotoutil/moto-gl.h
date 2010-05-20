#ifndef __MOTO_GL_H__
#define __MOTO_GL_H__

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>

#ifdef _WIN32
    #include <GL/wglew.h>
#else
    #include <GL/glxew.h>
#endif

#include <glib.h>

G_BEGIN_DECLS

void moto_gl_init(void);
void moto_gl_printlist(void);
void moto_gl_printinfo(void);
gboolean moto_gl_is_supported(const gchar *name);

gboolean moto_gl_is_glsl_supported(void);
gboolean moto_gl_is_vbo_supported(void);

G_END_DECLS

#endif /* __MOTO_GL_H__ */
