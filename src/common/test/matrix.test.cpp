#include <iostream>
#include <assert.h>

#include "numdef.h"
#include "matrix.h"

using namespace std;

// print

// Print column order matrix in math view.
void print_matrix44(double mat[16])
{
    for(int i = 0; i < 4; i++)
    {
        for(int j = 0; j < 4; j++)
        {
            cout << mat[j*4+i] << ' ';
        }
        cout << endl;
    }
}
void print_matrix44(float mat[16])
{
    for(int i = 0; i < 4; i++)
    {
        for(int j = 0; j < 4; j++)
        {
            cout << mat[j*4+i] << ' ';
        }
        cout << endl;
    }
}

void print_matrix33(double mat[9])
{
    for(int i = 0; i < 3; i++)
    {
        for(int j = 0; j < 3; j++)
        {
            cout << mat[j*3+i] << ' ';
        }
        cout << endl;
    }
}
void print_matrix33(float mat[9])
{
    for(int i = 0; i < 3; i++)
    {
        for(int j = 0; j < 3; j++)
        {
            cout << mat[j*3+i] << ' ';
        }
        cout << endl;
    }
}

void print_vector4(double vec[3])
{
    cout << vec[0] << ' ' << vec[1] << ' ' << vec[2] << ' ' << vec[3] << endl;
}
void print_vector4(float vec[3])
{
    cout << vec[0] << ' ' << vec[1] << ' ' << vec[2] << ' ' << vec[3] << endl;
}

void print_vector3(double vec[3])
{
    cout << vec[0] << ' ' << vec[1] << ' ' << vec[2] << endl;
}
void print_vector3(float vec[3])
{
    cout << vec[0] << ' ' << vec[1] << ' ' << vec[2] << endl;
}

void print_vector2(double vec[2])
{
    cout << vec[0] << ' ' << vec[1] << endl;
}
void print_vector2(float vec[2])
{
    cout << vec[0] << ' ' << vec[1] << endl;
}

// test

void test_copy_length_normalize() // and vector*_equal
{
    float buf;
    
    // 3
    float initial3[3] = {3, 0, 0};
    float vec3[3];
    
    vector3_copy(vec3, initial3);
    vector3_normalize(vec3, buf);
    
    float r3[3] = {1, 0, 0};
    assert(vector3_equal(vec3, r3));
    assert(not vector3_equal(vec3, initial3));
    
    // 2
    float initial2[2] = {3, 0};
    float vec2[2];
    
    vector2_copy(vec2, initial2);
    vector2_normalize(vec2, buf);
    
    float r2[2] = {1, 0};
    assert(vector2_equal(vec2, r2));
    assert(not vector2_equal(vec2, initial2));
}


void test_matrix_identity()
{
    // 44
    float identity44[16] = {1, 0, 0, 0,
                            0, 1, 0, 0,
                            0, 0, 1, 0,
                            0, 0, 0, 1};
    float mat44[16];
    matrix44_identity(mat44);
    
    assert(matrix44_equal(mat44, identity44));
    
    // 33
    float identity33[9] = {1, 0, 0,
                           0, 1, 0,
                           0, 0, 1};
    float mat33[9];
    matrix33_identity(mat33);
    
    assert(matrix33_equal(mat33, identity33));
}

void test_matrix_copy()
{
    // 44
    float mat44[16], mat44_2[16];
    matrix44_rotate_x(mat44, 45*RAD_PER_DEG);
    
    matrix44_copy(mat44_2, mat44);
    
    assert(matrix44_equal(mat44, mat44_2));
    
    // 33
    float mat33[9], mat33_2[9];
    matrix33_rotate(mat33, 45*RAD_PER_DEG);
    
    matrix33_copy(mat33_2, mat33);
    
    assert(matrix33_equal(mat33, mat33_2));
}

void test_matrix_rotate()
{
    // 44
    
    // x
    float a = -90*RAD_PER_DEG;

    float rot_x[16] = {1,       0,      0, 0,
                       0,  cos(a), sin(a), 0,
                       0, -sin(a), cos(a), 0,
                       0,       0,      0, 1};
    
    float mat44[16];
    matrix44_rotate_x(mat44, a);
    
    assert(matrix44_equal_dif(mat44, rot_x, MICRO));
    
    float vec3[] = {0, 10, 0};
    float r3[3];
    vector3_transform(r3, mat44, vec3);
    
    float t3[] = {0, 0, -10};
    assert(vector3_equal_dif(r3, t3, MICRO));
    
    // y
    a = 90*RAD_PER_DEG;
    float rot_y[16] = {cos(a), 0, -sin(a), 0,
                            0, 1,       0, 0,
                       sin(a), 0,  cos(a), 0,
                            0, 0,       0, 1};
                            
    matrix44_rotate_y(mat44, a);
    
    assert(matrix44_equal_dif(mat44, rot_y, MICRO));
    
    vector3_set(vec3, 0, 0, 10);
    vector3_transform(r3, mat44, vec3);
    
    vector3_set(t3, 10, 0, 0);
    assert(vector3_equal_dif(r3, t3, MICRO));
    
    // z
    a = 90*RAD_PER_DEG;
    float rot_z[16] = { cos(a), sin(a), 0, 0,
                       -sin(a), cos(a), 0, 0,
                             0,      0, 1, 0,
                             0,      0, 0, 1};
                             
    matrix44_rotate_z(mat44, a);
    
    assert(matrix44_equal_dif(mat44, rot_z, MICRO));
    
    vector3_set(vec3, 10, 0, 0);
    vector3_transform(r3, mat44, vec3);
    
    vector3_set(t3, 0, 10, 0);
    assert(vector3_equal_dif(r3, t3, MICRO));
    
    // 33
    a = 90*RAD_PER_DEG;
    float rot[9] = { cos(a), sin(a), 0,
                    -sin(a), cos(a), 0,
                          0,     0,  1};
                          
    float mat33[9];
    matrix33_rotate(mat33, a);
    assert(matrix33_equal_dif(mat33, rot, MICRO));
    
    float vec2[] = {10, 0};
    float r2[2];
    vector2_transform(r2, mat33, vec2);
    
    float t2[] = {0, 10};
    assert(vector2_equal_dif(r2, t2, MICRO));
}

void test_matrix_det()
{
    float m[16];
    matrix44_identity(m);
    assert(matrix44_det(m) == 1);
}

void test_matrix_inverse()
{
    float detbuf;
    
    // 4x4
    float mat44[16], mat44_rev[16], ambuf[16];
    matrix44_translate(mat44, 1, 2, 3);
    matrix44_inverse(mat44_rev, mat44, ambuf, detbuf);
    
    float identity44[16] = {1, 0, 0, 0,
                            0, 1, 0, 0,
                            0, 0, 1, 0,
                            0, 0, 0, 1};
    float mult[16];
    matrix44_mult(mult, mat44, mat44_rev);
    
    assert(matrix44_equal_dif(identity44, mult, MICRO));
    
    // 3x3
    float mat33[9], mat33_rev[9];
    matrix33_rotate(mat33, 38.2*RAD_PER_DEG);
    matrix33_inverse(mat33_rev, mat33, ambuf, detbuf);
    
    float identity33[] = {1, 0, 0,
                          0, 1, 0,
                          0, 0, 1};
    matrix33_mult(mult, mat33, mat33_rev);
    
    assert(matrix33_equal_dif(identity33, mult, MICRO));
}

void test_transform()
{
    float norm[3] = {10, 0, 0};
    float mat44[16], transbuf[16], revbuf[16],
          nr[3];
    
    matrix44_rotate_y(mat44, 90*RAD_PER_DEG);
    
    normal3_transform(nr, mat44, norm, transbuf, revbuf);
    
    float nt[] = {0, 0, -10};
    assert(vector3_equal_dif(nt, nr, MICRO));
}

void test_vector3_cross()
{
    float a[] = {10, 0, 0};
    float b[] = {0, 10, 0};
    float c[3];
    float lenbuf;
    
    vector3_cross(c, a, b);
    vector3_normalize(c, lenbuf);
    
    float t[] = {0, 0, 1};
    assert(vector3_equal_dif(t, c, MICRO));
    
    vector3_cross(c, b, a);
    vector3_normalize(c, lenbuf);
    
    vector3_set(t, 0, 0, -1);
    assert(vector3_equal_dif(t, c, MICRO));
}

void test_vector_mult()
{
    float v3[] = {1, 2, 3};
    float v2[] = {1, 2};
    float r3[3], r2[2];
    
    vector3_mult(r3, v3, 2);
    vector2_mult(r2, v2, 2);
    
    float t3[] = {2, 4, 6};
    float t2[] = {2, 4};
    
    assert(vector3_equal_dif(r3, t3, MICRO));
    assert(vector2_equal_dif(r2, t2, MICRO));
}

void test_quaternion()
{
    float i[] = {0, 0, 0, 1};
    float q[4], mult[4], inv[4];
    float buf[3];

    quat_from_axis(q, -90*RAD_PER_DEG, 0, 1, 0);
    quat_inverse(inv, q, buf[0]);

    quat_mult(mult, q, inv, buf);

    float v[4] = {1, 0, 0, 0};
    float r[4], r2[4];
    quat_mult(r, q, v, buf);
    quat_mult(r2, r, inv, buf);

    hvector4_compute(r2);
    vector3_normalize(r2, buf[0]);

    assert(quat_equal_dif(mult, i, MICRO));
}

int main(int argc, char *argv[])
{
    cout << "Testing \"matrix.h\" ... " << endl;
    
    test_matrix_identity();
    test_copy_length_normalize();
    test_matrix_copy();
    test_matrix_rotate();
    test_matrix_det();
    test_matrix_inverse();
    test_transform();
    test_vector3_cross();
    test_vector_mult();
    test_quaternion();
    
    cout << "OK" << endl;
    return 0;
}
