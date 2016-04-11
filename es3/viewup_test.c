#include <assert.h>
#include <stdio.h>
#include <math.h>
#include "viewtransf.h"

float csx,csy,csz;
float D,teta,fi;

float equal_f3(float a, float b) {
    return round(a*1000) == round(b*1000);
}

static void test_norma() {
    vec3 v;
    v.x = 1;
    v.y = 0;
    v.z = 0;
    assert(norma(&v) == 1);

    v.x = (float)0;
    v.y = (float)(-3);
    v.z = (float)4;
    assert(norma(&v) == 5);
}

static void test_normalize() {
    vec3 v;
    v.x = 1;
    v.y = 0;
    v.z = 0;

    normalize_vect(&v);
    assert(v.x == 1);
    assert(v.y == 0);
    assert(v.z == 0);

    v.x = 3;
    v.y = 1;
    v.z = 2;
    normalize_vect(&v);
    assert(equal_f3(v.x, 0.802));
    assert(equal_f3(v.y, 0.267));
    assert(equal_f3(v.z, 0.535));
}

static void test_vp() {
    vec3 vp;

    D       = 1;
    teta    = 0;
    fi      = 0;
    calculate_vp(&vp);
    assert(vp.x == 0);
    assert(vp.y == 0);
    assert(vp.z == 1);

    D       = 1;
    teta    = M_PI / 2;
    fi      = M_PI / 2;
    calculate_vp(&vp);
    assert(round(vp.x) == 0);
    assert(round(vp.y) == 1);
    assert(round(vp.z) == 0);
}

int main(int argc, char const *argv[]) {
    test_norma();
    test_normalize();
    test_vp();
    return 0;
}
