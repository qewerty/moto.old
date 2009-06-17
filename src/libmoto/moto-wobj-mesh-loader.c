#include <gio/gio.h>

#include "moto-wobj-mesh-loader.h"

/* forwards */

GSList *moto_wobj_mesh_loader_get_extentions(MotoMeshLoader *self);
MotoMesh *moto_wobj_mesh_loader_load(MotoMeshLoader *self, const gchar *filename);

/* class WobjMeshLoader */

static GObjectClass *wobj_mesh_loader_parent_class = NULL;

struct _MotoWobjMeshLoaderPriv
{
    GSList *extensions;
};

static void
free_gstring(gpointer data, gpointer user_data)
{
    g_string_free((GString *)data, TRUE);
}

static void
moto_wobj_mesh_loader_dispose(GObject *obj)
{
    MotoWobjMeshLoader *wobj = (MotoWobjMeshLoader *)obj;

    g_slist_foreach(wobj->priv->extensions, free_gstring, NULL);
    g_slist_free(wobj->priv->extensions);

    wobj_mesh_loader_parent_class->dispose(obj);
}

static void
moto_wobj_mesh_loader_finalize(GObject *obj)
{
    wobj_mesh_loader_parent_class->finalize(obj);
}

static void
moto_wobj_mesh_loader_init(MotoWobjMeshLoader *self)
{
    self->priv = g_slice_new(MotoWobjMeshLoaderPriv);

    self->priv->extensions = NULL;
    self->priv->extensions = \
        g_slist_append(self->priv->extensions, g_string_new(".obj"));
}

static void
moto_wobj_mesh_loader_class_init(MotoWobjMeshLoaderClass *klass)
{
    GObjectClass *goclass = (GObjectClass *)klass;
    MotoMeshLoaderClass *mlclass = (MotoMeshLoaderClass *)klass;

    wobj_mesh_loader_parent_class = (GObjectClass *)g_type_class_peek_parent(klass);

    goclass->dispose    = moto_wobj_mesh_loader_dispose;
    goclass->finalize   = moto_wobj_mesh_loader_finalize;

    mlclass->get_extensions   = moto_wobj_mesh_loader_get_extentions;
    // klass->can              = moto_wobj_mesh_loader_can;
    mlclass->load             = moto_wobj_mesh_loader_load;
}

G_DEFINE_TYPE(MotoWobjMeshLoader, moto_wobj_mesh_loader, MOTO_TYPE_MESH_LOADER);

/* Methods of class WobjMeshLoader */

MotoMeshLoader *moto_wobj_mesh_loader_new()
{
    MotoMeshLoader *self = (MotoMeshLoader *)g_object_new(MOTO_TYPE_WOBJ_MESH_LOADER, NULL);
    // MotoWobjMeshLoader *wobj = (MotoWobjMeshLoader *)self;

    return self;
}

GSList *moto_wobj_mesh_loader_get_extentions(MotoMeshLoader *self)
{

    return ((MotoWobjMeshLoader *)self)->priv->extensions;
}

/*
gboolean moto_wobj_mesh_loader_can(MotoWobjMeshLoader *self, const gchar *filename)
{


    return FALSE;
}
*/

GString *moto_gstring_from_file(const gchar *filename)
{
    GFile *f = g_file_new_for_path(filename);
    GInputStream *is = (GInputStream *)g_file_read(f, NULL, NULL);
    if( ! is)
        return NULL;

    GString *data = g_string_new("");
    gsize count = 1024;
    gsize read_bytes = 0;
    gchar buffer[count];
    while(read_bytes = g_input_stream_read(is, buffer, count, NULL, NULL))
        g_string_append_len(data, buffer, read_bytes);
    g_input_stream_close(is, NULL, NULL);

    return data;
}

MotoMesh *moto_wobj_mesh_loader_load(MotoMeshLoader *self, const gchar *filename)
{
    GString *data = moto_gstring_from_file(filename);

    guint v_num   = 0,
          e_num   = 0, // WARNING! Initially without edges. Edges will be calculated while preparing mesh.
          f_num   = 0,
          f_v_num = 0;

    gulong i;
    gchar *p = data->str;
    for(i = 0; i < data->len; i++)
    {
        gchar last = data->str[i];
        if('\n' == last)
        {
            if('v' == p[0])
                v_num++;
            if('f' == p[0])
            {
                f_num++;

                gchar *pp = p+1;
                do
                {
                    if(' ' != *pp)
                    {
                        f_v_num++;

                        while(' ' != *pp || '\n' == *pp)
                        {
                            if('\n' == *pp)
                                break;
                            pp++;
                        }
                    }
                    if('\n' == *pp)
                        break;
                    pp++;
                }
                while('\n' != *pp);
            }

            p = data->str + i + 1;
        }
    }

    MotoMesh *mesh = moto_mesh_new(v_num, e_num, f_num, f_v_num);

    GArray *f_verts = g_array_sized_new(FALSE, FALSE, sizeof(guint32), 10);

    v_num = 0;
    guint32 f_offset = 0;
    f_v_num = 0;
    f_num = 0;
    p = data->str;
    guint j = 0;
    for(i = 0; i < data->len; i++)
    {
        gchar last = data->str[i];
        if('\n' == last)
        {
            if('v' == p[0])
            {
                v_num++;
                gchar *p2 = p+1;
                do
                {
                    if(' ' != *p2)
                    {

                        ((gfloat *)mesh->v_coords)[j++] = (gfloat)g_ascii_strtod(p2, NULL);
                        while(' ' != *p2 || '\n' == *p2)
                        {
                            if('\n' == *p2)
                                break;
                            p2++;
                        }
                    }
                    if('\n' == *p2)
                        break;
                    p2++;
                }
                while('\n' != *p2);
            }
            if('f' == p[0])
            {
                f_num++;
                gchar *p2 = p+1;
                do
                {
                    if(' ' != *p2)
                    {

                        guint32 vi = (guint32)strtoul(p2, NULL, 10)-1;
                        g_array_append_val(f_verts, vi);
                        f_offset++;
                        f_v_num++;
                        while(' ' != *p2 || '\n' == *p2)
                        {
                            if('\n' == *p2)
                                break;
                            p2++;
                        }
                    }
                    if('\n' == *p2)
                        break;
                    p2++;
                }
                while('\n' != *p2);
                if( ! moto_mesh_set_face(mesh, f_num-1, f_offset, (guint32*)f_verts->data))
                    ; // FIXME: Error
                g_array_remove_range(f_verts, 0, f_v_num);
                f_v_num = 0;
            }

            p = data->str + i + 1;
        }
    }
    g_string_free(data, TRUE);
    g_array_free(f_verts, TRUE);

    if( ! moto_mesh_prepare(mesh))
    {
        // FIXME: Error while preparing mesh.
        g_object_unref(mesh);
        return NULL;
    }

    return mesh;
}
