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

#ifndef __MOTO_IPO_SEGMENT_H__
#define __MOTO_IPO_SEGMENT_H__

#include "moto-mesh.h"

G_BEGIN_DECLS

typedef struct _MotoIpoFunction MotoIpoFunction;
typedef struct _MotoIpoSegment MotoIpoSegment;
typedef struct _MotoIpoSegmentClass MotoIpoSegmentClass;
// typedef struct _MotoIpoSegmentPriv MotoIpoSegmentPriv;

typedef gfloat (*MotoIpoFunctionGetValueFunc)(MotoIpoSegment *ipo_segment, gfloat arg);

/* MotoIpoFunction */

struct _MotoIpoFunction
{
    GString *name;
    MotoIpoFunctionGetValueFunc get_value;
};

MotoIpoFunction *moto_ipo_function_new(const gchar *name, MotoIpoFunctionGetValueFunc func);
void moto_ipo_function_free(MotoIpoFunction *self);

/* class MotoIpoSegment */

struct _MotoIpoSegment
{
    GObject parent;

    /* <protected> */
    float _start_x, _start_y,
          _end_x, _end_y;
    float start_handle_x, start_handle_y,
          end_handle_y, end_handle_x;

    MotoIpoFunction *func_object;
    MotoIpoFunctionGetValueFunc func;
};

struct _MotoIpoSegmentClass
{
    GObjectClass parent;
};

GType moto_ipo_segment_get_type(void);

#define MOTO_TYPE_IPO_SEGMENT (moto_ipo_segment_get_type())
#define MOTO_IPO_SEGMENT(obj)  (G_TYPE_CHECK_INSTANCE_CAST ((obj), MOTO_TYPE_IPO_SEGMENT, MotoIpoSegment))
#define MOTO_IPO_SEGMENT_CLASS(klass)  (G_TYPE_CHECK_CLASS_CAST ((klass), MOTO_TYPE_IPO_SEGMENT, MotoIpoSegmentClass))
#define MOTO_IS_IPO_SEGMENT(obj)  (G_TYPE_CHECK_INSTANCE_TYPE ((obj),MOTO_TYPE_IPO_SEGMENT))
#define MOTO_IS_IPO_SEGMENT_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),MOTO_TYPE_IPO_SEGMENT))
#define MOTO_IPO_SEGMENT_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),MOTO_TYPE_IPO_SEGMENT, MotoIpoSegmentClass))

MotoIpoSegment *moto_ipo_segment_new(const gchar *func_name);

gfloat moto_ipo_segment_get_value(MotoIpoSegment *self, gfloat arg);
void moto_ipo_segment_set_function(MotoIpoSegment *self, const gchar *func_name);
gchar *moto_ipo_segment_get_function_name(MotoIpoSegment *self);

G_END_DECLS

#endif /* __MOTO_IPO_SEGMENT_H__ */
