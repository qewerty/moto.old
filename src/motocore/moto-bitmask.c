#include <string.h>

#include "moto-bitmask.h"

static MotoBitmask*
moto_bitmask_new_uninitialized(guint32 bits_num)
{
    MotoBitmask *self = g_slice_new(MotoBitmask);

    self->bits_num = bits_num;
    self->set_num = 0;

    guint num = bits_num/32 + 1;
    self->bits = (guint32 *)g_try_malloc(num * sizeof(guint32));
    if( ! self->bits)
        ; // TODO

    return self;
}

MotoBitmask *moto_bitmask_new(guint32 bits_num)
{
    MotoBitmask *self = moto_bitmask_new_uninitialized(bits_num);

    guint num = bits_num/32 + 1;
    memset(self->bits, 0, num*4);

    return self;
}

void moto_bitmask_free(MotoBitmask *self)
{
    g_free(self->bits);
    g_slice_free(MotoBitmask, self);
}

MotoBitmask *moto_bitmask_new_copy(MotoBitmask *self)
{
    MotoBitmask *copy = moto_bitmask_new_uninitialized(self->bits_num);

    memcpy(copy->bits, self->bits, self->bits_num*4);

    return copy;
}

void moto_bitmask_copy(MotoBitmask *self, MotoBitmask *other)
{
    // TODO
}

void moto_bitmask_copy_smth(MotoBitmask *self, MotoBitmask *other)
{
    // TODO
}

gboolean moto_bitmask_is_set(MotoBitmask *self, guint32 index)
{
    if(index >= self->bits_num)
        return FALSE;

    return *(self->bits + (index/32)) & (1 << (index % 32));
}

void moto_bitmask_set(MotoBitmask *self, guint32 index)
{
    if(index >= self->bits_num)
        return;

    if( ! moto_bitmask_is_set_fast(self, index))
        self->set_num++;
    *(self->bits + (index/32)) |= 1 << (index % 32);
}

void moto_bitmask_unset(MotoBitmask *self, guint32 index)
{
    if(index >= self->bits_num)
        return;

    if(moto_bitmask_is_set_fast(self, index))
        self->set_num--;
    *(self->bits + (index/32)) &= ~(1 << (index % 32));
}

void moto_bitmask_toggle(MotoBitmask *self, guint32 index)
{
    if(moto_bitmask_is_set(self, index))
        moto_bitmask_unset_fast(self, index);
    else
        moto_bitmask_set(self, index);
}

gboolean moto_bitmask_is_set_fast(MotoBitmask *self, guint32 index)
{
    return *(self->bits + (index/32)) & (1 << (index % 32));
}

void moto_bitmask_set_fast(MotoBitmask *self, guint32 index)
{
    if( ! moto_bitmask_is_set_fast(self, index))
        self->set_num++;
    *(self->bits + (index/32)) |= 1 << (index % 32);
}

void moto_bitmask_unset_fast(MotoBitmask *self, guint32 index)
{
    if(moto_bitmask_is_set_fast(self, index))
        self->set_num--;
    *(self->bits + (index/32)) &= ~(1 << (index % 32));
}

void moto_bitmask_toggle_fast(MotoBitmask *self, guint32 index)
{
    if(moto_bitmask_is_set_fast(self, index))
        moto_bitmask_unset_fast(self, index);
    else
        moto_bitmask_set_fast(self, index);
}
