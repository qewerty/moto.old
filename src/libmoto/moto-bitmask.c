#include <limits.h>
#include <string.h>

#include "libmotoutil/numdef.h"
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

    moto_bitmask_copy(copy, self);

    return copy;
}

void moto_bitmask_copy(MotoBitmask *self, MotoBitmask *other)
{
    guint32 num = min(self->bits_num, other->bits_num) / 32 + 1;
    memcpy(self->bits, other->bits, num*4);
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

void moto_bitmask_set_all(MotoBitmask *self)
{
    memset(self->bits, UCHAR_MAX, self->bits_num*4);
}

void moto_bitmask_unset(MotoBitmask *self, guint32 index)
{
    if(index >= self->bits_num)
        return;

    if(moto_bitmask_is_set_fast(self, index))
        self->set_num--;
    *(self->bits + (index/32)) &= ~(1 << (index % 32));
}

void moto_bitmask_unset_all(MotoBitmask *self)
{
    memset(self->bits, 0, self->bits_num*4);
}

void moto_bitmask_toggle(MotoBitmask *self, guint32 index)
{
    if(moto_bitmask_is_set(self, index))
        moto_bitmask_unset_fast(self, index);
    else
        moto_bitmask_set(self, index);
}

void moto_bitmask_inverse(MotoBitmask *self)
{
    guint32 num = self->bits_num/32 + 1;
    guint32 i;
    for(i = 0; i < num; i++)
        self->bits[i] = ~ self->bits[i];
}

guint32 moto_bitmask_get_bits_num(MotoBitmask *self)
{
    return self->bits_num;
}

guint32 moto_bitmask_get_set_num(MotoBitmask *self)
{
    return self->set_num;
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
