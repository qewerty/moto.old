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

#ifndef __MOTO_VARIATION_H__
#define __MOTO_VARIATION_H__

#include "moto-forward.h"
#include "moto-node.h"

G_BEGIN_DECLS

/* class MotoVariation */

struct _MotoVariation
{
    GObject parent;
};

struct _MotoVariationClass
{
    GObjectClass parent;
};

GType moto_variation_get_type(void);

#define MOTO_TYPE_VARIATION (moto_variation_get_type())
#define MOTO_VARIATION(obj)  (G_TYPE_CHECK_INSTANCE_CAST ((obj), MOTO_TYPE_VARIATION, MotoVariation))
#define MOTO_VARIATION_CLASS(klass)  (G_TYPE_CHECK_CLASS_CAST ((klass), MOTO_TYPE_VARIATION, MotoVariationClass))
#define MOTO_IS_VARIATION(obj)  (G_TYPE_CHECK_INSTANCE_TYPE ((obj),MOTO_TYPE_VARIATION))
#define MOTO_IS_VARIATION_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),MOTO_TYPE_VARIATION))
#define MOTO_VARIATION_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),MOTO_TYPE_VARIATION, MotoVariationClass))

MotoVariation *moto_variation_new(const gchar *name);

MotoVariation *moto_variation_get_parent(MotoVariation *self);
void moto_variation_set_parent(MotoVariation *self, MotoVariation *parent);

void moto_variation_save_param(MotoVariation *self, MotoParam *p);
void moto_variation_restore_param(MotoVariation *self, MotoParam *p);

G_END_DECLS

#endif /* __MOTO_VARIATION_H__ */
