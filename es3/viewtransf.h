typedef struct vec3 vec3;
struct vec3 {
    float x;
    float y;
    float z;
};

extern float   csx,csy,csz;
extern float   D,teta,fi,di;

void calculate_vp(vec3 *vp, float D, float teta, float fi);
float norma(vec3 *v);
void normalize_vect(vec3 *v);
void calculate_xaxis(vec3 *u, vec3 *v, vec3 *xaxis);
void calculate_yaxis(vec3 *u, vec3 *v, vec3 *yaxis);
void calculate_zaxis(vec3 *vp, vec3 *target, vec3 *zaxis);
void general_trasf_view_up_vect(float x, float y, float z, float *xe, float *ye, float *ze, float D, float teta, float fi, vec3 *view_up);
void trasf_view_up_vect(float x, float y, float z, float *xe, float *ye, float *ze);
