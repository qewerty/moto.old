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

#include <glib-object.h>

#include "moto-forward.h"

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
MotoWorld *moto_world_new_from_dump(const gchar *filename, MotoLibrary *lib);
/* MotoWorld *moto_world_new_from_binary_dump(const gchar *filename);
MotoWorld *moto_world_new_from_xml_dump(const gchar *filename); */

const gchar *moto_world_get_name(MotoWorld *self);

void moto_world_add_node(MotoWorld *self, MotoNode *node);
MotoNode *moto_world_create_node(MotoWorld *self,
        const gchar *type_name, const gchar *node_name);

/* Dump all nodes. */
void moto_world_binary_dump(MotoWorld *self, const gchar *filename, gboolean change_filename);
void moto_world_xml_dump(MotoWorld *self, const gchar *filename, gboolean change_filename);

/* Dump selected nodes and all its dependecies as a new world. */
void moto_world_binary_dump_selected(MotoWorld *self, const gchar *filename, gboolean change_filename);
void moto_world_xml_dump_selected(MotoWorld *self, const gchar *filename, gboolean change_filename);

void moto_world_merge(MotoWorld *self, const gchar *filename);

MotoObjectNode *moto_world_get_root(MotoWorld *self);
void moto_world_set_root(MotoWorld *self, MotoObjectNode *root);
void moto_world_draw(MotoWorld *self);

MotoLibrary *moto_world_get_library(MotoWorld *self);

#endif /* MOTO_WORLD_H */
