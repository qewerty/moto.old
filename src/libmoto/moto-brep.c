#include "moto-brep.h"
#include "moto-types.h"

/* class MotoBRep */

typedef struct _MotoBRepPriv
{
    MotoBRepCreator *creator;
    gboolean empty;
} MotoBRepPriv;

#define MOTO_BREP_GET_PRIVATE(obj) \
    G_TYPE_INSTANCE_GET_PRIVATE(obj, MOTO_TYPE_BREP, MotoBRepPriv)

static GObjectClass *brep_parent_class = NULL;

static void
moto_brep_init(MotoBRep *self)
{
    MotoBRepPriv *priv = MOTO_BREP_GET_PRIVATE(self);

    priv->empty = TRUE;
}

static void
moto_brep_class_init(MotoBRepClass *klass)
{
    brep_parent_class = (GObjectClass *)g_type_class_peek_parent(klass);

    klass->clear = NULL;

    g_type_class_add_private(klass, sizeof(MotoBRepPriv));
}

G_DEFINE_ABSTRACT_TYPE(MotoBRep, moto_brep, G_TYPE_INITIALLY_UNOWNED);

/* Methods of class MotoBRep */

void moto_brep_clear(MotoBRep *self)
{
    MotoBRepClass *klass = MOTO_BREP_GET_CLASS(self);

    if(klass->clear)
        klass->clear(self);

    MOTO_BREP_GET_PRIVATE(self)->empty = TRUE;
}

gboolean moto_brep_is_empty(MotoBRep *self)
{
    return MOTO_BREP_GET_PRIVATE(self)->empty;
}

void moto_brep_update(MotoBRep *self)
{
    MotoBRepPriv *priv = MOTO_BREP_GET_PRIVATE(self);

    priv->empty = FALSE;
    // priv->empty = moto_brep_creator_update_brep(priv->creator, self);
}
