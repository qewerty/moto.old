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

#ifndef MOTO_OBJECT_PARAM_DATA_H
#define MOTO_OBJECT_PARAM_DATA_H

#include "moto-param-data.h"
#include "moto-object-node.h"

typedef struct _MotoObjectParamData MotoObjectParamData;
typedef struct _MotoObjectParamDataClass MotoObjectParamDataClass;

/* class MotoObjectParamData */

struct _MotoObjectParamData
{
    GObject parent;

    MotoObjectNode *value;
    MotoObjectNode *default_value;
};

struct _MotoObjectParamDataClass
{
    GObjectClass parent;
};

GType moto_object_param_data_get_type(void);

#define MOTO_TYPE_OBJECT_PARAM_DATA (moto_object_param_data_get_type())
#define MOTO_OBJECT_PARAM_DATA(obj)  (G_TYPE_CHECK_INSTANCE_CAST ((obj), MOTO_TYPE_OBJECT_PARAM_DATA, MotoObjectParamData))
#define MOTO_OBJECT_PARAM_DATA_CLASS(klass)  (G_TYPE_CHECK_CLASS_CAST ((klass), MOTO_TYPE_OBJECT_PARAM_DATA, MotoObjectParamDataClass))
#define MOTO_IS_OBJECT_PARAM_DATA(obj)  (G_TYPE_CHECK_INSTANCE_TYPE ((obj),MOTO_TYPE_OBJECT_PARAM_DATA))
#define MOTO_IS_OBJECT_PARAM_DATA_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),MOTO_TYPE_OBJECT_PARAM_DATA))
#define MOTO_OBJECT_PARAM_DATA_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),MOTO_TYPE_OBJECT_PARAM_DATA, MotoObjectParamDataClass))

MotoParamData *
moto_object_param_data_new(MotoObjectNode *default_value);

#endif /* MOTO_OBJECT_PARAM_DATA_H */

