#include "moto-test-mesh.h"
#include "libmoto/moto-mesh.h"

#define pair moto_half_edge_pair
#define edge moto_half_edge_edge
#define next(he) he_data[he].next
#define prev(he) he_data[he].prev
#define origin(he) e_verts[he]
#define destination(he) origin(pair(he))
#define left(he) he_data[he].f_left
#define vertex_next(he) next(pair(he))
#define vertex_prev(he) pair(prev(he))

#define get_v(x, y, z) \
    ((0 == (x)) ? (y)*(div_z+1) + (z) : ((div_x) == (x)) ? v_num - (div_y+1)*(div_z+1) + (y)*(div_z+1) + (z) : \
        (div_y-1)*(div_z-1) + (div_y+div_z)*2*(x) + ((0 == (y)) ? (z) : ((div_y) == (y)) ? (div_z-1) + 2*(y) + (z) : \
            (div_z-1) + 2*(y) + ((z)?1:0) ))

static MotoMesh *create_mesh_cube(void)
{
    gfloat size_x = 1;
    gfloat size_y = 1;
    gfloat size_z = 1;
    gfloat hsx = size_x / 2;
    gfloat hsy = size_y / 2;
    gfloat hsz = size_z / 2;

    gint div_x = 10;
    gint div_y = 10;
    gint div_z = 10;

    guint v_num = (div_x + div_y)*2 * (div_z + 1) + (((div_x+1)*(div_y+1) - (div_x + div_y)*2) * 2);
    guint e_num = (div_x + div_y)*2 * (div_z + 1) + (div_x + div_y)*2*div_z + div_x*(div_y-1)*2 + div_y*(div_x-1)*2;
    guint f_num = div_x*div_y*2 + div_x*div_z*2 + div_y*div_z*2;

    MotoMesh *mesh = moto_mesh_new(v_num, e_num, f_num, f_num*4);
    if(mesh->b32)
    {
        MotoMeshFace32 *f_data  = (MotoMeshFace32 *)mesh->f_data;
        guint32 *f_verts = (guint32 *)mesh->f_verts;

        guint32 i, j, k, n = 4;
        for(i = 0; i < f_num; i++)
        {
            f_data[i].v_offset = n;
            n += 4;
        }

        guint32 vi = 0, fi = 0;
        for(i = 0; i < div_x+1; i++)
            for(j = 0; j < div_y+1; j++)
                for(k = 0; k < div_z+1; k++)
                {
                    if(0 != i && div_x != i)
                    {
                        if(( ! (0 == k || div_z == k)) && ( ! (0 == j || div_y == j)))
                            continue;
                    }

                    mesh->v_coords[vi].x = -hsx + size_x/div_x * i;
                    mesh->v_coords[vi].y = -hsy + size_y/div_y * j;
                    mesh->v_coords[vi].z = -hsz + size_z/div_z * k;
                    vi++;
                }
        if(1)
        {
            fi = 0;
            // x-axis faces
            for(i = 0; i < div_y; i++)
                for(j = 0; j < div_z; j++)
                {
                    f_verts[fi*4]   = get_v(0, i, j);
                    f_verts[fi*4+1] = get_v(0, i, j+1);
                    f_verts[fi*4+2] = get_v(0, i+1, j+1);
                    f_verts[fi*4+3] = get_v(0, i+1, j);

                    fi++;
                }
            for(i = 0; i < div_y; i++)
                for(j = 0; j < div_z; j++)
                {
                    guint32 v0 = get_v(div_x, i, j),
                            v1 = get_v(div_x, i+1, j),
                            v2 = get_v(div_x, i+1, j+1),
                            v3 = get_v(div_x, i, j+1);
                    f_verts[fi*4]   = v0;
                    f_verts[fi*4+1] = v1;
                    f_verts[fi*4+2] = v2;
                    f_verts[fi*4+3] = v3;

                    fi++;
                }
            // y-axis faces
            for(i = 0; i < div_x; i++)
                for(j = 0; j < div_z; j++)
                {
                    guint32 v0 = get_v(i, 0, j),
                            v1 = get_v(i+1, 0, j),
                            v2 = get_v(i+1, 0, j+1),
                            v3 = get_v(i, 0, j+1);
                    f_verts[fi*4]   = v0;
                    f_verts[fi*4+1] = v1;
                    f_verts[fi*4+2] = v2;
                    f_verts[fi*4+3] = v3;

                    fi++;
                }
            for(i = 0; i < div_x; i++)
                for(j = 0; j < div_z; j++)
                {
                    f_verts[fi*4]   = get_v(i, div_y, j);
                    f_verts[fi*4+1] = get_v(i, div_y, j+1);
                    f_verts[fi*4+2] = get_v(i+1, div_y, j+1);
                    f_verts[fi*4+3] = get_v(i+1, div_y, j);

                    fi++;
                }
            // z-axis faces
            for(i = 0; i < div_x; i++)
                for(j = 0; j < div_y; j++)
                {
                    f_verts[fi*4]   = get_v(i, j, 0);
                    f_verts[fi*4+1] = get_v(i, j+1, 0);
                    f_verts[fi*4+2] = get_v(i+1, j+1, 0);
                    f_verts[fi*4+3] = get_v(i+1, j, 0);

                    fi++;
                }
            for(i = 0; i < div_x; i++)
                for(j = 0; j < div_y; j++)
                {
                    f_verts[fi*4]   = get_v(i, j, div_z);
                    f_verts[fi*4+1] = get_v(i+1, j, div_z);
                    f_verts[fi*4+2] = get_v(i+1, j+1, div_z);
                    f_verts[fi*4+3] = get_v(i, j+1, div_z);

                    fi++;
                }
        }
    }
    else
    {
        MotoMeshFace16 *f_data  = (MotoMeshFace16 *)mesh->f_data;
        guint16 *f_verts = (guint16 *)mesh->f_verts;

        guint16 i, j, k, n = 4;
        for(i = 0; i < f_num; i++)
        {
            f_data[i].v_offset = n;
            n += 4;
        }

        guint16 vi = 0, fi = 0;
        for(i = 0; i < div_x+1; i++)
            for(j = 0; j < div_y+1; j++)
                for(k = 0; k < div_z+1; k++)
                {
                    if(0 != i && div_x != i)
                    {
                        if(( ! (0 == k || div_z == k)) && ( ! (0 == j || div_y == j)))
                            continue;
                    }

                    mesh->v_coords[vi].x = -hsx + size_x/div_x * i;
                    mesh->v_coords[vi].y = -hsy + size_y/div_y * j;
                    mesh->v_coords[vi].z = -hsz + size_z/div_z * k;
                    vi++;
                }
        if(1)
        {
            fi = 0;
            // x-axis faces
            for(i = 0; i < div_y; i++)
                for(j = 0; j < div_z; j++)
                {
                    f_verts[fi*4]   = get_v(0, i, j);
                    f_verts[fi*4+1] = get_v(0, i, j+1);
                    f_verts[fi*4+2] = get_v(0, i+1, j+1);
                    f_verts[fi*4+3] = get_v(0, i+1, j);

                    fi++;
                }
            for(i = 0; i < div_y; i++)
                for(j = 0; j < div_z; j++)
                {
                    guint16 v0 = get_v(div_x, i, j),
                            v1 = get_v(div_x, i+1, j),
                            v2 = get_v(div_x, i+1, j+1),
                            v3 = get_v(div_x, i, j+1);
                    f_verts[fi*4]   = v0;
                    f_verts[fi*4+1] = v1;
                    f_verts[fi*4+2] = v2;
                    f_verts[fi*4+3] = v3;

                    fi++;
                }
            // y-axis faces
            for(i = 0; i < div_x; i++)
                for(j = 0; j < div_z; j++)
                {
                    guint16 v0 = get_v(i, 0, j),
                            v1 = get_v(i+1, 0, j),
                            v2 = get_v(i+1, 0, j+1),
                            v3 = get_v(i, 0, j+1);
                    f_verts[fi*4]   = v0;
                    f_verts[fi*4+1] = v1;
                    f_verts[fi*4+2] = v2;
                    f_verts[fi*4+3] = v3;

                    fi++;
                }
            for(i = 0; i < div_x; i++)
                for(j = 0; j < div_z; j++)
                {
                    f_verts[fi*4]   = get_v(i, div_y, j);
                    f_verts[fi*4+1] = get_v(i, div_y, j+1);
                    f_verts[fi*4+2] = get_v(i+1, div_y, j+1);
                    f_verts[fi*4+3] = get_v(i+1, div_y, j);

                    fi++;
                }
            // z-axis faces
            for(i = 0; i < div_x; i++)
                for(j = 0; j < div_y; j++)
                {
                    f_verts[fi*4]   = get_v(i, j, 0);
                    f_verts[fi*4+1] = get_v(i, j+1, 0);
                    f_verts[fi*4+2] = get_v(i+1, j+1, 0);
                    f_verts[fi*4+3] = get_v(i+1, j, 0);

                    fi++;
                }
            for(i = 0; i < div_x; i++)
                for(j = 0; j < div_y; j++)
                {
                    f_verts[fi*4]   = get_v(i, j, div_z);
                    f_verts[fi*4+1] = get_v(i+1, j, div_z);
                    f_verts[fi*4+2] = get_v(i+1, j+1, div_z);
                    f_verts[fi*4+3] = get_v(i, j+1, div_z);

                    fi++;
                }
        }
    }

    return mesh;
}
#undef get_v

static void moto_test_mesh_he_invariants(void)
{
    // TODO: Run same test for other meshes.
    MotoMesh *mesh = create_mesh_cube();
    g_assert(mesh != NULL);
    g_assert(moto_mesh_prepare(mesh));

    MotoHalfEdge16* he_data = (MotoHalfEdge16*)mesh->he_data;
    guint16* e_verts = (guint16*)mesh->e_verts;

    guint16 he;
    for(he = 0; he < mesh->e_num*2; ++he)
    {
        g_assert(pair(pair(he)) == he);
        g_assert(origin(next(he)) == destination(he));
        g_assert(origin(he) == destination(prev(he)));
        g_assert(edge(he) == edge(pair(he)));
        g_assert(left(he) == left(next(he)));
        g_assert(left(he) == left(prev(he)));
        g_assert(next(prev(he)) == he);
        g_assert(prev(next(he)) == he);
        g_assert(vertex_next(vertex_prev(he)) == he);
        g_assert(vertex_prev(vertex_next(he)) == he);
    }

    g_object_unref(mesh);
}

#undef pair
#undef edge
#undef next
#undef prev
#undef origin
#undef destination
#undef left
#undef vertex_next
#undef vertex_prev

void moto_collect_mesh_tests(void)
{
    g_test_add_func("/moto/mesh/half-edge-invariants", moto_test_mesh_he_invariants);
}
