#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "moto-gl.h"

#define INDENT "    "

void moto_gl_init(void)
{
    static gboolean inited = FALSE;
    if( ! inited)
    {
        if(glewInit() != GLEW_OK)
        {
            printf("[Error] Can't initialize GLEW!\n");
            exit(-1);
        }
        inited = TRUE;
    }
}

void moto_gl_print_list(void)
{
    const gchar *extensions = (const gchar *)glGetString(GL_EXTENSIONS);

    gchar name[1024];
    gint i, j;

    printf("Supported OpenGL extensions:\n");
    for(i = 0, j = 0; extensions[i] != '\0'; i++)
    {
        if( ! isspace(extensions[i]))
        {
            name[j++] = extensions[i];
        }
        else
        {
            name[j] = '\0';
            printf("%s%s\n", INDENT, name);
            j = 0;
        }
    }
    if(j > 0)
    {
        name[j] = '\0';
        printf("%s%s\n", INDENT, name);
    }
}

gboolean moto_gl_is_supported(const gchar *name)
{
    const gchar *extensions = (const gchar *)glGetString(GL_EXTENSIONS);
    const gchar *start = extensions;
    const gchar *ptr;

    while((ptr = strstr(start, name)) != NULL)
    {
        const gchar *end = ptr + strlen(name);
        if(isspace(*end) || *end == '\0')
            return TRUE;
        start = end;
    }

    return FALSE;
}

gboolean moto_gl_is_glsl_100_supported()
{
    return GLEW_ARB_shader_objects && GLEW_ARB_vertex_shader && GLEW_ARB_fragment_shader && GLEW_ARB_shading_language_100;
}

gboolean moto_gl_is_vbo_supported()
{
    return GLEW_ARB_vertex_buffer_object;
}
