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

#ifndef __MOTO_SCENE_NODE_H__
#define __MOTO_SCENE_NODE_H__

#include <glib-object.h>

#include "moto-forward.h"
#include "moto-axes-view-node.h"

#include "moto-enums.h"

G_BEGIN_DECLS

typedef gboolean (*MotoSceneNodeForeachNodeFunc)(MotoSceneNode *scene_node, MotoNode *node, gpointer user_data);

typedef struct _MotoSceneNodeManipulator MotoSceneNodeManipulator;
typedef struct _MotoSceneNodeManipulatorClass MotoSceneNodeManipulatorClass;
typedef struct _MotoSceneNodeManipulatorPriv MotoSceneNodeManipulatorPriv;

/* class MotoSceneNode */

/**
 * MotoSceneNode:
 *
 * MotoSceneNode is an opaque data structure to represent scene_node of nodes in Moto system.
 * It should only be accessed via the following functions.
 */
struct _MotoSceneNode
{
    /*< private >*/
    MotoNode parent;

    MotoSceneNodePriv *priv;
};

struct _MotoSceneNodeClass
{
    /*< private >*/
    MotoNodeClass parent;

    /* signals */
    guint button_press_signal_id;
    guint button_release_signal_id;
    guint motion_notify_signal_id;

    guint updated_signal_id;
    guint changed_signal_id;
};

GType moto_scene_node_get_type(void);

#define MOTO_TYPE_SCENE_NODE (moto_scene_node_get_type())
#define MOTO_SCENE_NODE(obj)  (G_TYPE_CHECK_INSTANCE_CAST ((obj), MOTO_TYPE_SCENE_NODE, MotoSceneNode))
#define MOTO_SCENE_NODE_CLASS(klass)  (G_TYPE_CHECK_CLASS_CAST ((klass), MOTO_TYPE_SCENE_NODE, MotoSceneNodeClass))
#define MOTO_IS_SCENE_NODE(obj)  (G_TYPE_CHECK_INSTANCE_TYPE ((obj),MOTO_TYPE_SCENE_NODE))
#define MOTO_IS_SCENE_NODE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),MOTO_TYPE_SCENE_NODE))
#define MOTO_SCENE_NODE_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),MOTO_TYPE_SCENE_NODE, MotoSceneNodeClass))

/**
 * moto_scene_node_new:
 * @name: The name of the scene_node #const gchar*
 * @lib: #MotoLibrary instance which the scene_node will use.
 *
 * Creates new empty scene_node.
 *
 * Returns: MotoSceneNode instance without any nodes.
 */
MotoSceneNode *moto_scene_node_new(const gchar *name, MotoLibrary *lib);

/**
 * moto_scene_node_new_from_dump:
 * @filename: The filename of the scene_node dump.
 * @lib: #MotoLibrary instance which the scene_node will use.
 *
 * Creates scene_node and initializes it from previously saved dump.
 *
 * Returns: MotoSceneNode instance.
 */
MotoSceneNode *moto_scene_node_new_from_dump(const gchar *filename, MotoLibrary *lib);

/**
 * moto_scene_node_get_name:
 * @self: a #MotoSceneNode.
 *
 * Returns: the name of the given scene_node.
 */
const gchar *moto_scene_node_get_name(MotoSceneNode *self);

/**
 * moto_scene_node_create_node:
 * @self: a #MotoSceneNode.
 * @type_name: node class name as a string. Ex: "#MotoObjectNode", "#MotoMeshViewNode".
 * @node_name: name for new node. Name must be unique in the given container.
 * @path: path in which node will be placed. If NULL, node is placed in the root.
 *
 * Returns pointer to newly created node.
 */
MotoNode *moto_scene_node_create_node(MotoSceneNode *self,
        GType type,
        const gchar *node_name,
        const gchar *path);

/**
 * moto_scene_node_create_node_by_name:
 * @self: a #MotoSceneNode.
 * @type_name: node class name as a string. Ex: "#MotoObjectNode", "#MotoMeshViewNode".
 * @node_name: name for new node. Name must be unique in the given container.
 * @path: path in which node will be placed. If NULL, node is placed in the root.
 *
 * Returns pointer to newly created node.
 */
MotoNode *moto_scene_node_create_node_by_name(MotoSceneNode *self,
        const gchar *type_name,
        const gchar *node_name,
        const gchar *path);

/**
 * moto_scene_node_dump:
 * @self: a #MotoSceneNode.
 * @filename: name of the file to save.
 * @change_filename: if TRUE the scene_node changes filename and in the future will be saved in this.
 *
 * Dumps all nodes into the file.
 */
void moto_scene_node_dump(MotoSceneNode *self,
        const gchar *filename,
        gboolean change_filename);

/**
 * moto_scene_node_dump_selected:
 * @self: a #MotoSceneNode.
 * @filename: name of the file to save.
 *
 * Dumps selected nodes and all its dependecies as a new scene_node.
 */
void moto_scene_node_dump_selected(MotoSceneNode *self, const gchar *filename);

/**
 * moto_scene_node_merge:
 * @self: a #MotoSceneNode to merge into.
 * @other: a @MotoSceneNode which will be merged.
 *
 * Megres other scene_node into first scene_node.
 */
void moto_scene_node_merge(MotoSceneNode *self, MotoSceneNode *other);

/**
 * moto_scene_node_merge_from_file:
 * @self: a #MotoSceneNode to merge into.
 * @other: filename from which other scene_node will be loaded and merged.
 *
 * Megres scene_node loaded from given file into first scene_node.
 */
void moto_scene_node_merge_from_file(MotoSceneNode *self, const gchar *filename);

/**
 * moto_scene_node_get_current_object:
 * @self: a #MotoSceneNode.
 *
 * Returns current object node.
 */
MotoObjectNode *moto_scene_node_get_current_object(MotoSceneNode *self);

/**
 * moto_scene_node_set_current_object:
 * @self: a #MotoSceneNode.
 * @obj: a #MotoObjectNode to set current.
 *
 * Sets the given object node as a current object for the scene_node.
 */
void moto_scene_node_set_object_current(MotoSceneNode *self, MotoObjectNode *obj);

/**
 * moto_scene_node_get_root:
 * @self: a #MotoSceneNode.
 *
 * Returns current root object node.
 */
MotoObjectNode *moto_scene_node_get_root(MotoSceneNode *self);

/**
 * moto_scene_node_set_root:
 * @self: a #MotoSceneNode.
 * @root: a #MotoObjectNode to set as a root.
 *
 * Sets the given object node as a root object for the scene_node.
 */
void moto_scene_node_set_root(MotoSceneNode *self, MotoObjectNode *root);

/**
 * moto_scene_node_get_camera:
 * @self: a #MotoSceneNode.
 *
 * Returns object node used as a camera.
 */
MotoObjectNode * moto_scene_node_get_camera(MotoSceneNode *self);

/**
 * moto_scene_node_set_camera:
 * @self: a #MotoSceneNode.
 * @camera: a #MotoObjectNode to set as a camera.
 *
 * Sets the given object node to use as camera.
 */
void moto_scene_node_set_camera(MotoSceneNode *self, MotoObjectNode *camera);

/**
 * moto_scene_node_get_axes:
 * @self: a #MotoSceneNode.
 *
 * Returns object node used for  drawing coordinate axes.
 */
MotoObjectNode * moto_scene_node_get_axes(MotoSceneNode *self);

/**
 * moto_scene_node_set_axes:
 * @self: a #MotoSceneNode.
 * @axes: a #MotoObjectNode to set as an axes object.
 *
 * Sets the given object node to use for  drawing coordinate axes.
 */
void moto_scene_node_set_axes(MotoSceneNode *self, MotoObjectNode *axes);

/**
 * moto_scene_node_draw:
 * @self: a #MotoSceneNode.
 * @width: width of viewport.
 * @height: height of viewport.
 *
 * Draws the scene_node shape.
 */
void moto_scene_node_redraw(MotoSceneNode *self);

void moto_scene_node_draw(MotoSceneNode *self, gint width, gint height);
void moto_scene_node_draw_fps_test(MotoSceneNode *self);

/**
 * moto_scene_node_apply_default_camera:
 * @self: a #MotoSceneNode.
 * @width: width of viewport.
 * @height: height of viewport.
 *
 * Applies default camera settings for OpenGL camera.
 * These settings are used when object that plays role of a camera have not
 * instance of class MotoCameraNode connected to its camera input.
 */
void moto_scene_node_apply_default_camera(MotoSceneNode *self, gint width, gint height);

/**
 * moto_scene_node_get_library:
 * @self: a #MotoSceneNode.
 *
 * Returns instance of MotoLibrary which the scene_node use.
 */
MotoLibrary *moto_scene_node_get_library(MotoSceneNode *self);

MotoNode *moto_scene_node_get_node(MotoSceneNode *self, const gchar *name);

void moto_scene_node_foreach_node(MotoSceneNode *self, GType type,
    MotoSceneNodeForeachNodeFunc func, gpointer user_data);

/* Configuration */

void moto_scene_node_set_use_vbo(MotoSceneNode *self, gboolean use);
gboolean moto_scene_node_get_use_vbo(MotoSceneNode *self);

void moto_scene_node_set_use_arrays(MotoSceneNode *self, gboolean use);
gboolean moto_scene_node_get_use_arrays(MotoSceneNode *self);

void moto_scene_node_set_show_normals(MotoSceneNode *self, gboolean show);
gboolean moto_scene_node_get_show_normals(MotoSceneNode *self);

void moto_scene_node_set_cull_faces(MotoSceneNode *self, gboolean cull);
gboolean moto_scene_node_get_cull_faces(MotoSceneNode *self);

void moto_scene_node_reset(MotoSceneNode *self);

/* Animation */

void moto_scene_node_start_anim(MotoSceneNode *self);
void moto_scene_node_stop_anim(MotoSceneNode *self);

void moto_scene_node_update(MotoSceneNode *self);

/* Signals??? (TODO: These must be signals not just functions.) */

void moto_scene_node_button_press(MotoSceneNode *self,
    gint x, gint y, gint width, gint height);
void moto_scene_node_button_release(MotoSceneNode *self,
    gint x, gint y, gint width, gint height);
void moto_scene_node_motion_notify(MotoSceneNode *self,
    gint x, gint y, gint width, gint height);

/* MotoSceneNodeManipulator */

struct _MotoSceneNodeManipulator
{
    /*< private >*/
    GObject parent;

    MotoSceneNodeManipulatorPriv *priv;
};

struct _MotoSceneNodeManipulatorClass
{
    GObjectClass parent;

    guint draw_signal_id;
    guint button_press_signal_id;
    guint button_release_signal_id;
    guint motion_notify_signal_id;
};

GType moto_scene_node_manipualtor_get_type(void);

#define MOTO_TYPE_SCENE_NODE_MANIPULATOR (moto_scene_node_manipulator_get_type())
#define MOTO_SCENE_NODE_MANIPULATOR(obj)  (G_TYPE_CHECK_INSTANCE_CAST ((obj), MOTO_TYPE_SCENE_NODE_MANIPULATOR, MotoSceneNodeManipulator))
#define MOTO_SCENE_NODE_MANIPULATOR_CLASS(klass)  (G_TYPE_CHECK_CLASS_CAST ((klass), MOTO_TYPE_SCENE_NODE_MANIPULATOR, MotoSceneNodeManipulatorClass))
#define MOTO_IS_SCENE_NODE_MANIPULATOR(obj)  (G_TYPE_CHECK_INSTANCE_TYPE ((obj),MOTO_TYPE_SCENE_NODE_MANIPULATOR))
#define MOTO_IS_SCENE_NODE_MANIPULATOR_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),MOTO_TYPE_SCENE_NODE_MANIPULATOR))
#define MOTO_SCENE_NODE_MANIPULATOR_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),MOTO_TYPE_SCENE_NODE_MANIPULATOR, MotoSceneNodeManipulatorClass))

MotoSceneNodeManipulator *moto_scene_node_manipulator_new(void);
void moto_scene_node_manipulator_draw(MotoSceneNodeManipulator *self, MotoSceneNode *scene_node);

gboolean moto_scene_node_manipulator_button_press(MotoSceneNodeManipulator *self, MotoSceneNode *scene_node,
        gint x, gint y, gint width, gint height);
void moto_scene_node_manipulator_button_release(MotoSceneNodeManipulator *self, MotoSceneNode *scene_node,
        gint x, gint y, gint width, gint height);
void moto_scene_node_manipulator_motion_notify(MotoSceneNodeManipulator *self, MotoSceneNode *scene_node,
        gint x, gint y, gint width, gint height);

void moto_scene_node_set_draw_mode(MotoSceneNode *self, MotoDrawMode draw_mode);
MotoDrawMode moto_scene_node_get_draw_mode(MotoSceneNode *self);

G_END_DECLS

#endif /* __MOTO_SCENE_NODE_H__ */
