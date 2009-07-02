/* ##################################################################################
#
#  Moto - small node based image compositor with 32bit floating point processing
#  of data (http://motocompose.sf.net)
#  Copyleft (C) 2006 Konstantin Evdokimenko a.k.a Qew (qewerty@gmail.com)
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

#ifndef __MOTO_COMMANDS_H__
#define __MOTO_COMMANDS_H__

#include <glib-object.h>

#include "libmotoutil/moto-command-stack.h"
#include "moto-node.h"

G_BEGIN_DECLS

typedef struct _MotoCommandCreateNode  MotoCommandCreateNode;
typedef struct _MotoCommandCreateNodeClass  MotoCommandCreateNodeClass;

typedef struct _MotoCommandDeleteNode  MotoCommandDeleteNode;
typedef struct _MotoCommandDeleteNodeClass  MotoCommandDeleteNodeClass;

typedef struct _MotoCommandChangeParamValue MotoCommandChangeParamValue;
typedef struct _MotoCommandChangeParamValueClass MotoCommandChangeParamValueClass;

typedef struct _MotoCommandChangeParamExpr  MotoCommandChangeParamExpr;
typedef struct _MotoCommandChangeParamExprClass  MotoCommandChangeParamExprClass;

typedef struct _MotoCommandLinkParams MotoCommandLinkParams;
typedef struct _MotoCommandLinkParamsClass MotoCommandLinkParamsClass;

typedef struct _MotoCommandUnlinkParams MotoCommandUnlinkParams;
typedef struct _MotoCommandUnlinkParamsClass MotoCommandUnlinkParamsClass;

// MotoCommandCreateNode

struct _MotoCommandCreateNode
{
    MotoCommand parent;
};

struct _MotoCommandCreateNodeClass
{
    MotoCommandClass parent;
};

GType moto_command_create_get_type(void);

#define MOTO_TYPE_COMMAND_CREATE (moto_command_create_get_type())
#define MOTO_COMMAND_CREATE(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), MOTO_TYPE_COMMAND_CREATE, MotoCommandCreateNode))
#define MOTO_COMMAND_CREATE_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST ((klass), MOTO_TYPE_COMMAND_CREATE, MotoCommandCreateNodeClass))
#define MOTO_IS_COMMAND_CREATE(obj)      (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MOTO_TYPE_COMMAND_CREATE))
#define MOTO_IS_COMMAND_CREATE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), MOTO_TYPE_COMMAND_CREATE))
#define MOTO_COMMAND_CREATE_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), MOTO_TYPE_COMMAND_CREATE, MotoCommandCreateNodeClass))

MotoCommand *moto_command_create_new(GType g_type, const gchar *name, MotoNode *parent);

// MotoCommandDeleteNode

struct _MotoCommandDeleteNode
{
    MotoCommand parent;
};

struct _MotoCommandDeleteNodeClass
{
    MotoCommandClass parent;
};

GType moto_command_delete_get_type(void);

#define MOTO_TYPE_COMMAND_DELETE (moto_command_delete_get_type())
#define MOTO_COMMAND_DELETE(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), MOTO_TYPE_COMMAND_DELETE, MotoCommandDeleteNode))
#define MOTO_COMMAND_DELETE_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST ((klass), MOTO_TYPE_COMMAND_DELETE, MotoCommandDeleteNodeClass))
#define MOTO_IS_COMMAND_DELETE(obj)      (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MOTO_TYPE_COMMAND_DELETE))
#define MOTO_IS_COMMAND_DELETE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), MOTO_TYPE_COMMAND_DELETE))
#define MOTO_COMMAND_DELETE_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), MOTO_TYPE_COMMAND_DELETE, MotoCommandDeleteNodeClass))

MotoCommand *moto_command_delete_new(MotoNode *node);

// MotoCommandChangeParamValueNode

struct _MotoCommandChangeParamValueNode
{
    MotoCommand parent;
};

struct _MotoCommandChangeParamValueNodeClass
{
    MotoCommandClass parent;
};

GType moto_command_change_param_value_get_type(void);

#define MOTO_TYPE_COMMAND_CHANGE_PARAM_VALUE (moto_command_change_param_value_get_type())
#define MOTO_COMMAND_CHANGE_PARAM_VALUE(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), MOTO_TYPE_COMMAND_CHANGE_PARAM_VALUE, MotoCommandChangeParamValueNode))
#define MOTO_COMMAND_CHANGE_PARAM_VALUE_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST ((klass), MOTO_TYPE_COMMAND_CHANGE_PARAM_VALUE, MotoCommandChangeParamValueNodeClass))
#define MOTO_IS_COMMAND_CHANGE_PARAM_VALUE(obj)      (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MOTO_TYPE_COMMAND_CHANGE_PARAM_VALUE))
#define MOTO_IS_COMMAND_CHANGE_PARAM_VALUE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), MOTO_TYPE_COMMAND_CHANGE_PARAM_VALUE))
#define MOTO_COMMAND_CHANGE_PARAM_VALUE_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), MOTO_TYPE_COMMAND_CHANGE_PARAM_VALUE, MotoCommandChangeParamValueNodeClass))

MotoCommand *moto_command_change_param_value_new(MotoParam *param, GValue *new_value);

// MotoCommandChangeParamExprNode

struct _MotoCommandChangeParamExprNode
{
    MotoCommand parent;
};

struct _MotoCommandChangeParamExprNodeClass
{
    MotoCommandClass parent;
};

GType moto_command_change_param_expr_get_type(void);

#define MOTO_TYPE_COMMAND_CHANGE_PARAM_EXPR (moto_command_change_param_expr_get_type())
#define MOTO_COMMAND_CHANGE_PARAM_EXPR(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), MOTO_TYPE_COMMAND_CHANGE_PARAM_EXPR, MotoCommandChangeParamExprNode))
#define MOTO_COMMAND_CHANGE_PARAM_EXPR_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST ((klass), MOTO_TYPE_COMMAND_CHANGE_PARAM_EXPR, MotoCommandChangeParamExprNodeClass))
#define MOTO_IS_COMMAND_CHANGE_PARAM_EXPR(obj)      (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MOTO_TYPE_COMMAND_CHANGE_PARAM_EXPR))
#define MOTO_IS_COMMAND_CHANGE_PARAM_EXPR_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), MOTO_TYPE_COMMAND_CHANGE_PARAM_EXPR))
#define MOTO_COMMAND_CHANGE_PARAM_EXPR_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), MOTO_TYPE_COMMAND_CHANGE_PARAM_EXPR, MotoCommandChangeParamExprNodeClass))

MotoCommand *moto_command_change_param_expr_new(MotoParam *param, const gchar *new_expr);

// MotoCommandLinkParamsNode

struct _MotoCommandLinkParamsNode
{
    MotoCommand parent;
};

struct _MotoCommandLinkParamsNodeClass
{
    MotoCommandClass parent;
};

GType moto_command_link_params_get_type(void);

#define MOTO_TYPE_COMMAND_LINK_PARAMS (moto_command_link_params_get_type())
#define MOTO_COMMAND_LINK_PARAMS(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), MOTO_TYPE_COMMAND_LINK_PARAMS, MotoCommandLinkParamsNode))
#define MOTO_COMMAND_LINK_PARAMS_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST ((klass), MOTO_TYPE_COMMAND_LINK_PARAMS, MotoCommandLinkParamsNodeClass))
#define MOTO_IS_COMMAND_LINK_PARAMS(obj)      (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MOTO_TYPE_COMMAND_LINK_PARAMS))
#define MOTO_IS_COMMAND_LINK_PARAMS_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), MOTO_TYPE_COMMAND_LINK_PARAMS))
#define MOTO_COMMAND_LINK_PARAMS_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), MOTO_TYPE_COMMAND_LINK_PARAMS, MotoCommandLinkParamsNodeClass))

MotoCommand *moto_command_link_params_new(MotoParam *source, MotoParam *sink);

// MotoCommandUnlinkParamsNode

struct _MotoCommandUnlinkParamsNode
{
    MotoCommand parent;
};

struct _MotoCommandUnlinkParamsNodeClass
{
    MotoCommandClass parent;
};

GType moto_command_unlink_params_get_type(void);

#define MOTO_TYPE_COMMAND_UNLINK_PARAMS (moto_command_unlink_params_get_type())
#define MOTO_COMMAND_UNLINK_PARAMS(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), MOTO_TYPE_COMMAND_UNLINK_PARAMS, MotoCommandUnunlinkParamsNode))
#define MOTO_COMMAND_UNLINK_PARAMS_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST ((klass), MOTO_TYPE_COMMAND_UNLINK_PARAMS, MotoCommandUnunlinkParamsNodeClass))
#define MOTO_IS_COMMAND_UNLINK_PARAMS(obj)      (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MOTO_TYPE_COMMAND_UNLINK_PARAMS))
#define MOTO_IS_COMMAND_UNLINK_PARAMS_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), MOTO_TYPE_COMMAND_UNLINK_PARAMS))
#define MOTO_COMMAND_UNLINK_PARAMS_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), MOTO_TYPE_COMMAND_UNLINK_PARAMS, MotoCommandUnunlinkParamsNodeClass))

MotoCommand *moto_command_unlink_params_new(MotoParam *sink);

G_END_DECLS

#endif /* __MOTO_COMMANDS_H__ */

