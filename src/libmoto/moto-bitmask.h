/* ##################################################################################
#
#  Moto Animation System (http://motoanim.sf.net)
#  Copyleft (C) 2008 Konstantin Evdokimenko a.k.a Qew[erty] (qewerty@gmail.com)
#
#  This program is free software; you can redistribute it and/or
#  modify it under the terms of the GNU General Public License
#  as published by the Free Software Foundation; either version 2
#  of the License, or (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
#
################################################################################## */

#ifndef MOTO_BITMASK_H
#define MOTO_BITMASK_H

#include <glib.h>

G_BEGIN_DECLS

typedef struct _MotoBitmask MotoBitmask;

struct _MotoBitmask
{
    guint32 *bits;

    guint32 bits_num;
    guint32 set_num;
};

MotoBitmask *moto_bitmask_new(guint32 bits_num);
void moto_bitmask_free(MotoBitmask *self);

MotoBitmask *moto_bitmask_new_copy(MotoBitmask *self);
void moto_bitmask_copy(MotoBitmask *self, MotoBitmask *other);
void moto_bitmask_copy_smth(MotoBitmask *self, MotoBitmask *other);

gboolean moto_bitmask_is_set(MotoBitmask *self, guint32 index);
void moto_bitmask_set(MotoBitmask *self, guint32 index);
void moto_bitmask_set_all(MotoBitmask *self);
void moto_bitmask_unset(MotoBitmask *self, guint32 index);
void moto_bitmask_unset_all(MotoBitmask *self);
void moto_bitmask_toggle(MotoBitmask *self, guint32 index);
void moto_bitmask_inverse(MotoBitmask *self);

guint32 moto_bitmask_get_bits_num(MotoBitmask *self);
guint32 moto_bitmask_get_set_num(MotoBitmask *self);
guint32 moto_bitmask_calc_set_num(MotoBitmask *self);

gboolean moto_bitmask_is_set_fast(MotoBitmask *self, guint32 index);
void moto_bitmask_set_fast(MotoBitmask *self, guint32 index);
void moto_bitmask_unset_fast(MotoBitmask *self, guint32 index);
void moto_bitmask_toggle_fast(MotoBitmask *self, guint32 index);

guint32* moto_bitmask_create_array_32(MotoBitmask* self);
guint16* moto_bitmask_create_array_16(MotoBitmask* self);

G_END_DECLS

#endif /* MOTO_BITMASK_H */
