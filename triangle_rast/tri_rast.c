#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../GCGraLib2/GCGraLib2.h"
#include "triangle_rast.h"

#define DIM 100


float           S1, S2;         /* fattori di scala */
unsigned int    sidex,sidey;    /* dimensioni rettangolo pixel */
typedef float   rvector[10];
rvector         yymax , yymin , xa , dx;
int             edges , startedge , endedge , scandec;
float           scan;
int             nv;



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

/*semplice programma di prova di input da mouse e tastiera*/
int main(void)
{
	SDL_Window *win;
	SDL_Renderer *ren;
	SDL_Event event;
	int vxmax=500,vymax=500;
	int esc=1;
	//int x[DIM],y[DIM];
	//int flag=1;

	if(SDL_Init(SDL_INIT_VIDEO)<0)
	{
		fprintf(stderr,"Couldn't init video: %s\n",SDL_GetError());
		return(1);
	}

	win= SDL_CreateWindow("ScanConv", 0, 0, vxmax, vymax, SDL_WINDOW_RESIZABLE);
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

	printf("\nDisegno di una poligonale interattivamente: \n");
	printf("Dare i punti sulla finestra grafica con il mouse (button sinistro) \n");
	printf("Finire l'inserimento con il button destro \n");
	printf("Uscire dal programma con il tasto <ESC>  \n");
	//color
	//SDL_SetRenderDrawColor(ren,255,255,255,255);
	//  bcolor
	//SDL_SetRenderDrawColor(ren,0,0,0,255);

	//Set color
	SDL_SetRenderDrawColor(ren,0,0,0,255);
	//Clear back buffer
	SDL_RenderClear(ren);
	//Swap back and front buffer
	SDL_RenderPresent(ren);

	SDL_SetRenderDrawColor(ren,255,255,255,255);

	polx[0]=20; poly[0]=20;
	polx[1]=120; poly[1]=50;
	polx[2]=60; poly[2]=120;
	//polx[3]=20; poly[3]=20;
	while(esc)
	{
		if (SDL_PollEvent(&event))
			switch(event.type)
			{
				case SDL_MOUSEBUTTONDOWN:

					if(event.button.button==3)
					{
						// SDL_RenderDrawLine(ren, x[n-1], y[n-1], x[0], y[0]);

						scanconv(ren,3);
					}
					SDL_RenderPresent(ren);
					break;
				case SDL_KEYDOWN:
					if(event.key.keysym.sym == SDLK_ESCAPE)
						esc=0;
					break;

				case SDL_WINDOWEVENT:
					switch (event.window.event) {
						case SDL_WINDOWEVENT_RESIZED:
							vxmax = event.window.data1;
							vymax = event.window.data2;
							//redraw(n-1,x,y,ren);
							SDL_RenderPresent(ren);
					}
					break;
			}
	}

	SDL_DestroyRenderer(ren);
	SDL_DestroyWindow(win);
	SDL_Quit();
	return(0);
}
