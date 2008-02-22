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

#ifndef MOTO_STRING_PARAM_DATA_H
#define MOTO_STRING_PARAM_DATA_H

#include "moto-param-data.h"

typedef struct _MotoStringParamData MotoStringParamData;
typedef struct _MotoStringParamDataClass MotoStringParamDataClass;

/* class MotoStringParamData */

struct _MotoStringParamData
{
    MotoParamData parent;

    GString *value;
    GString *default_value;
};

struct _MotoStringParamDataClass
{
    MotoParamDataClass parent;
};

GType moto_string_param_data_get_type(void);

#define MOTO_TYPE_STRING_PARAM_DATA (moto_string_param_data_get_type())
#define MOTO_STRING_PARAM_DATA(obj)  (G_TYPE_CHECK_INSTANCE_CAST ((obj), MOTO_TYPE_STRING_PARAM_DATA, MotoStringParamData))
#define MOTO_STRING_PARAM_DATA_CLASS(klass)  (G_TYPE_CHECK_CLASS_CAST ((klass), MOTO_TYPE_STRING_PARAM_DATA, MotoStringParamDataClass))
#define MOTO_IS_STRING_PARAM_DATA(obj)  (G_TYPE_CHECK_INSTANCE_TYPE ((obj),MOTO_TYPE_STRING_PARAM_DATA))
#define MOTO_IS_STRING_PARAM_DATA_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),MOTO_TYPE_STRING_PARAM_DATA))
#define MOTO_STRING_PARAM_DATA_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),MOTO_TYPE_STRING_PARAM_DATA, MotoStringParamDataClass))

MotoParamData *
moto_string_param_data_new(const gchar *default_value);

#endif /* MOTO_STRING_PARAM_DATA_H */

