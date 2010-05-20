
#ifndef __moto_cclosure_marshal_MARSHAL_H__
#define __moto_cclosure_marshal_MARSHAL_H__

#include	<glib-object.h>

G_BEGIN_DECLS

/* VOID:OBJECT,INT,INT,INT,INT (marshal.txt:1) */
extern void moto_cclosure_marshal_VOID__OBJECT_INTINTINTINT (GClosure     *closure,
                                                                GValue       *return_value,
                                                                guint         n_param_values,
                                                                const GValue *param_values,
                                                                gpointer      invocation_hint,
                                                                gpointer      marshal_data);

/* BOOLEAN:OBJECT,INT,INT,INT,INT (marshal.txt:2) */
extern void moto_cclosure_marshal_BOOL_OBJECT_INTINTINTINT (GClosure     *closure,
                                                                   GValue       *return_value,
                                                                   guint         n_param_values,
                                                                   const GValue *param_values,
                                                                   gpointer      invocation_hint,
                                                                   gpointer      marshal_data);

G_END_DECLS

#endif /* __moto_cclosure_marshal_MARSHAL_H__ */

