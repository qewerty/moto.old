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

#ifndef MOTO_EDGE_LIST_H
#define MOTO_EDGE_LIST_H

#include <glib.h>

typedef struct _MotoEdgeList MotoEdgeList;
typedef struct _MotoEdgeList16 MotoEdgeList16;

struct _MotoEdgeList
{
    MotoEdgeList *next;
    guint32 ei;
};

MotoEdgeList *moto_edge_list_new(guint32 ei, MotoEdgeList *head);
guint32 moto_edge_list_find_edge_and_remove(MotoEdgeList **head,
                                            guint32 *e_verts, guint32 a, guint32 b);
guint32 moto_edge_list_find_and_remove(MotoEdgeList **head, guint32 ei);
void moto_edge_list_remove_all(MotoEdgeList *el);

struct _MotoEdgeList16
{
    MotoEdgeList16 *next;
    guint16 ei;
};

MotoEdgeList16 *moto_edge_list16_new(guint16 ei, MotoEdgeList16 *head);
guint16 moto_edge_list16_find_edge_and_remove(MotoEdgeList16 **head,
                                            guint16 *e_verts, guint16 a, guint16 b);
guint16 moto_edge_list16_find_and_remove(MotoEdgeList16 **head, guint16 ei);
void moto_edge_list16_remove_all(MotoEdgeList16 *el);

#endif /* MOTO_EDGE_LIST_H */
