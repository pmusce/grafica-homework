#include <stdio.h>
#include <math.h>
#include "../GCGraLib2/GCGraLib2.h"

#define MAXVERT (100)
#define MAXEDGE (200)
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
int     nvert,nedge;                            /* numero vertici e lati */
float   csx,csy,csz;                            /* centro oggetto */
float   D = 25,teta = 20,fi = 20,di;
int     xs[MAXVERT],ys[MAXVERT];                /* coordinate schermo */

int 	xvmin,xvmax,yvmin,yvmax;	/* coordinate viewport */
float   salpha=0.05;			/* zoomin e zoomout */
float   sr=0.25,ssx,ssy,ssz,ssmod,      /* modifica centro oggetto */
        Dstep=2.5,			/* passo di incremento D */
        alpha,				/* apertura angolare piramide di vista */
        s;
float 	xwmin,xwmax,ywmin,ywmax; 	/* coordinate window */
float 	Sx, Sy;				/* fattori di scala trasf. window-viewport */

float 	tmpx,tmpy;			/* variabili temporanee */
int 	i,j;

typedef struct RECT RECT;
struct RECT
{
    SDL_Rect rect;
    char     text[10];
};

RECT  menu[15];
int   done=0;

int isInViewport(SDL_Rect *viewport, int ix,int iy) {
    int maxx, maxy;
    maxx = viewport->x + viewport->w;
    maxy = viewport->y + viewport->h;
    return ix > viewport->x && ix < maxx && ix > viewport->y && ix < maxy;
}

void set_menu_opt(RECT *menu, int x, int y, int w, int h, char *text) {
    menu->rect.x = x;
    menu->rect.y = y;
    menu->rect.w = w;
    menu->rect.h = h;
    strcpy(menu->text, text);
}

void init_menu(SDL_Renderer *ren, RECT menu[], float teta, float fi)
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

    // draw teta and fi controller
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
        SDL_RenderDrawRect(ren,&(menu[i].rect));
    }
}

/* funzione di determinazione opzione del menu' scelta */
void clicked_opt_menu(RECT menu[],int n,int ix,int iy,int *choice)
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

int add_vertex(float x_i, float y_i, float z_i) {
    x[nvert]= x_i;
    y[nvert]= y_i;
    z[nvert]= z_i;;
    return nvert++;
}

void add_edge(int vert_a, int vert_b) {
    edge[nedge][0] = vert_a;
    edge[nedge][1] = vert_b;
    nedge++;
}

/* funzione che definisce l'oggetto mesh */
void define_cube()
{
    int cube_verts[8];

    cube_verts[0] = add_vertex(-1.0, -1.0, -1.0);
    cube_verts[1] = add_vertex(1.0, -1.0, -1.0);
    cube_verts[2] = add_vertex(1.0, 1.0, -1.0);
    cube_verts[3] = add_vertex(-1.0, 1.0, -1.0);
    cube_verts[4] = add_vertex(-1.0, -1.0, 1.0);
    cube_verts[5] = add_vertex(1.0, -1.0, 1.0);
    cube_verts[6] = add_vertex(1.0, 1.0, 1.0);
    cube_verts[7] = add_vertex(-1.0, 1.0, 1.0);

    add_edge(cube_verts[0], cube_verts[1]);
    add_edge(cube_verts[1], cube_verts[2]);
    add_edge(cube_verts[2], cube_verts[3]);
    add_edge(cube_verts[3], cube_verts[0]);

    add_edge(cube_verts[4], cube_verts[5]);
    add_edge(cube_verts[5], cube_verts[6]);
    add_edge(cube_verts[6], cube_verts[7]);
    add_edge(cube_verts[7], cube_verts[4]);
    
    add_edge(cube_verts[0], cube_verts[4]);
    add_edge(cube_verts[1], cube_verts[5]);
    add_edge(cube_verts[2], cube_verts[6]);
    add_edge(cube_verts[3], cube_verts[7]);
    
    add_edge(cube_verts[1], cube_verts[6]);
}

void define_pyramid() {
    int base_verts[4];
    int top_vert;

    base_verts[0] = add_vertex(2.0, -1.0, -1.0);
    base_verts[1] = add_vertex(4.0, -1.0, -1.0);
    base_verts[2] = add_vertex(4.0, 1.0, -1.0);
    base_verts[3] = add_vertex(2.0, 1.0, -1.0);
    top_vert = add_vertex(3.0, 0.0, 1.0);

    add_edge(base_verts[0], base_verts[1]);
    add_edge(base_verts[1], base_verts[2]);
    add_edge(base_verts[2], base_verts[3]);
    add_edge(base_verts[3], base_verts[0]);

    add_edge(base_verts[0], top_vert);
    add_edge(base_verts[1], top_vert);
    add_edge(base_verts[2], top_vert);
    add_edge(base_verts[3], top_vert);
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
    float   r;
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
    /* raggio sfera contenente oggetto */
    r = sqrt(powf(xmax-csx,2)+powf(ymax-csy,2)+powf(zmax-csz,2));
    /* calcolo semilato window iniziale; il piano di proiezione viene assunto
    ad una distanza D dall'osservatore */
    s = r*D/sqrt(powf(D,2)-powf(r,2));
    /* in base al semilato window cosi' calcolato si determina l'apertura
    angolare iniziale */
    alpha=atan(s/D);
}

void draw_mesh(SDL_Renderer *ren)
{
    int t,u,k,init;
    float xe,ye,ze;    /* coord. dell'osservatore */

    /* si ricalcola il semilato della window in base a D e ad alpha */
    s=D*tan(alpha);
    xwmin = -s ;
    xwmax = s;
    ywmin = -s;
    ywmax = s;

    /* fattori di scala per trasf. Window-Viewport */
    Sx = ((float)(xvmax) - (float)(xvmin))/(xwmax - xwmin);
    Sy = ((float)(yvmax) -(float)(yvmin))/(ywmax - ywmin);

    /* il piano di proiezione viene definito a passare per l'origine */
    di=D;
    init=1;
    for (k=0;k<nvert;k++)
    {
        trasf_prosp_gen(&init,x[k]-csx,y[k]-csy,z[k]-csz,&xe,&ye,&ze);
        /* proiezione e trasformazione in coordinate schermo */
        xs[k] = (int)(Sx * ((di * xe)/ze - xwmin) + xvmin + 0.5);
        ys[k] = (int)(Sy * (ywmin - (di * ye)/ze) + yvmax + 0.5);
    }

    /* disegno mesh */
    for (k = 0;k<nedge;k++)
    {
        t=edge[k][0];
        u=edge[k][1];
        SDL_RenderDrawLine(ren, xs[t],ys[t],xs[u],ys[u]);
    }

    SDL_RenderPresent(ren);
}				

void refresh_scene(SDL_Renderer *ren, SDL_Rect *sub_v) {
    SDL_SetRenderDrawColor(ren,255,255,255,255);
    SDL_RenderFillRect(ren, sub_v);
    SDL_SetRenderDrawColor(ren,0,0,0,255);
    draw_mesh(ren);
}

int main()
{
    SDL_Window *win;
    SDL_Renderer *ren;
    SDL_Rect sub_v, v, bsub_v;
    TTF_Font *font;
    SDL_Event myevent;
    int choice, done=0;

    if (SDL_Init (SDL_INIT_VIDEO) < 0) {
        fprintf (stderr, "Couldn't init video: %s\n", SDL_GetError ());
        return (1);
    }

    /* Initialize the TTF library */
    if (TTF_Init () < 0) {
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
    SDL_RenderDrawRect(ren,&bsub_v);

    init_menu(ren,menu,teta,fi);
    draw_menu(menu,ren,font);
    nvert = nedge = 0;
    define_cube();   /* determinazione mesh oggetto */
    define_pyramid();
    define_view();  /*calcolo dei parametri di vista */
    draw_mesh(ren);

    //Disegnare tutte le volte, anche per eventi intermedi,
    //rallenta tutto senza dare un contributo all'immagine finale.
    //Gli eventi si possono scremare in questo modo:
    //viene usata la costante 0xff perchè nel codice sorgente delle SDL è usato
    //come costante all'interno delle funzioni che accettano interi 8 bit come input
    //per rappresentare SDL_ALLEVENTS (che invece e' a 32 bit)

    while (done == 0)
    {
        if (!SDL_PollEvent(&myevent))
            continue;

        SDL_EventState(0xff, SDL_IGNORE); //0xff is all events
        switch(myevent.type) {
        case SDL_MOUSEMOTION:
            if(myevent.motion.state != SDL_PRESSED)
                break;

            if(isInViewport(&sub_v, myevent.motion.x, myevent.motion.y)) {
                teta -= 0.01 * myevent.motion.xrel;
                fi -= 0.01 * myevent.motion.yrel;
                refresh_scene(ren, &sub_v);
                break;
            }

            clicked_opt_menu(menu,11,myevent.motion.x,myevent.motion.y,&choice);
            switch(choice) {
            case TETAWIN:
                tmpx = myevent.motion.x-(menu[1].rect.x+menu[1].rect.w/2); // ???
                tmpy = myevent.motion.y-(menu[1].rect.y+menu[1].rect.h/2); // ???
                teta = atan2(tmpy,tmpx);
                // clear teta rect
                SDL_SetRenderDrawColor(ren,255,255,255,255);
                SDL_RenderFillRect(ren,&(menu[1].rect));
                SDL_SetRenderDrawColor(ren,0,0,0,255);
                GC_DrawText(ren, font, 0, 0, 0, 0, 255, 255, 255, 0, menu[1].text,
                    menu[1].rect.x, menu[1].rect.y, shaded);
                SDL_RenderDrawRect(ren,&(menu[1].rect));
                SDL_RenderDrawLine(ren, myevent.motion.x,myevent.motion.y,
                    menu[1].rect.x+menu[1].rect.w/2,menu[1].rect.y+menu[1].rect.h/2);

                refresh_scene(ren, &sub_v);
                break;

            case FIWIN:  
                tmpx = myevent.motion.x-(menu[2].rect.x+menu[2].rect.w/2);
                tmpy = myevent.motion.y-(menu[2].rect.y+menu[2].rect.h/2);
                SDL_SetRenderDrawColor(ren,255,255,255,255);
                SDL_RenderFillRect(ren,&(menu[2].rect));
                SDL_SetRenderDrawColor(ren,0,0,0,255);
                GC_DrawText(ren, font, 0, 0, 0, 0, 255, 255, 255, 0, menu[2].text,
                    menu[2].rect.x, menu[2].rect.y, shaded);
                SDL_RenderDrawRect(ren,&(menu[2].rect));
                SDL_RenderDrawLine (ren, myevent.motion.x,myevent.motion.y,
                    menu[2].rect.x+menu[2].rect.w/2,menu[2].rect.y+menu[2].rect.h/2);

                fi = atan2(tmpy,tmpx);
                refresh_scene(ren, &sub_v);
                break;
            }
            break;

        case SDL_MOUSEBUTTONDOWN:
            if(myevent.button.button != SDL_PRESSED)
                break;

            clicked_opt_menu(menu,11,myevent.button.x,myevent.button.y,&choice);
            switch(choice) {
            case FINEWIN:
                done = 1;
                break;

            case RIGHTWIN:
                ssx=-sin(teta);
                ssy=cos(teta);
                csx-=sr*ssx;
                csy-=sr*ssy;
                refresh_scene(ren, &sub_v);
                break;
                
            case LEFTWIN:
                ssx=-sin(teta);
                ssy=cos(teta);
                csx+=sr*ssx;
                csy+=sr*ssy;
                refresh_scene(ren, &sub_v);
                break;
                
            case UPWIN:
                ssx=-cos(fi)*cos(teta);
                ssy=-cos(fi)*sin(teta);
                ssz=sin(fi);
                csx+=sr*ssx;
                csy+=sr*ssy;
                csz+=sr*ssz;
                refresh_scene(ren, &sub_v);
                break;	
                
            case DOWNWIN:
                ssx=-cos(fi)*cos(teta);
                ssy=-cos(fi)*sin(teta);
                ssz=sin(fi);
                csx-=sr*ssx;
                csy-=sr*ssy;
                csz-=sr*ssz;
                refresh_scene(ren, &sub_v);
                break;
                
            case ZIWIN:
                if (alpha-salpha >0)
                    alpha-=salpha;
                refresh_scene(ren, &sub_v);
                break;
                
            case ZOWIN:
                if (alpha+salpha <1.57)
                    alpha+=salpha;
                refresh_scene(ren, &sub_v);
                break;
                
            case DZIWIN:
                if (D-Dstep>0)
                    D-=Dstep;
                refresh_scene(ren, &sub_v);
                break;
                
            case DZOWIN:
                D+=Dstep;
                refresh_scene(ren, &sub_v);
                break;
            }
            break;  

        case SDL_KEYDOWN:
            if(myevent.key.keysym.sym == SDLK_ESCAPE)
                done = 1;
            break; 
        }
        SDL_EventState(0xff, SDL_ENABLE);
    }

    TTF_CloseFont (font);
    TTF_Quit ();
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit ();
    return (0);
}