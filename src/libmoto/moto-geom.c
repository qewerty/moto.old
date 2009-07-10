#include "moto-geom.h"
#include "moto-types.h"

/* class MotoGeom */

typedef struct _MotoGeomPriv
{
    MotoGeomCreator *creator;
} MotoGeomPriv;

static GObjectClass *geom_parent_class = NULL;

static void
moto_geom_init(MotoGeom *self)
{}

static void
moto_geom_class_init(MotoGeomClass *klass)
{
    geom_parent_class = (GObjectClass *)g_type_class_peek_parent(klass);

    klass->prepare = NULL;

    // g_type_class_add_private(klass, sizeof(MotoGeomPriv));
}

G_DEFINE_ABSTRACT_TYPE(MotoGeom, moto_geom, G_TYPE_INITIALLY_UNOWNED);

/* Methods of class MotoGeom */

gboolean moto_geom_prepare(MotoGeom *self)
{
    MotoGeomClass *klass = MOTO_GEOM_GET_CLASS(self);

    if(klass->prepare)
        return klass->prepare(self);

    return FALSE;
}

