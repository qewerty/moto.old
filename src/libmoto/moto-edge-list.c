#include "moto-edge-list.h"

/* EdgeList */

MotoEdgeList *moto_edge_list_new(guint32 ei, MotoEdgeList *head)
{
    MotoEdgeList *el = g_slice_new(MotoEdgeList);
    el->ei = ei;
    el->next = head;

    return el;
}

guint32 moto_edge_list_find_edge_and_remove(MotoEdgeList **head,
                                            guint32 *e_verts, guint32 a, guint32 b)
{
    MotoEdgeList *el = *head;
    MotoEdgeList *prev = NULL;
    while(el)
    {
        guint32 i = el->ei*2;
        if((e_verts[i] == a && e_verts[i+1] == b) || \
           (e_verts[i+1] == a && e_verts[i] == b))
        {
            guint32 ei = el->ei;
            if(el == *head)
                *head = el->next;
            if(prev)
                prev->next = el->next;
            g_slice_free(MotoEdgeList, el);
            return ei;
        }

        prev = el;
        el = el->next;
    }
    return G_MAXUINT32;
}

guint32 moto_edge_list_find_and_remove(MotoEdgeList **head, guint32 ei)
{
    MotoEdgeList *el = *head;
    MotoEdgeList *prev = NULL;
    while(el)
    {
        if(ei == el->ei)
        {
            guint32 ei = el->ei;
            if(el == *head)
                *head = el->next;
            if(prev)
                prev->next = el->next;
            g_slice_free(MotoEdgeList, el);
            return ei;
        }

        prev = el;
        el = el->next;
    }
    return G_MAXUINT32;
}

void moto_edge_list_remove_all(MotoEdgeList *el)
{
    MotoEdgeList *tmp = el;
    while(el)
    {
        tmp = el;
        el = el->next;
        g_slice_free(MotoEdgeList, tmp);
    }
}

/* EdgeList16 */

MotoEdgeList16 *moto_edge_list16_new(guint16 ei, MotoEdgeList16 *head)
{
    MotoEdgeList16 *el = g_slice_new(MotoEdgeList16);
    el->ei = ei;
    el->next = head;

    return el;
}

guint16 moto_edge_list16_find_edge_and_remove(MotoEdgeList16 **head,
                                              guint16 *e_verts, guint16 a, guint16 b)
{
    MotoEdgeList16 *el = *head;
    MotoEdgeList16 *prev = NULL;
    while(el)
    {
        guint16 i = el->ei*2;
        if((e_verts[i] == a && e_verts[i+1] == b) || \
           (e_verts[i+1] == a && e_verts[i] == b))
        {
            guint16 ei = el->ei;
            if(el == *head)
                *head = el->next;
            if(prev)
                prev->next = el->next;
            g_slice_free(MotoEdgeList16, el);
            return ei;
        }

        prev = el;
        el = el->next;
    }
    return G_MAXUINT16;
}

guint16 moto_edge_list16_find_and_remove(MotoEdgeList16 **head, guint16 ei)
{
    MotoEdgeList16 *el = *head;
    MotoEdgeList16 *prev = NULL;
    while(el)
    {
        if(ei == el->ei)
        {
            guint16 ei = el->ei;
            if(el == *head)
                *head = el->next;
            if(prev)
                prev->next = el->next;
            g_slice_free(MotoEdgeList16, el);
            return ei;
        }

        prev = el;
        el = el->next;
    }
    return G_MAXUINT16;
}

void moto_edge_list16_remove_all(MotoEdgeList16 *el)
{
    MotoEdgeList16 *tmp = el;
    while(el)
    {
        tmp = el;
        el = el->next;
        g_slice_free(MotoEdgeList16, tmp);
    }
}
