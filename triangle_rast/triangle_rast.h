#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../GCGraLib2/GCGraLib2.h"

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

extern int	polx[], poly[];

typedef struct {
	int x0,y0,x1,y1,x2,y2;
} Triangle;

typedef struct {
	float yymax;
	float yymin;
	float xa;
	float dx;
} EdgeTable;

void scanconv(SDL_Renderer *ren, int n);
void tri_rast(SDL_Renderer *ren, Triangle *triangle);
