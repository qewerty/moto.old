#include <limits.h>
#include <string.h>

#include "libmotoutil/numdef.h"
#include "moto-bitmask.h"

#define DWORDS_NUM(bits_num) (((bits_num) >> 5) + 1)
#define BYTES_NUM(bits_num)  (DWORDS_NUM(bits_num) << 4)

inline static MotoBitmask*
moto_bitmask_new_uninitialized(guint32 bits_num)
{
    MotoBitmask* self = g_slice_new(MotoBitmask);
    if( ! self)
        return NULL;

    self->bits_num = bits_num;
    self->set_num = 0;

    self->bits = (guint32*)g_try_malloc(BYTES_NUM(bits_num));
    if( ! self->bits)
    {
        g_slice_free(MotoBitmask, self);
        return NULL;
    }

    return self;
}

MotoBitmask*
moto_bitmask_new(guint32 bits_num)
{
    MotoBitmask* self = moto_bitmask_new_uninitialized(bits_num);
    if( ! self)
        return NULL;

    memset(self->bits, 0, BYTES_NUM(bits_num));

    return self;
}

void moto_bitmask_free(MotoBitmask *self)
{
    g_free(self->bits);
    g_slice_free(MotoBitmask, self);
}

MotoBitmask *moto_bitmask_new_copy(MotoBitmask *self)
{
    MotoBitmask* copy = moto_bitmask_new_uninitialized(self->bits_num);
    if( ! copy)
        return NULL;

    moto_bitmask_copy(copy, self);
    return copy;
}

void moto_bitmask_copy(MotoBitmask *self, MotoBitmask *other)
{
    memcpy(self->bits, other->bits, BYTES_NUM(min(self->bits_num, other->bits_num)));
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
    self->set_num = self->bits_num;
    memset(self->bits, UCHAR_MAX, BYTES_NUM(self->bits_num));
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
    self->set_num = 0;
    memset(self->bits, 0, BYTES_NUM(self->bits_num));
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
    guint32 num = DWORDS_NUM(self->bits_num);
    guint32 i;
    for(i = 0; i < num; i++)
        self->bits[i] = ~ self->bits[i];
    self->set_num = self->bits_num - self->set_num;
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
