#include "moto-shape.h"
#include "moto-types.h"

/* class MotoShape */

static GObjectClass *geom_parent_class = NULL;

static void
moto_shape_init(MotoShape *self)
{}

static void
moto_shape_class_init(MotoShapeClass *klass)
{
    geom_parent_class = (GObjectClass *)g_type_class_peek_parent(klass);

    klass->prepare = NULL;
    klass->is_struct_the_same = NULL;
}

G_DEFINE_ABSTRACT_TYPE(MotoShape, moto_shape, G_TYPE_INITIALLY_UNOWNED);

/* Methods of class MotoShape */

gboolean moto_shape_prepare(MotoShape *self)
{
    MotoShapeClass *klass = MOTO_SHAPE_GET_CLASS(self);

    if(klass->prepare)
        return klass->prepare(self);

    return FALSE;
}

gboolean moto_shape_is_struct_the_same(MotoShape *self, MotoShape *other)
{
    MotoShapeClass *klass = MOTO_SHAPE_GET_CLASS(self);

    if(klass->is_struct_the_same)
        return klass->is_struct_the_same(self, other);

    return FALSE;
}

/* MeshSelection */

MotoShapeSelection *moto_shape_selection_new(guint v_num, guint e_num, guint f_num)
{
    MotoShapeSelection *self = \
        (MotoShapeSelection *)g_try_malloc(sizeof(MotoShapeSelection));

    self->verts = moto_bitmask_new(v_num);
    self->edges = moto_bitmask_new(e_num);
    self->faces = moto_bitmask_new(f_num);

    return self;
}

MotoShapeSelection *moto_shape_selection_copy(MotoShapeSelection *other)
{
    MotoShapeSelection *self = \
        moto_shape_selection_new(moto_shape_selection_get_v_num(other),
                                moto_shape_selection_get_e_num(other),
                                moto_shape_selection_get_f_num(other));

    moto_bitmask_copy(self->verts, other->verts);
    moto_bitmask_copy(self->edges, other->edges);
    moto_bitmask_copy(self->faces, other->faces);

    return self;
}

void moto_shape_selection_copy_smth(MotoShapeSelection *self, MotoShapeSelection *other)
{
    moto_bitmask_copy_smth(self->verts, other->verts);
    moto_bitmask_copy_smth(self->edges, other->edges);
    moto_bitmask_copy_smth(self->faces, other->faces);
}

void moto_shape_selection_free(MotoShapeSelection *self)
{
    moto_bitmask_free(self->verts);
    moto_bitmask_free(self->edges);
    moto_bitmask_free(self->faces);
    g_free(self);
}

guint32 moto_shape_selection_get_v_num(MotoShapeSelection *self)
{
    return moto_bitmask_get_bits_num(self->verts);
}

guint32 moto_shape_selection_get_e_num(MotoShapeSelection *self)
{
    return moto_bitmask_get_bits_num(self->edges);
}

guint32 moto_shape_selection_get_f_num(MotoShapeSelection *self)
{
    return moto_bitmask_get_bits_num(self->faces);
}

guint32 moto_shape_selection_get_selected_v_num(MotoShapeSelection *self)
{
    return moto_bitmask_get_set_num(self->verts);
}

guint32 moto_shape_selection_get_selected_e_num(MotoShapeSelection *self)
{
    return moto_bitmask_get_set_num(self->edges);
}

guint32 moto_shape_selection_get_selected_f_num(MotoShapeSelection *self)
{
    return moto_bitmask_get_set_num(self->faces);
}

/* vertex selection */

void moto_shape_selection_select_vertex(MotoShapeSelection *self, guint index)
{
    moto_bitmask_set(self->verts, index);
}

void moto_shape_selection_select_verts(MotoShapeSelection *self, ...)
{
    va_list ap;
    va_start(ap, self);

    while(1)
    {
        guint32 index = va_arg(ap, guint);
        if(G_MAXUINT32 == index)
            break;

        moto_shape_selection_select_vertex(self, index);
    }

    va_end(ap);
}

void moto_shape_selection_deselect_vertex(MotoShapeSelection *self, guint index)
{
    moto_bitmask_unset(self->verts, index);
}

void moto_shape_selection_deselect_verts(MotoShapeSelection *self, ...)
{
    va_list ap;
    va_start(ap, self);

    while(1)
    {
        guint32 index = va_arg(ap, guint);
        if(G_MAXUINT32 == index)
            break;

        moto_shape_selection_deselect_vertex(self, index);
    }

    va_end(ap);
}

void moto_shape_selection_deselect_all_verts(MotoShapeSelection *self)
{
    moto_bitmask_unset_all(self->verts);
}

void moto_shape_selection_toggle_vertex_selection(MotoShapeSelection *self, guint index)
{
    moto_bitmask_toggle(self->verts, index);
}

gboolean moto_shape_selection_is_vertex_selected(MotoShapeSelection *self, guint index)
{
    return moto_bitmask_is_set(self->verts, index);
}

/* edge selection */

void moto_shape_selection_select_edge(MotoShapeSelection *self, guint index)
{
    moto_bitmask_set(self->edges, index);
}

void moto_meshV_selection_select_edges(MotoShapeSelection *self, ...)
{
    va_list ap;
    va_start(ap, self);

    while(1)
    {
        guint32 index = va_arg(ap, guint);
        if(G_MAXUINT32 == index)
            break;

        moto_shape_selection_select_edge(self, index);
    }

    va_end(ap);
}

void moto_shape_selection_deselect_edge(MotoShapeSelection *self, guint index)
{
    moto_bitmask_unset(self->edges, index);
}

void moto_shape_selection_deselect_edges(MotoShapeSelection *self, ...)
{
    va_list ap;
    va_start(ap, self);

    while(1)
    {
        guint32 index = va_arg(ap, guint);
        if(G_MAXUINT32 == index)
            break;

        moto_shape_selection_deselect_edge(self, index);
    }

    va_end(ap);
}

void moto_shape_selection_deselect_all_edges(MotoShapeSelection *self)
{
    moto_bitmask_unset_all(self->edges);
}

void moto_shape_selection_toggle_edge_selection(MotoShapeSelection *self, guint index)
{
    moto_bitmask_toggle(self->edges, index);
}

gboolean moto_shape_selection_is_edge_selected(MotoShapeSelection *self, guint index)
{
    return moto_bitmask_is_set(self->edges, index);
}

/* face selection */

void moto_shape_selection_select_face(MotoShapeSelection *self, guint index)
{
    moto_bitmask_set(self->faces, index);
}

void moto_shape_selection_select_faces(MotoShapeSelection *self, ...)
{
    va_list ap;
    va_start(ap, self);

    while(1)
    {
        guint32 index = va_arg(ap, guint);
        if(G_MAXUINT32 == index)
            break;

        moto_shape_selection_select_face(self, index);
    }

    va_end(ap);
}

void moto_shape_selection_deselect_face(MotoShapeSelection *self, guint index)
{
    return moto_bitmask_unset(self->faces, index);
}

void moto_shape_selection_deselect_faces(MotoShapeSelection *self, ...)
{
    va_list ap;
    va_start(ap, self);

    while(1)
    {
        guint32 index = va_arg(ap, guint);
        if(G_MAXUINT32 == index)
            break;

        moto_shape_selection_deselect_face(self, index);
    }

    va_end(ap);
}

void moto_shape_selection_deselect_all_faces(MotoShapeSelection *self)
{
    return moto_bitmask_unset_all(self->faces);
}

void moto_shape_selection_toggle_face_selection(MotoShapeSelection *self, guint index)
{
    moto_bitmask_toggle(self->faces, index);
}

gboolean moto_shape_selection_is_face_selected(MotoShapeSelection *self, guint index)
{
    return moto_bitmask_is_set(self->faces, index);
}

void moto_shape_selection_deselect_all(MotoShapeSelection *self)
{
    moto_shape_selection_deselect_all_verts(self);
    moto_shape_selection_deselect_all_edges(self);
    moto_shape_selection_deselect_all_faces(self);
}
