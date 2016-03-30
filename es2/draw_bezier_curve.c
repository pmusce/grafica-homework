#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../GCGraLib2/GCGraLib2.h"

#define DIM 100
#define POINTS 40

/*funzione di redraw*/
void redraw(int n, int x[], int y[],SDL_Renderer *ren)
{
	int i;

	GC_FillCircle(ren,x[0],y[0],3);
	for (i=1; i<=n; i++)
	{
		SDL_RenderDrawLine(ren, x[i-1], y[i-1], x[i], y[i]);
		GC_FillCircle(ren,x[i],y[i],3);
	}
}

void deCasteljau(float t, int n, int x[], int y[], int *xt, int *yt) {
	int x1[n-1], y1[n-1];

	if(n ==	 1) {
		*xt = x[0];
		*yt = y[0];
		return;
	}

	for(int i=0; i<n-1; i++) {
		x1[i] = x[i] * (1-t) + x[i+1] * t;
		y1[i] = y[i] * (1-t) + y[i+1] * t;
	}

	deCasteljau(t, n-1, x1, y1, xt, yt);
}

void draw_Bezier_Curve(int n, int x[], int y[], SDL_Renderer *ren) {
	float t;
	int c_x[POINTS], c_y[POINTS];

	for(int i = 0; i<POINTS; i++) {
		t = (float)i/(POINTS-1);
		
		deCasteljau(t, n, x, y, &c_x[i], &c_y[i]);

		if(i > 0)
			SDL_RenderDrawLine(ren, c_x[i-1], c_y[i-1], c_x[i], c_y[i]);
	}
}

void addControlPoint(SDL_Renderer *ren, SDL_Event *event, int x[], int y[], int *num_points) {
	int n = *num_points;
	SDL_SetRenderDrawColor(ren, 255, 0, 50, 255);
	x[n]=event->button.x;
	y[n]=event->button.y;
	GC_FillCircle(ren,x[n],y[n],3);
	if(n>0) 
		SDL_RenderDrawLine(ren, x[n-1], y[n-1], x[n], y[n]);
	*(num_points) = n+1;
}

int almost(int a, int b) {
	if(abs(a-b) < 5)
		return 1;
	return 0;
}

int getClickedPoint(SDL_Event *event, int x[], int y[], int n) {
	int click_x=event->button.x;
	int click_y=event->button.y;
	for(int i=0; i<n; i++) {
		if(almost(click_x, x[i]) && almost(click_y, y[i]))
			return i;
	}
	return (-1);
}

/*semplice programma di prova di input da mouse e tastiera*/
int main(void) 
{
	SDL_Window *win;
	SDL_Renderer *ren;
	SDL_Event event;
	int vxmax,vymax;
	int esc=1,n=0;
	int x[DIM],y[DIM];
	int is_curve_drawn = 0;
	int clicked = -1;
	Uint32 windowID;


	if(SDL_Init(SDL_INIT_VIDEO)<0)
	{
		fprintf(stderr,"Couldn't init video: %s\n",SDL_GetError());
		return(1);
	}

	vxmax=300;
	vymax=300;

	win= SDL_CreateWindow("Inter_Polygon", 100, 100, vxmax, vymax, 
		SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
	if(win==NULL){
		fprintf(stderr,"SDL_CreateWindow Error: %s\n",SDL_GetError());
		SDL_Quit();
		return 1;
	}

	ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (ren == NULL){
		SDL_DestroyWindow(win);
		fprintf(stderr,"SDL_CreateRenderer Error: %s\n",SDL_GetError());
		SDL_Quit();
		return 1;
	}

	SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
	SDL_RenderClear(ren);
	SDL_SetRenderDrawColor(ren, 255, 0, 50, 255);
	SDL_RenderPresent(ren);

	printf("\nDisegno di una poligonale interattivamente: \n");
	printf("Dare i punti sulla finestra grafica con il mouse (button sinistro) \n");
	printf("Finire l'inserimento con il button destro \n");
	printf("Uscire dal programma con il tasto <ESC>  \n");

	while(esc)
	{
		if (!SDL_PollEvent(&event))
			continue;

		switch(event.type) {
			case SDL_MOUSEBUTTONUP:
			if(event.button.button==1 && clicked >= 0) {
				x[clicked] = event.button.x;
				y[clicked] = event.button.y;

				SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
				SDL_RenderClear(ren);
				SDL_SetRenderDrawColor(ren, 255, 0, 50, 255);
				redraw(n-1,x,y,ren);
				SDL_SetRenderDrawColor(ren, 0, 0, 255, 255);
				draw_Bezier_Curve(n, x, y, ren);
				SDL_RenderPresent(ren);
			}
			break;
			
			case SDL_MOUSEBUTTONDOWN:
			// Tasto SX
			if(event.button.button==1) {
				if (is_curve_drawn) {
					clicked = getClickedPoint(&event, x, y, n);
				} else {
					addControlPoint(ren, &event, x, y, &n);
				}
			}

			// Tasto DX
			if(event.button.button==3)
			{
				SDL_SetRenderDrawColor(ren, 0, 0, 255, 255);

				draw_Bezier_Curve(n, x, y, ren);
				is_curve_drawn=1;
			}
			SDL_RenderPresent(ren);
			break;	

			// ESC key
			case SDL_KEYDOWN:
			if(event.key.keysym.sym == SDLK_ESCAPE)
				esc=0;
			break;

			// Resize finestra
			case SDL_WINDOWEVENT:
			windowID = SDL_GetWindowID(win);
			if (event.window.windowID == windowID)  {
				switch (event.window.event)  {
					case SDL_WINDOWEVENT_SIZE_CHANGED:  {
						vxmax = event.window.data1;
						vymax = event.window.data2;

						SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
						SDL_RenderClear(ren);
						SDL_SetRenderDrawColor(ren, 255, 0, 50, 255);
						redraw(n-1,x,y,ren);

						SDL_RenderPresent(ren);
						break;
					}
				}
			}
			break;

		}
	}

	SDL_Quit();
	return(0);
}
