#include <stdlib.h>

#include <gio/gio.h>

#include "moto-filename.h"
#include "moto-mesh-file-node.h"
#include "moto-library.h"
#include "moto-messager.h"
#include "moto-mesh-loader.h"

/* forwards */

static gchar *moto_mesh_file_node_get_filename(MotoMeshFileNode *self);
static void moto_mesh_file_node_set_filename(MotoMeshFileNode *self, const gchar *filename);

static MotoBound *moto_mesh_file_node_get_bound(MotoGeometryNode *self);

/*
static MotoMesh *moto_mesh_file_node_get_mesh(MotoMeshFileNode *self);
*/

static void moto_mesh_file_node_load(MotoMeshFileNode *self, const gchar *filename);

static void moto_mesh_file_node_update(MotoNode *self);

/* class MeshFileNode */

static GObjectClass *mesh_file_node_parent_class = NULL;

struct _MotoMeshFileNodePriv
{
    gchar **filename_ptr;

    MotoMesh *mesh;
    MotoMesh **mesh_ptr;

    MotoBound *bound;
    gboolean bound_calculated;
};

static void
moto_mesh_file_node_dispose(GObject *obj)
{
    MotoMeshFileNode *self = (MotoMeshFileNode *)obj;

    g_object_unref((GObject *)self->priv->mesh);
    g_slice_free(MotoMeshFileNodePriv, self->priv);

    mesh_file_node_parent_class->dispose(obj);
}

static void
moto_mesh_file_node_finalize(GObject *obj)
{
    mesh_file_node_parent_class->finalize(obj);
}

static void
moto_mesh_file_node_init(MotoMeshFileNode *self)
{
    MotoNode *node = (MotoNode *)self;

    self->priv = g_slice_new(MotoMeshFileNodePriv);

    GParamSpec *pspec = NULL; // FIXME: Implement.
    moto_node_add_params(node,
            "filename", "Filename", MOTO_TYPE_FILENAME, MOTO_PARAM_MODE_INOUT, "", pspec, "General", "General",
            "mesh",   "Polygonal Mesh",   MOTO_TYPE_MESH, MOTO_PARAM_MODE_OUT, self->priv->mesh, pspec, "Geometry", "Geometry",
            NULL);

    self->priv->filename_ptr = moto_node_param_value_pointer(node, "filename", gchar*);
    self->priv->mesh_ptr = moto_node_param_value_pointer(node, "mesh", MotoMesh*);

    self->priv->bound = moto_bound_new(0, 0, 0, 0, 0, 0);
    self->priv->bound_calculated = FALSE;
}

static void
moto_mesh_file_node_class_init(MotoMeshFileNodeClass *klass)
{
    GObjectClass *goclass = (GObjectClass *)klass;
    MotoNodeClass *nclass = (MotoNodeClass *)klass;
    MotoGeometryNodeClass *gnclass = (MotoGeometryNodeClass *)klass;

    mesh_file_node_parent_class = G_OBJECT_CLASS(g_type_class_peek_parent(klass));

    gnclass->get_bound = moto_mesh_file_node_get_bound;

    goclass->dispose = moto_mesh_file_node_dispose;
    goclass->finalize = moto_mesh_file_node_finalize;

    nclass->update = moto_mesh_file_node_update;
}

G_DEFINE_TYPE(MotoMeshFileNode, moto_mesh_file_node, MOTO_TYPE_GEOMETRY_NODE);

/* methods of class MeshFileNode */

MotoMeshFileNode *moto_mesh_file_node_new()
{
    MotoMeshFileNode *self = (MotoMeshFileNode *)g_object_new(MOTO_TYPE_MESH_FILE_NODE, NULL);
    // MotoNode *node = (MotoNode *)self;

    return self;
}

static gchar *moto_mesh_file_node_get_filename(MotoMeshFileNode *self)
{
    return *(self->priv->filename_ptr);
}

static void moto_mesh_file_node_set_filename(MotoMeshFileNode *self, const gchar *filename)
{
    //g_string_assign(self->priv->filename, filename);
}

/*
static MotoMesh *moto_mesh_file_node_get_mesh(MotoMeshFileNode *self)
{
    return self->priv->mesh;
}
*/

typedef struct _TryMeshLoaderData
{
    const gchar *filename;
    MotoMesh *mesh;
} TryMeshLoaderData;

static gboolean try_mesh_loader(gpointer data, gpointer user_data)
{
    MotoMeshLoader *loader  = (MotoMeshLoader *)data;
    TryMeshLoaderData *tmld = (TryMeshLoaderData *)user_data;

    if(moto_mesh_loader_can(loader, tmld->filename))
    {
        tmld->mesh = moto_mesh_loader_load(loader, tmld->filename);
        if(tmld->mesh)
            return TRUE;
    }
    return FALSE;
}

static void moto_mesh_file_node_load(MotoMeshFileNode *self, const gchar *filename)
{
    MotoLibrary *lib = moto_node_get_library((MotoNode *)self);
    if( ! lib)
    {
        GString *msg = g_string_new("I have no library and can't load meshes. :(");
        moto_error(msg->str);
        g_string_free(msg, TRUE);
        return;
    }

    TryMeshLoaderData tmld;
    tmld.filename = filename;
    tmld.mesh = NULL;

    moto_library_foreach(lib, "mesh-loader", try_mesh_loader, & tmld);

    if(tmld.mesh)
    {
        // g_string_assign(self->priv->loaded_filename, tmld.filename);

        GString *msg = g_string_new("Mesh \"");
        g_string_append(msg, tmld.filename);
        g_string_append(msg, "\" successfully loaded.");
        moto_info(msg->str);
        g_string_free(msg, TRUE);
    }
    else
    {
        GString *msg = g_string_new("Error while loading mesh \"");
        g_string_append(msg, tmld.filename);
        g_string_append(msg, "\". I can't load it. ;(");
        moto_error(msg->str);
        g_string_free(msg, TRUE);
    }
}

static void moto_mesh_file_node_update_mesh(MotoMeshFileNode *self)
{
    gchar *filename = *(self->priv->filename_ptr);

    g_print("filename: %s\n", filename);

    GFile *f = g_file_new_for_path(filename);
    GInputStream *is = (GInputStream *)g_file_read(f, NULL, NULL);
    if( ! is)
        return;

    GString *data = g_string_new("");
    gsize count = 256;
    gsize read_bytes = 0;
    gchar buffer[256];
    while(read_bytes = g_input_stream_read(is, buffer, count, NULL, NULL))
        g_string_append_len(data, buffer, read_bytes);
    g_input_stream_close(is, NULL, NULL);

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

    g_print("MeshFile: v_num, e_num, f_num, f_v_num: %u, %u, %u, %u\n", v_num, e_num, f_num, f_v_num);

    if(self->priv->mesh)
    {
        g_object_unref(self->priv->mesh);
    }
    self->priv->mesh = moto_mesh_new(v_num, e_num, f_num, f_v_num);
    MotoMesh *mesh = self->priv->mesh;

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

                        ((gfloat *)mesh->v_coords)[j++] = (gfloat)g_strtod(p2, NULL);
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
                // f_data[f_num-1].v_num = f_v_num;
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

    g_print("MeshFile: v_num, e_num, f_num, f_v_num: %u, %u, %u, %u\n", v_num, e_num, f_num, f_v_num);
    g_print("DONE\n");

    self->priv->bound_calculated = FALSE;
    if( ! moto_mesh_prepare(mesh))
    {
        // FIXME: Error while preparing mesh.
        return;
    }
    MotoParam *pm = moto_node_get_param((MotoNode *)self, "mesh");
    g_value_set_object(moto_param_get_value(pm), mesh);
    moto_param_update_dests(pm);

}

static void moto_mesh_file_node_update(MotoNode *self)
{
    MotoMeshFileNode *mesh_file = (MotoMeshFileNode *)self;

    MotoParam *param;

    param = moto_node_get_param(self, "mesh");
    if(param && 1)//moto_param_has_dests(param)) /* FIXME */
        moto_mesh_file_node_update_mesh(mesh_file);
}

static void calc_bound(MotoMeshFileNode *self)
{
    self->priv->bound->bound[0] = 0;
    self->priv->bound->bound[1] = 0;
    self->priv->bound->bound[2] = 0;
    self->priv->bound->bound[3] = 0;
    self->priv->bound->bound[4] = 0;
    self->priv->bound->bound[5] = 0;

    if( ! self->priv->mesh)
        return;

    MotoMesh *mesh = self->priv->mesh;
    gfloat min_x = 0, max_x = 0, min_y = 0, max_y = 0, min_z = 0, max_z = 0;
    guint i;
    for(i = 0; i < mesh->v_num; i++)
    {
        if(mesh->v_coords[i].x < min_x)
            min_x = mesh->v_coords[i].x;
        if(mesh->v_coords[i].x > max_x)
            max_x = mesh->v_coords[i].x;

        if(mesh->v_coords[i].y < min_y)
            min_y = mesh->v_coords[i].y;
        if(mesh->v_coords[i].y > max_y)
            max_y = mesh->v_coords[i].y;

        if(mesh->v_coords[i].z < min_z)
            min_z = mesh->v_coords[i].z;
        if(mesh->v_coords[i].z > max_z)
            max_z = mesh->v_coords[i].z;
    }

    self->priv->bound->bound[0] = min_x;
    self->priv->bound->bound[1] = max_y;
    self->priv->bound->bound[2] = min_y;
    self->priv->bound->bound[3] = max_y;
    self->priv->bound->bound[4] = min_z;
    self->priv->bound->bound[5] = max_z;
}

static MotoBound *moto_mesh_file_node_get_bound(MotoGeometryNode *self)
{
    MotoMeshFileNode *mf = (MotoMeshFileNode *)self;

    if( ! mf->priv->bound_calculated)
        calc_bound(mf);

    return mf->priv->bound;
}

