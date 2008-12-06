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

#ifndef __MOTO_WORLD_H__
#define __MOTO_WORLD_H__

#include <glib-object.h>

#include "moto-forward.h"
#include "moto-axes-view-node.h"

#include "moto-enums.h"

G_BEGIN_DECLS

typedef gboolean (*MotoWorldForeachNodeFunc)(MotoWorld *world, MotoNode *node, gpointer user_data);

typedef struct _MotoWorldManipulator MotoWorldManipulator;
typedef struct _MotoWorldManipulatorClass MotoWorldManipulatorClass;
typedef struct _MotoWorldManipulatorPriv MotoWorldManipulatorPriv;

/* class MotoWorld */

/**
 * MotoWorld:
 *
 * MotoWorld is an opaque data structure to represent world of nodes in Moto system.
 * It should only be accessed via the following functions.
 */
struct _MotoWorld
{
    /*< private >*/
    GObject parent;

    MotoWorldPriv *priv;
};

struct _MotoWorldClass
{
    /*< private >*/
    GObjectClass parent;

    /* signals */
    guint button_press_signal_id;
    guint button_release_signal_id;
    guint motion_notify_signal_id;

    guint updated_signal_id;
};

GType moto_world_get_type(void);

#define MOTO_TYPE_WORLD (moto_world_get_type())
#define MOTO_WORLD(obj)  (G_TYPE_CHECK_INSTANCE_CAST ((obj), MOTO_TYPE_WORLD, MotoWorld))
#define MOTO_WORLD_CLASS(klass)  (G_TYPE_CHECK_CLASS_CAST ((klass), MOTO_TYPE_WORLD, MotoWorldClass))
#define MOTO_IS_WORLD(obj)  (G_TYPE_CHECK_INSTANCE_TYPE ((obj),MOTO_TYPE_WORLD))
#define MOTO_IS_WORLD_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),MOTO_TYPE_WORLD))
#define MOTO_WORLD_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),MOTO_TYPE_WORLD, MotoWorldClass))

/**
 * moto_world_new:
 * @name: The name of the world #const gchar*
 * @lib: #MotoLibrary instance which the world will use.
 *
 * Creates new empty world.
 *
 * Returns: MotoWorld instance without any nodes.
 */
MotoWorld *moto_world_new(const gchar *name, MotoLibrary *lib);

/**
 * moto_world_new_from_dump:
 * @filename: The filename of the world dump.
 * @lib: #MotoLibrary instance which the world will use.
 *
 * Creates world and initializes it from previously saved dump.
 *
 * Returns: MotoWorld instance.
 */
MotoWorld *moto_world_new_from_dump(const gchar *filename, MotoLibrary *lib);

/**
 * moto_world_get_name:
 * @self: a #MotoWorld.
 *
 * Returns: the name of the given world.
 */
const gchar *moto_world_get_name(MotoWorld *self);

/**
 * moto_world_create_node:
 * @self: a #MotoWorld.
 * @type_name: node class name as a string. Ex: "#MotoObjectNode", "#MotoMeshViewNode".
 * @node_name: name for new node. Name must be unique in the given container.
 * @path: path in which node will be placed. If NULL, node is placed in the root.
 *
 * Returns pointer to newly created node.
 */
MotoNode *moto_world_create_node(MotoWorld *self,
        GType type,
        const gchar *node_name,
        const gchar *path);

/**
 * moto_world_create_node_by_name:
 * @self: a #MotoWorld.
 * @type_name: node class name as a string. Ex: "#MotoObjectNode", "#MotoMeshViewNode".
 * @node_name: name for new node. Name must be unique in the given container.
 * @path: path in which node will be placed. If NULL, node is placed in the root.
 *
 * Returns pointer to newly created node.
 */
MotoNode *moto_world_create_node_by_name(MotoWorld *self,
        const gchar *type_name,
        const gchar *node_name,
        const gchar *path);

/**
 * moto_world_dump:
 * @self: a #MotoWorld.
 * @filename: name of the file to save.
 * @change_filename: if TRUE the world changes filename and in the future will be saved in this.
 *
 * Dumps all nodes into the file.
 */
void moto_world_dump(MotoWorld *self,
        const gchar *filename,
        gboolean change_filename);

/**
 * moto_world_dump_selected:
 * @self: a #MotoWorld.
 * @filename: name of the file to save.
 *
 * Dumps selected nodes and all its dependecies as a new world.
 */
void moto_world_dump_selected(MotoWorld *self, const gchar *filename);

/**
 * moto_world_merge:
 * @self: a #MotoWorld to merge into.
 * @other: a @MotoWorld which will be merged.
 *
 * Megres other world into first world.
 */
void moto_world_merge(MotoWorld *self, MotoWorld *other);

/**
 * moto_world_merge_from_file:
 * @self: a #MotoWorld to merge into.
 * @other: filename from which other world will be loaded and merged.
 *
 * Megres world loaded from given file into first world.
 */
void moto_world_merge_from_file(MotoWorld *self, const gchar *filename);

/**
 * moto_world_get_current_object:
 * @self: a #MotoWorld.
 *
 * Returns current object node.
 */
MotoObjectNode *moto_world_get_current_object(MotoWorld *self);

/**
 * moto_world_set_current_object:
 * @self: a #MotoWorld.
 * @obj: a #MotoObjectNode to set current.
 *
 * Sets the given object node as a current object for the world.
 */
void moto_world_set_object_current(MotoWorld *self, MotoObjectNode *obj);

/**
 * moto_world_get_root:
 * @self: a #MotoWorld.
 *
 * Returns current root object node.
 */
MotoObjectNode *moto_world_get_root(MotoWorld *self);

/**
 * moto_world_set_root:
 * @self: a #MotoWorld.
 * @root: a #MotoObjectNode to set as a root.
 *
 * Sets the given object node as a root object for the world.
 */
void moto_world_set_root(MotoWorld *self, MotoObjectNode *root);

/**
 * moto_world_get_camera:
 * @self: a #MotoWorld.
 *
 * Returns object node used as a camera.
 */
MotoObjectNode * moto_world_get_camera(MotoWorld *self);

/**
 * moto_world_set_camera:
 * @self: a #MotoWorld.
 * @camera: a #MotoObjectNode to set as a camera.
 *
 * Sets the given object node to use as camera.
 */
void moto_world_set_camera(MotoWorld *self, MotoObjectNode *camera);

/**
 * moto_world_get_axes:
 * @self: a #MotoWorld.
 *
 * Returns object node used for  drawing coordinate axes.
 */
MotoObjectNode * moto_world_get_axes(MotoWorld *self);

/**
 * moto_world_set_axes:
 * @self: a #MotoWorld.
 * @axes: a #MotoObjectNode to set as an axes object.
 *
 * Sets the given object node to use for  drawing coordinate axes.
 */
void moto_world_set_axes(MotoWorld *self, MotoObjectNode *axes);

/**
 * moto_world_draw:
 * @self: a #MotoWorld.
 * @width: width of viewport.
 * @height: height of viewport.
 *
 * Draws the world geometry.
 */
void moto_world_redraw(MotoWorld *self);

void moto_world_draw(MotoWorld *self, gint width, gint height);
void moto_world_draw_fps_test(MotoWorld *self);

/**
 * moto_world_apply_default_camera:
 * @self: a #MotoWorld.
 * @width: width of viewport.
 * @height: height of viewport.
 *
 * Applies default camera settings for OpenGL camera.
 * These settings are used when object that plays role of a camera have not
 * instance of class MotoCameraNode connected to its camera input.
 */
void moto_world_apply_default_camera(MotoWorld *self, gint width, gint height);

/**
 * moto_world_get_library:
 * @self: a #MotoWorld.
 *
 * Returns instance of MotoLibrary which the world use.
 */
MotoLibrary *moto_world_get_library(MotoWorld *self);

MotoNode *moto_world_get_node(MotoWorld *self, const gchar *name);

void moto_world_foreach_node(MotoWorld *self, GType type,
        MotoWorldForeachNodeFunc func, gpointer user_data);

/* Configuration */

void moto_world_set_use_vbo(MotoWorld *self, gboolean use);
gboolean moto_world_get_use_vbo(MotoWorld *self);
void moto_world_reset(MotoWorld *self);

/* Animation */

void moto_world_start_anim(MotoWorld *self);
void moto_world_stop_anim(MotoWorld *self);

void moto_world_update(MotoWorld *self);

/* Signals??? (TODO: These must be signals not just functions.) */

void moto_world_button_press(MotoWorld *self,
    gint x, gint y, gint width, gint height);
void moto_world_button_release(MotoWorld *self,
    gint x, gint y, gint width, gint height);
void moto_world_motion_notify(MotoWorld *self,
    gint x, gint y, gint width, gint height);

/* MotoWorldManipulator */

struct _MotoWorldManipulator
{
    /*< private >*/
    GObject parent;

    MotoWorldManipulatorPriv *priv;
};

struct _MotoWorldManipulatorClass
{
    GObjectClass parent;

    guint draw_signal_id;
    guint button_press_signal_id;
    guint button_release_signal_id;
    guint motion_notify_signal_id;
};

GType moto_world_manipualtor_get_type(void);

#define MOTO_TYPE_WORLD_MANIPULATOR (moto_world_manipulator_get_type())
#define MOTO_WORLD_MANIPULATOR(obj)  (G_TYPE_CHECK_INSTANCE_CAST ((obj), MOTO_TYPE_WORLD_MANIPULATOR, MotoWorldManipulator))
#define MOTO_WORLD_MANIPULATOR_CLASS(klass)  (G_TYPE_CHECK_CLASS_CAST ((klass), MOTO_TYPE_WORLD_MANIPULATOR, MotoWorldManipulatorClass))
#define MOTO_IS_WORLD_MANIPULATOR(obj)  (G_TYPE_CHECK_INSTANCE_TYPE ((obj),MOTO_TYPE_WORLD_MANIPULATOR))
#define MOTO_IS_WORLD_MANIPULATOR_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),MOTO_TYPE_WORLD_MANIPULATOR))
#define MOTO_WORLD_MANIPULATOR_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),MOTO_TYPE_WORLD_MANIPULATOR, MotoWorldManipulatorClass))

MotoWorldManipulator *moto_world_manipulator_new(void);
void moto_world_manipulator_draw(MotoWorldManipulator *self, MotoWorld *world);

gboolean moto_world_manipulator_button_press(MotoWorldManipulator *self, MotoWorld *world,
        gint x, gint y, gint width, gint height);
void moto_world_manipulator_button_release(MotoWorldManipulator *self, MotoWorld *world,
        gint x, gint y, gint width, gint height);
void moto_world_manipulator_motion_notify(MotoWorldManipulator *self, MotoWorld *world,
        gint x, gint y, gint width, gint height);

void moto_world_set_draw_mode(MotoWorld *self, MotoDrawMode draw_mode);
MotoDrawMode moto_world_get_draw_mode(MotoWorld *self);

G_END_DECLS

#endif /* __MOTO_WORLD_H__ */
