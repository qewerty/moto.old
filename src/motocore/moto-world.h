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

#ifndef MOTO_WORLD_H
#define MOTO_WORLD_H

#include "moto-node.h"
#include "moto-library.h"

typedef struct _MotoWorld MotoWorld;
typedef struct _MotoWorldClass MotoWorldClass;
typedef struct _MotoWorldPriv MotoWorldPriv;

/* class MotoWorld */

struct _MotoWorld
{
    GObject parent;

    MotoWorldPriv *priv;
};

struct _MotoWorldClass
{
    GObjectClass parent;
};

GType moto_world_get_type(void);

#define MOTO_TYPE_WORLD (moto_world_get_type())
#define MOTO_WORLD(obj)  (G_TYPE_CHECK_INSTANCE_CAST ((obj), MOTO_TYPE_WORLD, MotoWorld))
#define MOTO_WORLD_CLASS(klass)  (G_TYPE_CHECK_CLASS_CAST ((klass), MOTO_TYPE_WORLD, MotoWorldClass))
#define MOTO_IS_WORLD(obj)  (G_TYPE_CHECK_INSTANCE_TYPE ((obj),MOTO_TYPE_WORLD))
#define MOTO_IS_WORLD_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),MOTO_TYPE_WORLD))
#define MOTO_WORLD_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),MOTO_TYPE_WORLD, MotoWorldClass))

MotoWorld *moto_world_new(const gchar *name, MotoLibrary *lib);

MotoWorld *moto_world_new_from_dump(const gchar *filename);
/* MotoWorld *moto_world_new_from_binary_dump(const gchar *filename);
MotoWorld *moto_world_new_from_xml_dump(const gchar *filename); */

/* Dump all nodes. */
void moto_world_binary_dump(MotoWorld *self, const *gchar filename, gboolean change_filename);
void moto_world_xml_dump(MotoWorld *self, const *gchar filename, gboolean change_filename);

/* Dump selected nodes and all its dependecies as a new world. */
void moto_world_binary_dump_selected(MotoWorld *self, const *gchar filename, gboolean change_filename);
void moto_world_xml_dump_selected(MotoWorld *self, const *gchar filename, gboolean change_filename);

void moto_world_merge(MotoWorld *self, const gchar *filename);

#endif /* MOTO_WORLD_H */
