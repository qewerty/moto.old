#include <limits.h>
#include <string.h>

#include "libmotoutil/numdef.h"
#include "moto-bitmask.h"

#define DWORDS_NUM(bits_num) (((bits_num) >> 5) + 1)
#define BYTES_NUM(bits_num)  (DWORDS_NUM(bits_num) << 2)

inline static MotoBitmask*
moto_bitmask_new_uninitialized(guint32 bits_num)
{
    MotoBitmask* self = g_slice_new(MotoBitmask);
    if(G_UNLIKELY( ! self))
        return NULL;

    self->bits_num = bits_num;
    self->set_num = 0;

    self->bits = (guint32*)g_try_malloc(BYTES_NUM(bits_num));
    if(G_UNLIKELY( ! self->bits))
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
    if(G_UNLIKELY( ! self))
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
    if(G_UNLIKELY( ! copy))
        return NULL;

    moto_bitmask_copy(copy, self);
    return copy;
}

void moto_bitmask_copy(MotoBitmask *self, MotoBitmask *other)
{
    self->set_num = other->set_num;
    memcpy(self->bits, other->bits, BYTES_NUM(self->bits_num));
}

void moto_bitmask_copy_smth(MotoBitmask *self, MotoBitmask *other)
{
    if(G_UNLIKELY(self->bits_num == other->bits_num))
    {
        moto_bitmask_copy(self, other);
    }
    else
    {
        guint min_bits_num   = min(self->bits_num, other->bits_num);
        guint full_bytes_num = BYTES_NUM(min_bits_num) - sizeof(guint32);
        guint full_bits_num  = full_bytes_num << 3;

        memset(self->bits, 0, BYTES_NUM(self->bits_num));
        memcpy(self->bits, other->bits, full_bytes_num);
        guint i;
        for(i = full_bits_num; i < min_bits_num; i++)
        {
            if(moto_bitmask_is_set_fast(other, i))
                *(self->bits + (i/32)) |= 1 << (i % 32);
        }
        self->set_num = moto_bitmask_calc_set_num(self);
    }
}

gboolean moto_bitmask_is_set(MotoBitmask *self, guint32 index)
{
    if(G_UNLIKELY(index >= self->bits_num))
        return FALSE;

    return *(self->bits + (index/32)) & (1 << (index % 32));
}

void moto_bitmask_set(MotoBitmask *self, guint32 index)
{
    if(G_UNLIKELY(index >= self->bits_num))
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
    if(G_UNLIKELY(index >= self->bits_num))
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

guint32 moto_bitmask_calc_set_num(MotoBitmask *self)
{
    guint32 num = 0;
    guint32 i;
    for(i = 0; i < self->bits_num; i++)
        num += (moto_bitmask_is_set_fast(self, i)) ? 1 : 0;
    return num;
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

guint32* moto_bitmask_create_array_32(MotoBitmask* self)
{
    guint32* array = (guint32*)g_try_malloc(sizeof(guint32)*self->set_num);

    guint32 i, j = 0;
    for(i = 0; i < self->bits_num; i++)
        if(moto_bitmask_is_set_fast(self, i))
            array[j++] = i;

    return array;
}

guint16* moto_bitmask_create_array_16(MotoBitmask* self)
{
    guint16* array = (guint16*)g_try_malloc(sizeof(guint16)*self->set_num);

    guint16 i, j = 0;
    for(i = 0; i < self->bits_num; i++)
        if(moto_bitmask_is_set_fast(self, i))
            array[j++] = i;

    return array;
}
