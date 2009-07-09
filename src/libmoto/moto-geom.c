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

    g_type_class_add_private(klass, sizeof(MotoGeomPriv));
}

G_DEFINE_ABSTRACT_TYPE(MotoGeom, moto_geom, G_TYPE_INITIALLY_UNOWNED);

/* Methods of class MotoGeom */

void moto_geom_update(MotoGeom *self)
{

}
