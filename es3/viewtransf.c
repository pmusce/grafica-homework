#include <math.h>
#include <stdio.h>
#include "viewtransf.h"

void calculate_vp(vec3 *vp) {
    vp->x = D * sin(fi) * cos(teta);
    vp->y = D * sin(fi) * sin(teta);
    vp->z = D * cos(fi);
}

float norma(vec3 *v) {
    return sqrt(pow(v->x, 2) + pow(v->y, 2) + pow(v->z, 2));
}

void normalize_vect(vec3 *v) {
    float n = norma(v);
    v->x = v->x / n;
    v->y = v->y / n;
    v->z = v->z / n;
}

void calculate_xaxis(vec3 *u, vec3 *v, vec3 *xaxis) {
    xaxis->x = u->y * v->z - u->z * v->y;
    xaxis->y = u->z * v->x - u->x * v->z;
    xaxis->z = u->x * v->y - u->y * v->x;
    normalize_vect(xaxis);
}

void calculate_yaxis(vec3 *u, vec3 *v, vec3 *yaxis) {
    yaxis->x = -1 * (u->y * v->z - u->z * v->y);
    yaxis->y = -1 * (u->z * v->x - u->x * v->z);
    yaxis->z = -1 * (u->x * v->y - u->y * v->x);
    normalize_vect(yaxis);
}

void calculate_zaxis(vec3 *vp, vec3 *target, vec3 *zaxis) {
    zaxis->x = -1 * (vp->x - target->x);
    zaxis->y = -1 * (vp->y - target->y);
    zaxis->z = -1 * (vp->z - target->z);
    normalize_vect(zaxis);
}

void trasf_view_up_vect(float x, float y, float z, float *xe, float *ye, float *ze) {
    vec3 xaxis, yaxis, zaxis;
    vec3 vp;
    vec3 target;
    vec3 view_up;
    float M[4][4];

    view_up.x = 0;
    view_up.y = 1;
    view_up.z = 0;

    target.x = csx;
    target.y = csy;
    target.z = csz;

    calculate_vp(&vp);
    calculate_zaxis(&vp, &target, &zaxis);
    calculate_xaxis(&zaxis, &view_up, &xaxis);
    calculate_yaxis(&zaxis, &xaxis, &yaxis);

 /* A */
    M[0][0] = xaxis.x;
    M[0][1] = yaxis.x;
    M[0][2] = zaxis.x;
    M[0][3] = 0;

    M[1][0] = xaxis.y;
    M[1][1] = yaxis.y;
    M[1][2] = zaxis.y;
    M[1][3] = 0;

    M[2][0] = xaxis.z;
    M[2][1] = yaxis.z;
    M[2][2] = zaxis.z;
    M[2][3] = 0;

    M[3][0] = 0;
    M[3][1] = 0;
    M[3][2] = D;
    M[3][3] = 1;

    (*xe) = M[0][0] * x + M[1][0] * y + M[2][0] * z + M[3][0];
    (*ye) = M[0][1] * x + M[1][1] * y + M[2][1] * z + M[3][1];
    (*ze) = M[0][2] * x + M[1][2] * y + M[2][2] * z + M[3][2];
}
