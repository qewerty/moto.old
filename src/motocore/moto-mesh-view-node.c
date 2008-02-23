#include "GL/gl.h"

#include "moto-mesh-view-node.h"
#include "moto-mesh.h"

/* forwards */

static void moto_mesh_view_node_draw(MotoGeometryViewNode *self);
static void moto_mesh_view_node_prepare_for_draw(MotoGeometryViewNode *self);

/* class MeshViewNode */

static GObjectClass *mesh_view_node_parent_class = NULL;

struct _MotoMeshViewNodePriv
{
    MotoMesh *mesh;
    MotoMesh **mesh_ptr;

    gboolean prepared;
    GLuint dlist;
};

static void
moto_mesh_view_node_dispose(GObject *obj)
{
    MotoMeshViewNode *self = (MotoMeshViewNode *)obj;

    g_slice_free(MotoMeshViewNodePriv, self->priv);

    G_OBJECT_CLASS(mesh_view_node_parent_class)->dispose(obj);
}

static void
moto_mesh_view_node_finalize(GObject *obj)
{
    mesh_view_node_parent_class->finalize(obj);
}

static void
moto_mesh_view_node_init(MotoMeshViewNode *self)
{
    self->priv = g_slice_new(MotoMeshViewNodePriv);

    self->priv->mesh = NULL;
    self->priv->mesh_ptr = & self->priv->mesh;

    self->priv->prepared = FALSE;
    self->priv->dlist = 0;
}

static void
moto_mesh_view_node_class_init(MotoMeshViewNodeClass *klass)
{
    GObjectClass *goclass = G_OBJECT_CLASS(klass);
    MotoGeometryViewNodeClass *gvnclass = \
        MOTO_GEOMETRY_VIEW_NODE_CLASS(klass);

    mesh_view_node_parent_class = (GObjectClass *)g_type_class_peek_parent(klass);

    goclass->dispose    = moto_mesh_view_node_dispose;
    goclass->finalize   = moto_mesh_view_node_finalize;

    gvnclass->draw              = moto_mesh_view_node_draw;
    gvnclass->prepare_for_draw  = moto_mesh_view_node_prepare_for_draw;
}

G_DEFINE_TYPE(MotoMeshViewNode, moto_mesh_view_node, MOTO_TYPE_GEOMETRY_VIEW_NODE);

/* methods of class MeshViewNode */

MotoMeshViewNode *moto_mesh_view_node_new(const gchar *name)
{
    MotoMeshViewNode *self = (MotoMeshViewNode *)g_object_new(MOTO_TYPE_MESH_VIEW_NODE, NULL);

    return self;
}

static void moto_mesh_view_node_draw(MotoGeometryViewNode *self)
{
    MotoMeshViewNode *view = (MotoMeshViewNode *)self;

    if(view->priv->mesh == NULL)
        return;

    if( ! view->priv->prepared)
        moto_mesh_view_node_prepare_for_draw(self);
    else
        glCallList(view->priv->dlist);

    /* draw */
}

static void moto_mesh_view_node_prepare_for_draw(MotoGeometryViewNode *self)
{
    MotoMeshViewNode *view = (MotoMeshViewNode *)self;
    MotoMesh *mesh = *(view->priv->mesh_ptr);

    if(mesh == NULL)
        return;

    if( ! glIsList(view->priv->dlist))
        view->priv->dlist = glGenLists(1);

    glNewList(view->priv->dlist, GL_COMPILE_AND_EXECUTE);

    int i, j;
    for(i = 0; i < mesh->faces_num; i++)
    {
        /* TODO: Temporary solution! =) */

        MotoMeshFace *face = & mesh->faces[i];

        glBegin(GL_POLYGON);

        for(j = 0; j < face->verts_num; j++)
        {
            MotoMeshVertex *vert = & mesh->verts[face->indecies[j]];

            /* TODO: attrs */

            glNormal3fv(vert->normal);
            glVertex3fv(vert->xyz);
        }

        glEnd();
    }

    glEndList();

    view->priv->prepared = TRUE;
}
