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

#ifndef __MOTO_NODE_H__
#define __MOTO_NODE_H__

#include <glib-object.h>

#include "moto-forward.h"
#include "moto-param-spec.h"

G_BEGIN_DECLS

typedef void (*MotoNodeActionFunc)(MotoNode *node);
typedef void (*MotoNodeForeachParamFunc)(MotoNode *node, MotoParam *param, gpointer user_data);
typedef void (*MotoNodeForeachGroupFunc)(MotoNode *node, const gchar *group, gpointer user_data);
typedef void (*MotoNodeForeachParamInGroupFunc)(MotoNode *node, const gchar *group, MotoParam *param, gpointer user_data);

typedef enum
{
    MOTO_PARAM_MODE_IN      = 1,
    MOTO_PARAM_MODE_OUT     = 2,
    MOTO_PARAM_MODE_INOUT   = 3
} MotoParamMode; // TODO: Rename to MOTO_PARAM_DIRECTION_*

GType moto_param_mode_get_type(void);
#define MOTO_TYPE_PARAM_MODE (moto_param_mode_get_type())

/* class MotoNode */

struct _MotoNode
{
    GInitiallyUnowned parent;
};

struct _MotoNodeClass
{
    GInitiallyUnownedClass parent;

    GData *actions;

    /* Virtual Table */
    MotoNodeUpdateMethod update;

    /* Signals */
    guint source_changed_signal_id;
};

GType moto_node_get_type(void);

#define MOTO_TYPE_NODE (moto_node_get_type())
#define MOTO_NODE(obj)  (G_TYPE_CHECK_INSTANCE_CAST ((obj), MOTO_TYPE_NODE, MotoNode))
#define MOTO_NODE_CLASS(klass)  (G_TYPE_CHECK_CLASS_CAST ((klass), MOTO_TYPE_NODE, MotoNodeClass))
#define MOTO_IS_NODE(obj)  (G_TYPE_CHECK_INSTANCE_TYPE ((obj),MOTO_TYPE_NODE))
#define MOTO_IS_NODE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),MOTO_TYPE_NODE))
#define MOTO_NODE_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),MOTO_TYPE_NODE, MotoNodeClass))

MotoNode *moto_create_node(GType type, const gchar *name);
MotoNode *moto_create_node_by_name(const gchar *type_name, const gchar *name);

MotoNode *moto_node_create_child(MotoNode *self, GType type, const gchar *name);
MotoNode *moto_node_create_child_by_name(MotoNode *self,
    const gchar *type_name, const gchar *name);
MotoNode *moto_node_get_child(MotoNode *self, const gchar *name);
guint moto_node_get_n_children(MotoNode *self);
void moto_node_foreach_children(MotoNode *self, GFunc func, gpointer user_data);

MotoNode *moto_node_get_parent(MotoNode *self);
void moto_node_set_parent(MotoNode *self, MotoNode *parent);

void moto_node_do_action(MotoNode *self, const gchar *action_name);
/* TODO: Make as moto_node_class_set_action */
void moto_node_set_action(MotoNode *self, const gchar *action_name, MotoNodeActionFunc func);

const gchar *moto_node_get_name(MotoNode *self);
void moto_node_set_name(MotoNode *self, const gchar *name);
const gchar *moto_node_get_full_name(MotoNode *self);
const gchar *moto_node_get_type_name(MotoNode *self);

const gchar *moto_node_get_type_alias(MotoNode *self);

/* Reimplement. */
guint moto_node_get_id(MotoNode *self);

gboolean moto_node_is_valid(MotoNode *self);
gboolean moto_node_is_independent(MotoNode *self);
gboolean moto_node_is_ready_to_update(MotoNode *self);
gboolean moto_node_needs_update(MotoNode *self);

void moto_node_add_dynamic_param(MotoNode *self, MotoParam *param, const gchar *group);
#define moto_node_add_param(self, param, group) \
    moto_node_add_dynamic_param(self, param, group)
void moto_node_add_params(MotoNode *self, ...) G_GNUC_NULL_TERMINATED;

void moto_node_add_static_param(MotoNode *self, MotoParam *param, const gchar *group);
void moto_node_add_static_params(MotoNode *self, ...) G_GNUC_NULL_TERMINATED;

MotoParam *moto_node_get_param(MotoNode *self, const gchar *name);
GValue *moto_node_get_param_value(MotoNode *self, const gchar *name);

gboolean moto_node_get_param_boolean(MotoNode *self, const gchar *name, gboolean *v);
gboolean moto_node_get_param_int(MotoNode *self,     const gchar *name, gint *v);
gboolean moto_node_get_param_float(MotoNode *self,   const gchar *name, gfloat *v);
gboolean moto_node_get_param_string(MotoNode *self,  const gchar *name, const gchar **v);
gboolean moto_node_get_param_pointer(MotoNode *self, const gchar *name, gpointer *v);
gboolean moto_node_get_param_enum(MotoNode *self,    const gchar *name, gint *v);
gboolean moto_node_get_param_object(MotoNode *self,  const gchar *name, GObject **v);

gboolean moto_node_set_param_boolean(MotoNode *self, const gchar *name, gboolean value);
gboolean moto_node_set_param_int(MotoNode *self,     const gchar *name, gint     value);
gboolean moto_node_set_param_float(MotoNode *self,   const gchar *name, gfloat   value);
gboolean moto_node_set_param_pointer(MotoNode *self, const gchar *name, gpointer value);
gboolean moto_node_set_param_enum(MotoNode *self,    const gchar *name, gint     value);
gboolean moto_node_set_param_object(MotoNode *self,  const gchar *name, GObject *value);

// boolean
gboolean moto_node_set_param_2b(MotoNode *self,  const gchar *name, gboolean v0, gboolean v1);
gboolean moto_node_set_param_2bv(MotoNode *self, const gchar *name, const gboolean *v);

gboolean moto_node_set_param_3b(MotoNode *self,  const gchar *name, gboolean v0, gboolean v1, gboolean v2);
gboolean moto_node_set_param_3bv(MotoNode *self, const gchar *name, const gboolean *v);

gboolean moto_node_set_param_4b(MotoNode *self,  const gchar *name, gboolean v0, gboolean v1, gboolean v2, gboolean v3);
gboolean moto_node_set_param_4bv(MotoNode *self, const gchar *name, const gboolean *v);

// int
gboolean moto_node_set_param_2i(MotoNode *self,  const gchar *name, gint v0, gint v1);
gboolean moto_node_set_param_2iv(MotoNode *self, const gchar *name, const gint *v);

gboolean moto_node_set_param_3i(MotoNode *self,  const gchar *name, gint v0, gint v1, gint v2);
gboolean moto_node_set_param_3iv(MotoNode *self, const gchar *name, const gint *v);

gboolean moto_node_set_param_4i(MotoNode *self,  const gchar *name, gint v0, gint v1, gint v2, gint v3);
gboolean moto_node_set_param_4iv(MotoNode *self, const gchar *name, const gint *v);

// float
gboolean moto_node_set_param_2f(MotoNode *self,  const gchar *name, gfloat v0, gfloat v1);
gboolean moto_node_set_param_2fv(MotoNode *self, const gchar *name, const gfloat *v);

gboolean moto_node_set_param_3f(MotoNode *self,  const gchar *name, gfloat v0, gfloat v1, gfloat v2);
gboolean moto_node_set_param_3fv(MotoNode *self, const gchar *name, const gfloat *v);

gboolean moto_node_set_param_4f(MotoNode *self,  const gchar *name, gfloat v0, gfloat v1, gfloat v2, gfloat v3);
gboolean moto_node_set_param_4fv(MotoNode *self, const gchar *name, const gfloat *v);

// boolean
gboolean moto_node_get_param_2b(MotoNode *self,  const gchar *name, gboolean *v0, gboolean *v1);
gboolean moto_node_get_param_2bv(MotoNode *self, const gchar *name, gboolean *v);

gboolean moto_node_get_param_3b(MotoNode *self,  const gchar *name, gboolean *v0, gboolean *v1, gboolean *v2);
gboolean moto_node_get_param_3bv(MotoNode *self, const gchar *name, gboolean *v);

gboolean moto_node_get_param_4b(MotoNode *self,  const gchar *name, gboolean *v0, gboolean *v1, gboolean *v2, gboolean *v3);
gboolean moto_node_get_param_4bv(MotoNode *self, const gchar *name, gboolean *v);

// int
gboolean moto_node_get_param_2i(MotoNode *self,  const gchar *name, gint *v0, gint *v1);
gboolean moto_node_get_param_2iv(MotoNode *self, const gchar *name, gint *v);

gboolean moto_node_get_param_3i(MotoNode *self,  const gchar *name, gint *v0, gint *v1, gint *v2);
gboolean moto_node_get_param_3iv(MotoNode *self, const gchar *name, gint *v);

gboolean moto_node_get_param_4i(MotoNode *self,  const gchar *name, gint *v0, gint *v1, gint *v2, gint *v3);
gboolean moto_node_get_param_4iv(MotoNode *self, const gchar *name, gint *v);

// float
gboolean moto_node_get_param_2f(MotoNode *self,  const gchar *name, gfloat *v0, gfloat *v1);
gboolean moto_node_get_param_2fv(MotoNode *self, const gchar *name, gfloat *v);

gboolean moto_node_get_param_3f(MotoNode *self,  const gchar *name, gfloat *v0, gfloat *v1, gfloat *v2);
gboolean moto_node_get_param_3fv(MotoNode *self, const gchar *name, gfloat *v);

gboolean moto_node_get_param_4f(MotoNode *self,  const gchar *name, gfloat *v0, gfloat *v1, gfloat *v2, gfloat *v3);
gboolean moto_node_get_param_4fv(MotoNode *self, const gchar *name, gfloat *v);

gboolean moto_node_get_params(MotoNode *self, ...) G_GNUC_NULL_TERMINATED;
gboolean moto_node_set_params(MotoNode *self, ...) G_GNUC_NULL_TERMINATED;

void moto_node_foreach_param(MotoNode *self,
        MotoNodeForeachParamFunc func, gpointer user_data);

void moto_node_foreach_group(MotoNode *self,
        MotoNodeForeachGroupFunc func, gpointer user_data);
void moto_node_foreach_param_in_group(MotoNode *self, const gchar *group_name,
        MotoNodeForeachParamInGroupFunc func, gpointer user_data);

void moto_node_link(MotoNode *self, const gchar *in_name,
                    MotoNode *other, const gchar *out_name);

void moto_node_update_param_dests(MotoNode *self, const gchar *name);
void moto_node_update_all_param_dests(MotoNode *self);

void moto_node_save_to_variation(MotoNode *self, MotoVariation *variation);
void moto_node_restore_from_variation(MotoNode *self, MotoVariation *variation);

const GTimeVal *moto_node_get_last_modified(MotoNode *self);
void moto_node_update_last_modified(MotoNode *self);

/* Get manipulator wth given name or NULL if not found. */
MotoManipulator *moto_node_get_manipulator(MotoNode *self, const gchar *name);
/* Get first manipulator in the list or NULL if node has no manipulators. */
MotoManipulator *moto_node_get_first_manipulator(MotoNode *self);
/* Get next manipulator in the list.
 * If manip argument is the last manipulator or it's not in the list or manip is NULL get first. */
MotoManipulator *moto_node_get_next_manipulator(MotoNode *self, MotoManipulator *manip);

void moto_node_set_tag(MotoNode *self, const gchar *tag);
void moto_node_del_tag(MotoNode *self, const gchar *tag); 
gboolean moto_node_has_tag(MotoNode *self, const gchar *tag);

/* Get dump for saving. */
gconstpointer moto_node_get_dump(MotoNode *self, glong *numbytes);

/* Update node internals. This must not affect on other nodes. */
void moto_node_update(MotoNode *self);

/* Just moto_node_update if there is no cache or restore state of node from undo/redo cache. */
void moto_node_undo(MotoNode *self);
void moto_node_redo(MotoNode *self);

MotoWorld *moto_node_get_world(MotoNode *self);
void moto_node_set_world(MotoNode *self, MotoWorld *world);

MotoLibrary *moto_node_get_library(MotoNode *self);

gboolean moto_node_depends_on(MotoNode *self, MotoNode *other);

/* Returns TRUE if node changes during animation or FALSE otherwise.
 * Used for optimization. */
gboolean moto_node_is_animated(MotoNode *self);

/* class MotoParam */

struct _MotoParam
{
    GObject parent;
};

struct _MotoParamClass
{
    GObjectClass parent;

    guint value_changed_signal_id;
    guint source_changed_signal_id;
    guint dest_added_signal_id;
    guint dest_removed_signal_id;
};

GType moto_param_get_type(void);

#define MOTO_TYPE_PARAM (moto_param_get_type())
#define MOTO_PARAM(obj)  (G_TYPE_CHECK_INSTANCE_CAST ((obj), MOTO_TYPE_PARAM, MotoParam))
#define MOTO_PARAM_CLASS(klass)  (G_TYPE_CHECK_CLASS_CAST ((klass), MOTO_TYPE_PARAM, MotoParamClass))
#define MOTO_IS_PARAM(obj)  (G_TYPE_CHECK_INSTANCE_TYPE ((obj),MOTO_TYPE_PARAM))
#define MOTO_IS_PARAM_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),MOTO_TYPE_PARAM))
#define MOTO_PARAM_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),MOTO_TYPE_PARAM, MotoParamClass))

MotoParam *moto_param_new(const gchar *name, const gchar *title,
        MotoParamMode mode, GValue *value, MotoParamSpec *pspec,
        MotoNode *node);

MotoParam *moto_param_new_full(const gchar *name, const gchar *title,
        MotoParamMode mode, GValue *value, MotoParamSpec *pspec,
        MotoNode *node, gboolean scriptable, gboolean is_static);

const gchar *moto_param_get_name(MotoParam *self);
const gchar *moto_param_get_title(MotoParam *self);
MotoNode *moto_param_get_node(MotoParam *self);
MotoParamMode moto_param_get_mode(MotoParam *self);

MotoParamSpec *moto_param_get_spec(MotoParam *self);

guint moto_param_get_id(MotoParam *self);

GType moto_param_get_value_type(MotoParam *self);

gboolean moto_param_get_boolean(MotoParam *self);
gint     moto_param_get_int(MotoParam *self);
gfloat   moto_param_get_float(MotoParam *self);
const gchar *moto_param_get_string(MotoParam *self);
gpointer moto_param_get_pointer(MotoParam *self);
gint     moto_param_get_enum(MotoParam *self);
GObject *moto_param_get_object(MotoParam *self);

// boolean
void moto_param_get_2b(MotoParam *self,  gboolean *v0, gboolean *v1);
void moto_param_get_2bv(MotoParam *self, gboolean *v);

void moto_param_get_3b(MotoParam *self,  gboolean *v0, gboolean *v1, gboolean *v2);
void moto_param_get_3bv(MotoParam *self, gboolean *v);

void moto_param_get_4b(MotoParam *self,  gboolean *v0, gboolean *v1, gboolean *v2, gboolean *v3);
void moto_param_get_4bv(MotoParam *self, gboolean *v);

// int
void moto_param_get_2i(MotoParam *self,  gint *v0, gint *v1);
void moto_param_get_2iv(MotoParam *self, gint *v);

void moto_param_get_3i(MotoParam *self,  gint *v0, gint *v1, gint *v2);
void moto_param_get_3iv(MotoParam *self, gint *v);

void moto_param_get_4i(MotoParam *self,  gint *v0, gint *v1, gint *v2, gint *v3);
void moto_param_get_4iv(MotoParam *self, gint *v);

// float
void moto_param_get_2f(MotoParam *self,  gfloat *v0, gfloat *v1);
void moto_param_get_2fv(MotoParam *self, gfloat *v);

void moto_param_get_3f(MotoParam *self,  gfloat *v0, gfloat *v1, gfloat *v2);
void moto_param_get_3fv(MotoParam *self, gfloat *v);

void moto_param_get_4f(MotoParam *self,  gfloat *v0, gfloat *v1, gfloat *v2, gfloat *v3);
void moto_param_get_4fv(MotoParam *self, gfloat *v);

// Set parameter value
void moto_param_set_boolean(MotoParam *self, gboolean     value);
void moto_param_set_int(MotoParam *self,     gint         value);
void moto_param_set_float(MotoParam *self,   gfloat       value);
void moto_param_set_string(MotoParam *self,  const gchar *value);
void moto_param_set_pointer(MotoParam *self, gpointer     value);
void moto_param_set_enum(MotoParam *self,    gint         value);
void moto_param_set_object(MotoParam *self,  GObject     *value);

// boolean
void moto_param_set_2b(MotoParam *self, gboolean v0, gboolean v1);
void moto_param_set_2bv(MotoParam *self, const gboolean *v);

void moto_param_set_3b(MotoParam *self, gboolean v0, gboolean v1, gboolean v2);
void moto_param_set_3bv(MotoParam *self, const gboolean *v);

void moto_param_set_4b(MotoParam *self, gboolean v0, gboolean v1, gboolean v2, gboolean v3);
void moto_param_set_4bv(MotoParam *self, const gboolean *v);

// int
void moto_param_set_2i(MotoParam *self, gint v0, gint v1);
void moto_param_set_2iv(MotoParam *self, const gint *v);

void moto_param_set_3i(MotoParam *self, gint v0, gint v1, gint v2);
void moto_param_set_3iv(MotoParam *self, const gint *v);

void moto_param_set_4i(MotoParam *self, gint v0, gint v1, gint v2, gint v3);
void moto_param_set_4iv(MotoParam *self, const gint *v);

// float
void moto_param_set_2f(MotoParam *self, gfloat v0, gfloat v1);
void moto_param_set_2fv(MotoParam *self, const gfloat *v);

void moto_param_set_3f(MotoParam *self, gfloat v0, gfloat v1, gfloat v2);
void moto_param_set_3fv(MotoParam *self, const gfloat *v);

void moto_param_set_4f(MotoParam *self, gfloat v0, gfloat v1, gfloat v2, gfloat v3);
void moto_param_set_4fv(MotoParam *self, const gfloat *v);

gboolean moto_param_is_static(MotoParam *self);

void moto_param_set_scriptable(MotoParam *self, gboolean scriptable);
gboolean moto_param_get_scriptable(MotoParam *self);

void moto_param_set_use_expression(MotoParam *self, gboolean use);
gboolean moto_param_get_use_expression(MotoParam *self);

gboolean moto_param_eval(MotoParam *self);

void moto_param_set_expression(MotoParam *self, const gchar *body);
const gchar *moto_param_get_expression(MotoParam *self);

GValue *moto_param_get_value(MotoParam *self);

/* Valid only if mode is IN or INOUT and does nothing else. */
MotoParam *moto_param_get_source(MotoParam *self);
void moto_param_link(MotoParam *self, MotoParam *src);
void moto_param_unlink_source(MotoParam *self);

/* Valid only if mode is OUT or INOUT and does nothing else. */
void moto_param_unlink_dests(MotoParam *self);

/* Just calls moto_param_unlink_source and moto_param_unlink_dests. */
void moto_param_unlink(MotoParam *self);

gboolean moto_param_has_dests(MotoParam *self);

/* May be FALSE if source is invalid or when limits are exceeded.  */
gboolean moto_param_is_valid(MotoParam *self);

gboolean moto_param_is_animated(MotoParam *self);

G_END_DECLS

#endif /* __MOTO_NODE_H__ */
