#include <stdio.h>
#include <math.h>
#include "../GCGraLib2/GCGraLib2.h"

#define MAXVERT (100)
#define MAXEDGE (200)
#define MAXFACE (100)
#define DEFAULT_PTSIZE  12

#define FINEWIN     0
#define TETAWIN     1
#define FIWIN       2
#define ZIWIN       3
#define ZOWIN       4
#define DZIWIN      5
#define DZOWIN      6
#define RIGHTWIN    7
#define LEFTWIN     8
#define UPWIN       9
#define DOWNWIN     10

/* Dati per visualizzare un oggetto definito solo da vertici e lati*/
float   x[MAXVERT],y[MAXVERT],z[MAXVERT];       /* coordinate vertici */
int     edge[MAXEDGE][2];                       /* lista lati */
int     face[MAXFACE][4];                       /* lista facce */
float   nx[MAXFACE],ny[MAXFACE],nz[MAXFACE];    /* coordinate vertici */
int     tx[MAXFACE][4],ty[MAXFACE][4];          /* coordinate texture */
int     nvert,nedge,nface;                      /* numero vertici, lati e facce */
float   csx,csy,csz;                            /* centro oggetto */
float   D = 25,teta = 20,fi = 20,di;
int     xs[MAXVERT],ys[MAXVERT];                /* coordinate schermo */
float   zeye[MAXVERT];                          /* coordinate z trasformate */

int 	xvmin,xvmax,yvmin,yvmax;	/* coordinate viewport */
float   salpha=0.05;			/* zoomin e zoomout */
float   sr=0.25,ssx,ssy,ssz,ssmod,      /* modifica centro oggetto */
Dstep=1.0,			/* passo di incremento D */
alpha,				/* apertura angolare piramide di vista */
s;
float 	xwmin,xwmax,ywmin,ywmax; 	/* coordinate window */
float 	Sx, Sy;				/* fattori di scala trasf. window-viewport */

float 	tmpx,tmpy;			/* variabili temporanee */
int 	i,j;
int     wim,him;

typedef struct RECT RECT;
struct RECT
{
	SDL_Rect rect;
	char     text[10];
};

RECT  menu[15];
int   done=0;

void set_menu_opt(RECT *menu, int x, int y, int w, int h, char *text) {
    menu->rect.x = x;
    menu->rect.y = y;
    menu->rect.w = w;
    menu->rect.h = h;
    strcpy(menu->text, text);
}

/* inizializzazione menu */
void init_menu(SDL_Renderer *ren, RECT menu[],float teta, float fi)
{
	set_menu_opt(&menu[FINEWIN], 605, 550, 100, 30, "QUIT");
	set_menu_opt(&menu[TETAWIN], 605, 20, 100, 100, "THETA");
	set_menu_opt(&menu[FIWIN], 605, 150, 100, 100, "PHI");
	set_menu_opt(&menu[ZIWIN], 602, 280, 50, 30, "ZoomI");
	set_menu_opt(&menu[ZOWIN], 662, 280, 50, 30, "ZoomO");
	set_menu_opt(&menu[DZIWIN], 602, 320, 50, 30, "DZoomI");
	set_menu_opt(&menu[DZOWIN], 662, 320, 50, 30, "DZoomO");
	set_menu_opt(&menu[RIGHTWIN], 662, 400, 50, 30, "RIGHT");
	set_menu_opt(&menu[LEFTWIN], 602, 400, 50, 30, "LEFT");
	set_menu_opt(&menu[UPWIN], 631, 360, 50, 30, "UP");
	set_menu_opt(&menu[DOWNWIN], 631, 440, 50, 30, "DOWN");
	SDL_RenderDrawLine(ren, menu[1].rect.x+menu[1].rect.w/2, menu[1].rect.y+menu[1].rect.h/2,
		(int)(menu[1].rect.w/2)*cos(teta)+menu[1].rect.x+menu[1].rect.w/2,
		(int)(menu[1].rect.h/2)*sin(teta)+menu[1].rect.y+menu[1].rect.h/2);
	SDL_RenderDrawLine(ren, menu[2].rect.x+menu[2].rect.w/2, menu[2].rect.y+menu[2].rect.h/2,
		(int)(menu[2].rect.w/2)*cos(fi)+menu[2].rect.x+menu[2].rect.w/2,
		(int)(menu[2].rect.h/2)*sin(fi)+menu[2].rect.y+menu[2].rect.h/2);
}

void draw_menu(RECT menu[], SDL_Renderer *ren, TTF_Font *font)
{
	int i;

	for( i=0 ; i<=10 ; i++ )
	{
		GC_DrawText(ren, font, 0, 0, 0, 0, 255, 255, 255, 0, menu[i].text,
			menu[i].rect.x, menu[i].rect.y, shaded);
		//GC_DrawRect(ren,menu[i].rect.x,menu[i].rect.y,menu[i].rect.w,menu[i].rect.h);
		SDL_RenderDrawRect(ren,&(menu[i].rect));
	}
}

/* funzione di determinazione opzione del menu' scelta */
void opt_menu(RECT menu[],int n,int ix,int iy,int *choice)
{
	int i;

	*choice=-1;
	for(i=0; i<=n; i++)
	{
		if (ix>=menu[i].rect.x && iy>=menu[i].rect.y &&
			ix-menu[i].rect.x<=menu[i].rect.w && iy-menu[i].rect.y<=menu[i].rect.h)
			*choice=i;
	}
}

float rad(float angle)
{
	return(angle/180*3.1415);
}

void copy_trasl(int nv,int *nvert,float x[],float y[],float z[],
	int ne,int *nedge,int edge[][2],float dx,float dy,float dz)
{
	int i;

	for (i=0; i<nv; i++)
	{
		x[i+*nvert]=x[i]+dx;
		y[i+*nvert]=y[i]+dy;
		z[i+*nvert]=z[i]+dz;
	}
	for (i=0; i<=ne; i++)
	{
		edge[i+*nedge][0]=edge[i][0]+*nvert;
		edge[i+*nedge][1]=edge[i][1]+*nvert;
	}
	*nvert+=nv;
	*nedge+=ne;
}

/* Calcola la normale ad una faccia */
void compute_face_normal(int k)
{
	int i1,i2,i3;
	float v[3],w[3];

	i1=face[k][0]; i2=face[k][1]; i3=face[k][2];
	v[0]=x[i2]-x[i1]; v[1]=y[i2]-y[i1]; v[2]=z[i2]-z[i1];
	w[0]=x[i3]-x[i2]; w[1]=y[i3]-y[i2]; w[2]=z[i3]-z[i2];
	nx[k]=v[1]*w[2]-v[2]*w[1];
	ny[k]=v[2]*w[0]-v[0]*w[2];
	nz[k]=v[0]*w[1]-v[1]*w[0];
	//  printf("%d %f %f %f \n",k,nx[k],ny[k],nz[k]);
}

/* funzione che definisce l'oggetto mesh */
void define_object()
{
	int k;

	/* Determino le coordinate dei vertici dell'oggetto */
	nvert=8;
	x[0]=-1.0; y[0]=-1.0; z[0]=-1.0;
	x[1]=1.0; y[1]=-1.0; z[1]=-1.0;
	x[2]=1.0;y[2]=1.0;z[2]=-1.0;
	x[3]=-1.0;y[3]=1.0;z[3]=-1.0;
	x[4]=-1.0;y[4]=-1.0;z[4]=1.0;
	x[5]=1.0;y[5]=-1.0;z[5]=1.0;
	x[6]=1.0;y[6]=1.0;z[6]=1.0;
	x[7]=-1.0;y[7]=1.0;z[7]=1.0;

	/* Determino la lista dei lati dell'oggetto */
	nedge=13;
	edge[0][0]=0;edge[0][1]=1;
	edge[1][0]=1;edge[1][1]=2;
	edge[2][0]=2;edge[2][1]=3;
	edge[3][0]=3;edge[3][1]=0;
	edge[4][0]=4;edge[4][1]=5;
	edge[5][0]=5;edge[5][1]=6;
	edge[6][0]=6;edge[6][1]=7;
	edge[7][0]=7;edge[7][1]=4;
	edge[8][0]=0;edge[8][1]=4;
	edge[9][0]=1;edge[9][1]=5;
	edge[10][0]=2;edge[10][1]=6;
	edge[11][0]=3;edge[11][1]=7;
	/*diagonale faccia*/
	edge[12][0]=1;edge[12][1]=6;

	/*Determino la lista delle facce dell'oggetto */
	nface=6;
	face[0][0]=0; face[0][1]=1; face[0][2]=5; face[0][3]=4;
	face[1][0]=1; face[1][1]=2; face[1][2]=6; face[1][3]=5;
	face[2][0]=2; face[2][1]=3; face[2][2]=7; face[2][3]=6;
	face[3][0]=3; face[3][1]=0; face[3][2]=4; face[3][3]=7;
	face[4][0]=4; face[4][1]=5; face[4][2]=6; face[4][3]=7;
	face[5][0]=3; face[5][1]=2; face[5][2]=1; face[5][3]=0;

	/*Determino la normale alle facce */
	for (k=0; k<nface; k++)
		compute_face_normal(k);

	/*Definisco coordinate texture dell'immagine */
	tx[0][0]=wim; tx[0][1]=0; tx[0][2]=0; tx[0][3]=wim;
	ty[0][0]=0; ty[0][1]=0; ty[0][2]=him; ty[0][3]=him;
	/**/
	tx[1][0]=wim; tx[1][1]=0; tx[1][2]=0; tx[1][3]=wim;
	ty[1][0]=0; ty[1][1]=0; ty[1][2]=him; ty[1][3]=him;
	tx[2][0]=wim; tx[2][1]=0; tx[2][2]=0; tx[2][3]=wim;
	ty[2][0]=0; ty[2][1]=0; ty[2][2]=him; ty[2][3]=him;
	tx[3][0]=wim; tx[3][1]=0; tx[3][2]=0; tx[3][3]=wim;
	ty[3][0]=0; ty[3][1]=0; ty[3][2]=him; ty[3][3]=him;
	tx[4][0]=wim; tx[4][1]=0; tx[4][2]=0; tx[4][3]=wim;
	ty[4][0]=0; ty[4][1]=0; ty[4][2]=him; ty[4][3]=him;
	tx[5][0]=wim; tx[5][1]=0; tx[5][2]=0; tx[5][3]=wim;
	ty[5][0]=0; ty[5][1]=0; ty[5][2]=him; ty[5][3]=him;
	/**/

	//copy_trasl(8,&nvert,x,y,z,13,&nedge,edge,2.0,0.0,0.0);
}

/* trasformazione prospettica a tre punti di fuga (generico punto di vista) */
void trasf_prosp_gen(int *init,float x, float y, float z,
	float *xe, float *ye, float *ze)
{
	static float steta,cteta,cfi,sfi;

	if (*init)
	{
		*init=0;
		steta=sin(teta);
		cteta=cos(teta);
		sfi=sin(fi);
		cfi=cos(fi);
	}

	/* trasformazione in coordinate del sistema osservatore */
	*xe = -steta*x + y*cteta;
	*ye = -cteta*cfi*x - y*steta*cfi + z*sfi;
	*ze = -x*cteta*sfi - y *steta*sfi - z*cfi + D;
}

/* funzione che determina i fattori di scala */
void define_view()
{
	int     k;
	float   s,r;
	float   xmin,xmax,ymin,ymax,zmin,zmax;

	xmin = x[0];
	ymin = y[0];
	zmin = z[0];
	xmax = xmin;
	ymax = ymin;
	zmax = zmin;
	for (k=1;k<nvert;k++)
	{
		if (x[k] > xmax) xmax = x[k];
		else if (x[k]<xmin) xmin = x[k];
		if (y[k] > ymax) ymax = y[k];
		else if (y[k]<ymin) ymin = y[k];
		if (z[k] > zmax) zmax = z[k];
		else if (z[k]<zmin) zmin = z[k];
	}
	/* centro oggetto iniziale */
	csx = (xmin + xmax)/2;
	csy = (ymin + ymax)/2;
	csz = (zmin + zmax)/2;
	/* raggio sfera contenete oggetto */
	r = sqrt(powf(xmax-csx,2)+powf(ymax-csy,2)+powf(zmax-csz,2));
	/* calcolo semilato window iniziale; il piano di proiezione viene assunto
	ad una distanza D dall'osservatore */
	s = r*D/sqrt(powf(D,2)-powf(r,2));
	/* in base al semilato window cosi' calcolato si determina l'apertura
	angolare iniziale */
	alpha=atan(s/D);
	/* printf("\nApertura angolare: %f \n",alpha);*/

}

void conv(unsigned long m, int b, int a[])
{
	int p;

	a[0]=a[1]=a[2]=0;
	p=-1;
	while (m != 0)
	{
		p++;
		a[p]= m % b;
		m= m / b;
	}
}

int max(int a,int b,int c)
{
	int m;
	m=a;
	if (b>m) m=b;
	if (c>m) m=c;
	return m;
}
int min(int a,int b,int c)
{
	int m;
	m=a;
	if (b<m) m=b;
	if (c<m) m=c;
	return m;
}

float f01(float x0,float y0,float x1,float y1,float x,float y)
{
	return (y0-y1)*x+(x1-x0)*y+x0*y1-x1*y0;
}

void draw_texture(SDL_Renderer *ren, SDL_Surface *tux,int kf,int i1,int i2,int i3,int i4)
{
	int x0,y0,x1,y1,x2,y2;
	float z0,z1,z2,fz;
	int xmin,xmax,ymin,ymax;
	int ix,iy;
	float alpha, beta, gamma;
	int k;
	int xim0,yim0,xim1,yim1,xim2,yim2;
	float fx,fy,aa,bb;
	int i,j;
	int pix[3],pix1[3],pix2[3],pix3[3],pix4[3];

	/*ogni faccia quadrata viene divisa in due triangoli */
	for (k=0; k<2; k++)
	{
		if (k==0)
		{
			/*triangolo immagine con cui texturare il triangolo schermo */
			xim0=tx[kf][3]; yim0=ty[kf][3];
			xim1=tx[kf][2]; yim1=ty[kf][2];
			xim2=tx[kf][0]; yim2=ty[kf][0];
			/*triangolo schermo*/
			x0=xs[i4];y0=ys[i4];z0=zeye[i4];
			x1=xs[i3];y1=ys[i3];z1=zeye[i3];
			x2=xs[i1];y2=ys[i1];z2=zeye[i1];
		}
		else
		{
			/*triangolo immagine con cui texturare il triangolo schermo */
			xim0=tx[kf][2]; yim0=ty[kf][2];
			xim1=tx[kf][1]; yim1=ty[kf][1];
			xim2=tx[kf][0]; yim2=ty[kf][0];
			/*triangolo schermo*/
			x0=xs[i3];y0=ys[i3];z0=zeye[i3];
			x1=xs[i2];y1=ys[i2];z1=zeye[i2];
			x2=xs[i1];y2=ys[i1];z2=zeye[i1];
		}
		/*extent triangolo schermo*/
		xmin=min(x0,x1,x2);
		xmax=max(x0,x1,x2);
		ymin=min(y0,y1,y2);
		ymax=max(y0,y1,y2);
		for (ix=xmin; ix<=xmax; ix++)
			for (iy=ymin; iy<=ymax; iy++)
			{
				alpha=f01(x1,y1,x2,y2,ix,iy)/f01(x1,y1,x2,y2,x0,y0);
				beta=f01(x2,y2,x0,y0,ix,iy)/f01(x2,y2,x0,y0,x1,y1);
				gamma=f01(x0,y0,x1,y1,ix,iy)/f01(x0,y0,x1,y1,x2,y2);
				if (alpha>=0 && beta>=0 && gamma>=0)
				{
					/*corregge il parametro per il triangolo 3D con il quale si puo' risalire
					all'immagine; se commentiamo queste 4 linee si avra' un problema sull'
					immagine proiettata */
					/**/
					fz=alpha/z0+beta/z1+gamma/z2;
					alpha=alpha/z0/fz;
					beta=beta/z1/fz;
					gamma=gamma/z2/fz;
					/**/
					fx=alpha*xim0+beta*xim1+gamma*xim2;
					fy=alpha*yim0+beta*yim1+gamma*yim2;
					j=(int)fx;
					i=(int)fy;
					aa=fx-j;
					bb=fy-i;
					conv(GC_GetPixelImage(tux,j,i),256,pix1);
					conv(GC_GetPixelImage(tux,j+1,i),256,pix2);
					conv(GC_GetPixelImage(tux,j,i+1),256,pix3);
					conv(GC_GetPixelImage(tux,j+1,i+1),256,pix4);

					pix[0]=(int)((1-aa)*(1-bb)*pix1[0]+aa*(1-bb)*pix2[0]+bb*(1-aa)*pix3[0]+aa*bb*pix4[0]);
					pix[1]=(int)((1-aa)*(1-bb)*pix1[1]+aa*(1-bb)*pix2[1]+bb*(1-aa)*pix3[1]+aa*bb*pix4[1]);
					pix[2]=(int)((1-aa)*(1-bb)*pix1[2]+aa*(1-bb)*pix2[2]+bb*(1-aa)*pix3[2]+aa*bb*pix4[2]);
					//               ic=pix[2]*256*256+pix[1]*256+pix[0];
					//                     conv(XGetPixel(ImageOrig,ix,iy),256,pix);
					//                     ic=pix[2]*256*256+pix[1]*256+pix[0];
					SDL_SetRenderDrawColor(ren,pix[2],pix[1],pix[0],255);
					SDL_RenderDrawPoint(ren, ix, iy);
				}
			}
	}
}

void draw_mesh(SDL_Renderer *ren, SDL_Surface *tux)
{
	int t,u,k,init;
	float xe,ye,ze;    /* coord. dell'osservatore */
	int i;
	float vd[3],vp[3];

	/* si ricalcola il semilato della window in base a D e ad alpha */
	s=D*tan(alpha);
	xwmin = -s ;
	xwmax = s;
	ywmin = -s;
	ywmax = s;

	/* fattori di scala per trasf. Window-Viewport */
	Sx = ((float)(xvmax) - (float)(xvmin))/(xwmax - xwmin);
	Sy = ((float)(yvmax) -(float)(yvmin))/(ywmax - ywmin);

	/* il piano di proiezione viene aggiornato a passare per l'origine */
	di=D;
	init=1;
	for (k=0;k<nvert;k++)
	{
		trasf_prosp_gen(&init,x[k]-csx,y[k]-csy,z[k]-csz,&xe,&ye,&ze);
		/* proiezione e trasformazione in coordinate schermo */
		xs[k] = (int)(Sx * ((di * xe)/ze - xwmin) + xvmin + 0.5);
		ys[k] = (int)(Sy * (ywmin - (di * ye)/ze) + yvmax + 0.5);
		zeye[k] = ze;
	}

	/*direzione di vista per poter applicare il back face culling */
	vp[0]=D*sin(fi)*cos(teta);
	vp[1]=D*sin(fi)*sin(teta);
	vp[2]=D*cos(fi);

	/* disegno facce visibili */
	for (k = 0;k < nface ;k++)
	{
		t=face[k][0];
		vd[0]=x[t]-vp[0];
		vd[1]=y[t]-vp[1];
		vd[2]=z[t]-vp[2];
		if (nx[k]*vd[0]+ny[k]*vd[1]+nz[k]*vd[2] < 0)
			draw_texture(ren,tux,k,face[k][0],face[k][1],face[k][2],face[k][3]);
	}

	SDL_SetRenderDrawColor(ren,0,0,0,255);
	/* disegno lati di ogni faccia visibile */
	for (k = 0;k < nface ;k++)
	{
		t=face[k][0];
		vd[0]=x[t]-vp[0];
		vd[1]=y[t]-vp[1];
		vd[2]=z[t]-vp[2];
		if (nx[k]*vd[0]+ny[k]*vd[1]+nz[k]*vd[2] < 0)
		{
			t=face[k][3];
			u=face[k][0];
			for (i=0; i<4; i++)
			{
				SDL_RenderDrawLine(ren, xs[t],ys[t],xs[u],ys[u]);
				t=face[k][i];
				u=face[k][i+1];
			}
		}
	}
	/*printf("\n [D,teta,fi]= %f %f %f",D,teta,fi);*/
	SDL_RenderPresent(ren);
}

void redraw_scene(SDL_Renderer *ren, SDL_Rect *sub_v, SDL_Surface *tux) {
	SDL_SetRenderDrawColor(ren,255,255,255,255);
	SDL_RenderFillRect(ren, sub_v);
	draw_mesh(ren,tux);
}

int main()
{
	SDL_Window *win;
	SDL_Renderer *ren;
	SDL_Surface *tux;
	SDL_Rect sub_v, v, bsub_v;
	TTF_Font *font;
	SDL_Event myevent;
	int res = 0;
	int choice, done=0;

	/* load an image by file */
	//  tux = GC_LoadImage("grid.ppm",&res);
	tux = GC_LoadImage("ganesha.ppm",&res);
	if (res == 0)
	return 1;

	if (SDL_Init (SDL_INIT_VIDEO) < 0)
	{
		fprintf (stderr, "Couldn't init video: %s\n", SDL_GetError ());
		return (1);
	}

	/* Initialize the TTF library */
	if (TTF_Init () < 0)
	{
		fprintf (stderr, "Couldn't initialize TTF: %s\n", SDL_GetError ());
		SDL_Quit ();
		return (2);
	}

	font = TTF_OpenFont ("FreeSans.ttf", DEFAULT_PTSIZE);
	if (font == NULL)
	{
		fprintf (stderr, "Couldn't load font\n");
	}

	v.x = 0;
	v.y = 0;
	v.w = 720;
	v.h = 600;
	xvmin = v.x+5;
	yvmin = v.y+5;
	xvmax = v.w-130;
	yvmax = v.h-10;
	sub_v.x = xvmin;
	sub_v.y = yvmin;
	sub_v.w = xvmax;
	sub_v.h = yvmax;
	bsub_v.x=sub_v.x-2;
	bsub_v.y=sub_v.y-2;
	bsub_v.w=sub_v.w+4;
	bsub_v.h=sub_v.h+4;
	wim=tux->w;
	him=tux->h;

	win= SDL_CreateWindow("View Cube Model", 0, 0, v.w, v.h, SDL_WINDOW_RESIZABLE);
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

	//Set color
	SDL_SetRenderDrawColor(ren,255,255,255,255);
	//Clear back buffer
	SDL_RenderClear(ren);
	//Swap back and front buffer
	SDL_RenderPresent(ren);

	SDL_SetRenderDrawColor(ren,0,0,0,255);
	//GC_DrawRect(ren,sub_v.x-2,sub_v.y-2,sub_v.w+4,sub_v.h+4);
	SDL_RenderDrawRect(ren,&bsub_v);

	init_menu(ren,menu,teta,fi);
	draw_menu(menu,ren,font);
	define_object();   /* determinazione mesh oggetto */
	define_view();
	draw_mesh(ren,tux);

	//Disegnare tutte le volte, anche per eventi intermedi,
	//rallenta tutto senza dare un contributo all'immagine finale.
	//Gli eventi si possono scremare in questo modo:
	//viene usata la costante 0xff perchè nel codice sorgente delle SDL è usato
	//come costante all'interno delle funzioni che accettano interi 8 bit come input
	//per rappresentare SDL_ALLEVENTS (che invece e' a 32 bit)

	while (done == 0)
	{
		if (SDL_PollEvent(&myevent))
		{
			//  SDL_EventState(0xff, SDL_IGNORE); //0xff is all events
			SDL_EventState(SDL_MOUSEMOTION, SDL_IGNORE);
		switch(myevent.type) {
			case SDL_MOUSEMOTION:
				if(myevent.motion.state == SDL_PRESSED)
				{
					opt_menu(menu,11,myevent.motion.x,myevent.motion.y,&choice);
					switch(choice) {
					case TETAWIN:
						tmpx = myevent.motion.x-(menu[1].rect.x+menu[1].rect.w/2);
						tmpy = myevent.motion.y-(menu[1].rect.y+menu[1].rect.h/2);
						teta = atan2(tmpy,tmpx);
						SDL_SetRenderDrawColor(ren,255,255,255,255);
						SDL_RenderFillRect(ren,&(menu[1].rect));
						SDL_SetRenderDrawColor(ren,0,0,0,255);
						GC_DrawText(ren, font, 0, 0, 0, 0, 255, 255, 255, 0, menu[1].text,
							menu[1].rect.x, menu[1].rect.y, shaded);
						//GC_DrawRect(ren,menu[1].rect.x,menu[1].rect.y,menu[1].rect.w,menu[1].rect.h);
						SDL_RenderDrawRect(ren,&(menu[1].rect));
						SDL_RenderDrawLine(ren, myevent.motion.x,myevent.motion.y,
							menu[1].rect.x+menu[1].rect.w/2,menu[1].rect.y+menu[1].rect.h/2);
						draw_menu(menu,ren,font);

						redraw_scene(ren, &sub_v, tux);
						break;
					case FIWIN:
						tmpx = myevent.motion.x-(menu[2].rect.x+menu[2].rect.w/2);
						tmpy = myevent.motion.y-(menu[2].rect.y+menu[2].rect.h/2);
						SDL_SetRenderDrawColor(ren,255,255,255,255);
						SDL_RenderFillRect(ren,&(menu[2].rect));
						SDL_SetRenderDrawColor(ren,0,0,0,255);
						GC_DrawText(ren, font, 0, 0, 0, 0, 255, 255, 255, 0, menu[2].text,
							menu[2].rect.x, menu[2].rect.y, shaded);
						//GC_DrawRect(ren,menu[2].rect.x,menu[2].rect.y,menu[2].rect.w,menu[2].rect.h);
						SDL_RenderDrawRect(ren,&(menu[2].rect));
						SDL_RenderDrawLine (ren, myevent.motion.x,myevent.motion.y,
							menu[2].rect.x+menu[2].rect.w/2,menu[2].rect.y+menu[2].rect.h/2);
						draw_menu(menu,ren,font);
						fi = atan2(tmpy,tmpx);

						redraw_scene(ren, &sub_v, tux);
						break;
					}
				}
				break;

			case SDL_MOUSEBUTTONDOWN:
				if(myevent.button.button == SDL_PRESSED)
				{
					opt_menu(menu,11,myevent.button.x,myevent.button.y,&choice);
					switch(choice) {
					case FINEWIN:
						done = 1;
						break;
					case RIGHTWIN:
						ssx=-sin(teta);
						ssy=cos(teta);
						csx-=sr*ssx;
						csy-=sr*ssy;
						redraw_scene(ren, &sub_v, tux);
						break;
					case LEFTWIN:
						ssx=-sin(teta);
						ssy=cos(teta);
						csx+=sr*ssx;
						csy+=sr*ssy;
						redraw_scene(ren, &sub_v, tux);
						break;
					case UPWIN:
						ssx=-cos(fi)*cos(teta);
						ssy=-cos(fi)*sin(teta);
						ssz=sin(fi);
						csx+=sr*ssx;
						csy+=sr*ssy;
						csz+=sr*ssz;
						redraw_scene(ren, &sub_v, tux);
						break;
					case DOWNWIN:
						ssx=-cos(fi)*cos(teta);
						ssy=-cos(fi)*sin(teta);
						ssz=sin(fi);
						csx-=sr*ssx;
						csy-=sr*ssy;
						csz-=sr*ssz;
						redraw_scene(ren, &sub_v, tux);
						break;
					case ZIWIN:
						if (alpha-salpha >0)
							alpha-=salpha;
						redraw_scene(ren, &sub_v, tux);
						break;
					case ZOWIN:
						if (alpha+salpha <1.57)
							alpha+=salpha;
						redraw_scene(ren, &sub_v, tux);
						break;
					case DZIWIN:
						if (D-Dstep>0)
							D-=Dstep;
						redraw_scene(ren, &sub_v, tux);
						break;
					case DZOWIN:
						D+=Dstep;
						redraw_scene(ren, &sub_v, tux);
						break;
					}
				}
				break;
			case SDL_KEYDOWN:
				if(myevent.key.keysym.sym == SDLK_ESCAPE)
				done = 1;
				break;
			}
			//   SDL_EventState(0xff, SDL_ENABLE);
			SDL_EventState(SDL_MOUSEMOTION, SDL_ENABLE);
		}
	}
	TTF_CloseFont (font);
	TTF_Quit ();
	SDL_DestroyRenderer(ren);
	SDL_DestroyWindow(win);
	SDL_Quit ();
	return (0);
}
