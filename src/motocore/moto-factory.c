#include "moto-messager.h"
#include "moto-factory.h"

void moto_factory_init(MotoFactory *self,
        MotoFactoryCreateFunc create_object, MotoFactoryFreeFunc free_object,
        gpointer user_data)
{
    self->create_object = create_object;
    self->free_object = free_object;

    self->user_data = user_data;

    g_datalist_init(& self->dl);
}

void moto_factory_free_all(MotoFactory *self)
{
    if( ! self->free_object)
    {
        moto_error("MotoFactory instance has no \"free_object\" function set. I can't free memory of objects.");
        return;
    }

    g_datalist_foreach(& self->dl, self->free_object, self->user_data);
}

gpointer moto_factory_get(MotoFactory *self, const gchar *key)
{
    if( ! self->create_object)
    {
        GString *msg = g_string_new("");
        g_string_printf(msg, "MotoFactory instance has no \"create_object\" function set. I can't create object. Key is \"%s\"", key);
        moto_error(msg->str);
        g_string_free(msg, TRUE);
        return NULL;
    }

    gpointer data = g_datalist_get_data(& self->dl, key);
    if( ! data)
    {
        data = self->create_object(self->user_data);
        g_datalist_set_data(& self->dl, key, data);
    }

    return data;
}
