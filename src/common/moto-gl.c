#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "moto-glext.h"

#define INDENT "    "

void moto_glext_print_list(void)
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

gboolean moto_glext_is_supported(const gchar *name)
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
