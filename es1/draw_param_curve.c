#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../GCGraLib2/GCGraLib2.h"

#define DIM 2000

#define WINDOW_WIDTH	780
#define WINDOW_HEIGHT 	440

#define PLOT_WIDTH		(WINDOW_WIDTH -210)
#define PLOT_HEIGHT		(WINDOW_HEIGHT - 60)

#define DEFAULT_TEXTSIZE  18

typedef struct RECT RECT;
struct RECT
{
	float xmin, xmax, ymin, ymax;
};

typedef RECT VIEWPORT;
typedef RECT WINDOW;

void init(SDL_Window **win, SDL_Renderer **ren, TTF_Font **font, SDL_Rect *v) {

	if (SDL_Init (SDL_INIT_VIDEO) < 0)
	{
		fprintf (stderr, "Couldn't init video: %s\n", SDL_GetError ());
		exit(1);
	}

	/* Initialize the TTF library */
	if (TTF_Init () < 0)
	{
		fprintf (stderr, "Couldn't initialize TTF: %s\n", SDL_GetError ());
		SDL_Quit ();
		exit(2);
	}

	*font = TTF_OpenFont ("/usr/share/fonts/truetype/freefont/FreeSans.ttf", DEFAULT_TEXTSIZE);
	if (*font == NULL)
	{
		fprintf (stderr, "Couldn't load font\n");
	}

	// Dimensioni finestra
	v->x = 0;
	v->y = 0;
	v->w = WINDOW_WIDTH;
	v->h = WINDOW_HEIGHT;

	*win= SDL_CreateWindow("Draw Parametric Curve", v->x, v->y, v->w, v->h, SDL_WINDOW_SHOWN);
	if(*win==NULL){
		fprintf(stderr,"SDL_CreateWindow Error: %s\n",SDL_GetError());
		SDL_Quit();
		exit(1);
	}

	*ren = SDL_CreateRenderer(*win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (*ren == NULL){
		SDL_DestroyWindow(*win);
		fprintf(stderr,"SDL_CreateRenderer Error: %s\n",SDL_GetError());
		SDL_Quit();
		exit(1);
	}
}

void show_background(SDL_Renderer *ren, TTF_Font *font, SDL_Rect *v) {
	SDL_SetRenderDrawColor(ren, 50, 50, 50, 255);
	SDL_RenderClear(ren);
	SDL_RenderPresent(ren);

	SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
	SDL_RenderFillRect (ren, v);
	GC_DrawText (ren, font, 255, 0, 0, 0, 255, 255, 255, 0, "Grafico Curva", 600, 50, shaded);
	SDL_RenderPresent(ren);
}

void get_scale (RECT r1, RECT r2, float *scx, float *scy)
{
	*scx = (r1.xmax - r1.xmin) / (r2.xmax - r2.xmin);
	*scy = (r1.ymax - r1.ymin) / (r2.ymax - r2.ymin);
}

void window_to_viewport (float px, float py, int *ix, int *iy, VIEWPORT view, WINDOW win)
{
	float scx, scy;
	get_scale (view, win, &scx, &scy);
	*ix = 0.5 + scx * (px - win.xmin) + view.xmin;
	*iy = 0.5 + scy * (win.ymin - py) + view.ymax;
}

void draw_data (SDL_Renderer* ren, int n, VIEWPORT view, WINDOW win, float x[], float y[])
{
	int i;
	int x1, y1, x2, y2;

	window_to_viewport (x[0], y[0], &x1, &y1, view, win);

	for (i = 1; i < n; i++)
	{
		window_to_viewport (x[i], y[i], &x2, &y2, view, win);
		SDL_RenderDrawLine(ren, x1, y1, x2, y2);

		x1 = x2;
		y1 = y2;
	}

}

void strange_function(float t, float *x, float *y) {
	int a, b, c, d, j, k;
	j = 3;
	k = 4;

	a = 1;
	b = 80;
	c = 80;
	d = 1;

	*x = cos(a * 2*M_PI*t) - pow(cos(b * 2*M_PI*t), j);
	*y = sin(c * 2*M_PI*t) - pow(sin(d * 2*M_PI*t), k);
}

void lissajous_3_2(float t, float *x, float *y) {
	*x = cos(3*2*M_PI*t);
	*y = sin(2*2*M_PI*t);
}

void parabole(float t, float *x, float *y) {
	*x = 6*t - 3;
	*y = pow(6*t - 3, 2);
}

void circle(float t, float *x, float *y) {
	*x = cos(2*M_PI*t);
	*y = sin(2*M_PI*t);
}

void adapt_window(int n, WINDOW* rect, float x[], float y[]) {
	float w, h, diff;
	float viewport_ratio, data_ratio;

	// Allarga il rettangolo per contenere i max e i min x/y
	rect->xmin=rect->xmax=x[0];
	rect->ymin=rect->ymax=y[0];
	for (int i = 1; i < n; i++)
	{
		if(x[i]<rect->xmin) rect->xmin=x[i];
		else if(x[i]>rect->xmax) rect->xmax=x[i];

		if(y[i]<rect->ymin) rect->ymin=y[i];
		else if(y[i]>rect->ymax) rect->ymax=y[i];
	}

	// Aggiusta le dimensioni per mantenere il rateo altezza larghezza della viewport
	w = rect->xmax - rect->xmin;
	h = rect->ymax - rect->ymin;

	data_ratio = w / h;
	viewport_ratio =  (float)PLOT_WIDTH / PLOT_HEIGHT;

	if(data_ratio > viewport_ratio) {
		// Allarga altezza
		diff = ((w / PLOT_WIDTH * PLOT_HEIGHT)	 - h ) / 2;
		rect->ymax += diff;
		rect->ymin -= diff;
	} else if(data_ratio < viewport_ratio) {
		// Allarga larghezza
		diff = ((h / PLOT_HEIGHT * PLOT_WIDTH) - w) / 2;
		rect->xmax += diff;
		rect->xmin -= diff;
	}

}

void get_data(void (*functionPtr)(float, float*, float*), int n, WINDOW* rect, float x[], float y[]) {
	float step, t;

	// calcola step
	step = 1.0 / (n-1);

	for(int i=0; i<n; i++) {
		t = i*step;
		(*functionPtr)(t, &x[i], &y[i]);
	}

	adapt_window(n, rect, x, y);
}

int main(int argc, char const *argv[])
{
	SDL_Window *win;
	SDL_Renderer *ren;
	TTF_Font *font;
	SDL_Rect sub_v, v;
	VIEWPORT fun_view;
	WINDOW fun_win;
	int n;
	float x[DIM], y[DIM];
	char ch;

	init(&win, &ren, &font, &v);

	show_background(ren, font, &v);

	// sub viewport con height e width
	sub_v.x = v.x + 10;
	sub_v.y = v.y + 10;
	sub_v.w = PLOT_WIDTH;
	sub_v.h = PLOT_HEIGHT;

	// viewport della funzione da disegnare con coordinate min/max della x e y
	fun_view.xmin = sub_v.x;
	fun_view.xmax = sub_v.x + sub_v.w - 1;
	fun_view.ymin = sub_v.y;
	fun_view.ymax = sub_v.y + sub_v.h - 1;

	do {
		int plotting_fun;

		// draw bg del grafico
		SDL_SetRenderDrawColor(ren, 240, 240, 240, 255);
		SDL_RenderFillRect (ren, &sub_v);

		n = DIM;

 		printf("Scegli funzione (1-4): ");
		scanf("%d", &plotting_fun);

 		switch(plotting_fun) {
			case 1:
				get_data(&parabole, n, &fun_win, x, y);
				break;
			case 2:
				get_data(&circle, n, &fun_win, x, y);
				break;
			case 3:
				get_data(&lissajous_3_2, n, &fun_win, x, y);
				break;
			case 4:
				get_data(&strange_function, n, &fun_win, x, y);
				break;
			default:
				get_data(&parabole, n, &fun_win, x, y);
		}

		// draw grafico
		SDL_SetRenderDrawColor(ren, 0, 0, 255, 255);
		draw_data (ren, n, fun_view, fun_win, x, y);

		SDL_RenderPresent(ren);

		printf ("\n <e> EXIT   <f> NEW DATA  ");
		while(getchar() != '\n');
		scanf ("%c", &ch);
	} while ((ch != 'E') && (ch != 'e'));

	TTF_CloseFont (font);
	SDL_DestroyRenderer(ren);
	SDL_DestroyWindow(win);
	TTF_Quit ();
	SDL_Quit ();
	return 0;
}
