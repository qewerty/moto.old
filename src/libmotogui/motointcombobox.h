/* Adapted from The GIMP */

#ifndef __MOTO_INTCOMBO_BOX_H__
#define __MOTO_INTCOMBO_BOX_H__

#include "motointstore.h"

G_BEGIN_DECLS

#define MOTO_TYPE_INTCOMBO_BOX            (moto_intcombo_box_get_type ())
#define MOTO_INTCOMBO_BOX(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), MOTO_TYPE_INTCOMBO_BOX, MotoIntComboBox))
#define MOTO_INTCOMBO_BOX_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), MOTO_TYPE_INTCOMBO_BOX, MotoIntComboBoxClass))
#define MOTO_IS_INTCOMBO_BOX(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MOTO_TYPE_INTCOMBO_BOX))
#define MOTO_IS_INTCOMBO_BOX_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), MOTO_TYPE_INTCOMBO_BOX))
#define MOTO_INTCOMBO_BOX_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), MOTO_TYPE_INTCOMBO_BOX, MotoIntComboBoxClass))

typedef struct _MotoIntComboBox       MotoIntComboBox;
typedef struct _MotoIntComboBoxClass  MotoIntComboBoxClass;

struct _MotoIntComboBox
{
    GtkComboBox       parent;

    /*< private >*/
    gpointer          priv;
};

struct _MotoIntComboBoxClass
{
    GtkComboBoxClass  parent;
};


typedef  gboolean (* MotoIntSensitivityFunc)(gint      value,
                                             gpointer  data);



GType       moto_intcombo_box_get_type       (void) G_GNUC_CONST;

GtkWidget * moto_intcombo_box_new            (const gchar     *first_label,
                                               gint             first_value,
                                               ...) G_GNUC_NULL_TERMINATED;
GtkWidget * moto_intcombo_box_new_valist     (const gchar     *first_label,
                                               gint             first_value,
                                               va_list          values);

GtkWidget * moto_intcombo_box_new_array      (gint             n_values,
                                               const gchar     *labels[]);

void        moto_intcombo_box_prepend        (MotoIntComboBox *combo_box,
                                               ...);
void        moto_intcombo_box_append         (MotoIntComboBox *combo_box,
                                               ...);

gboolean    moto_intcombo_box_set_active     (MotoIntComboBox *combo_box,
                                               gint             value);
gboolean    moto_intcombo_box_get_active     (MotoIntComboBox *combo_box,
                                               gint            *value);

gulong      moto_intcombo_box_connect        (MotoIntComboBox *combo_box,
                                               gint             value,
                                               GCallback        callback,
                                               gpointer         data);

void        moto_intcombo_box_set_sensitivity(MotoIntComboBox        *combo_box,
                                               MotoIntSensitivityFunc  func,
                                               gpointer                data,
                                               GDestroyNotify          destroy);


G_END_DECLS

#endif  /* __MOTO_INTCOMBO_BOX_H__ */
