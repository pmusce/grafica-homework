#include <stdio.h>
#include <math.h>
#include "../GCGraLib2/GCGraLib2.h"
#include "viewtransf.h"

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
#define VUPWIN      11

/* Dati per visualizzare un oggetto definito solo da vertici e lati*/
float   x[MAXVERT],y[MAXVERT],z[MAXVERT];       /* coordinate vertici */
int     edge[MAXEDGE][2];                       /* lista lati */

#define CAMERA_VERTEX   4
vec3 camera[CAMERA_VERTEX];

int     nvert,nedge;                            /* numero vertici e lati */
float   csx,csy,csz;                            /* centro oggetto */
float   D = 10,teta = 1.57,fi = 20,di;
vec3    view_up, obs_vup;
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
    set_menu_opt(&menu[TETAWIN], 605, 10, 100, 100, "THETA");
    set_menu_opt(&menu[FIWIN], 605, 120, 100, 100, "PHI");
    set_menu_opt(&menu[ZIWIN], 602, 340, 50, 30, "ZoomI");
    set_menu_opt(&menu[ZOWIN], 662, 340, 50, 30, "ZoomO");
    set_menu_opt(&menu[DZIWIN], 602, 380, 50, 30, "DZoomI");
    set_menu_opt(&menu[DZOWIN], 662, 380, 50, 30, "DZoomO");
    set_menu_opt(&menu[RIGHTWIN], 662, 460, 50, 30, "RIGHT");
    set_menu_opt(&menu[LEFTWIN], 602, 460, 50, 30, "LEFT");
    set_menu_opt(&menu[UPWIN], 631, 420, 50, 30, "UP");
    set_menu_opt(&menu[DOWNWIN], 631, 500, 50, 30, "DOWN");
    set_menu_opt(&menu[VUPWIN], 605, 230, 100, 100, "ViewUp");

    // draw teta and fi controller
    SDL_RenderDrawLine(ren, menu[TETAWIN].rect.x+menu[TETAWIN].rect.w/2, menu[TETAWIN].rect.y+menu[TETAWIN].rect.h/2,
        (int)(menu[TETAWIN].rect.w/2)*cos(teta)+menu[TETAWIN].rect.x+menu[TETAWIN].rect.w/2,
        (int)(menu[TETAWIN].rect.h/2)*sin(teta)+menu[TETAWIN].rect.y+menu[TETAWIN].rect.h/2);
    SDL_RenderDrawLine(ren, menu[FIWIN].rect.x+menu[FIWIN].rect.w/2, menu[FIWIN].rect.y+menu[FIWIN].rect.h/2,
        (int)(menu[FIWIN].rect.w/2)*cos(fi)+menu[FIWIN].rect.x+menu[FIWIN].rect.w/2,
        (int)(menu[FIWIN].rect.h/2)*sin(fi)+menu[FIWIN].rect.y+menu[FIWIN].rect.h/2);
    SDL_RenderDrawLine(ren, menu[VUPWIN].rect.x+menu[VUPWIN].rect.w/2, menu[VUPWIN].rect.y+menu[VUPWIN].rect.h/2,
        (int)(menu[VUPWIN].rect.w/2)*cos(fi)+menu[VUPWIN].rect.x+menu[VUPWIN].rect.w/2,
        (int)(menu[VUPWIN].rect.h/2)*sin(fi)+menu[VUPWIN].rect.y+menu[VUPWIN].rect.h/2);
}

void draw_menu(RECT menu[], SDL_Renderer *ren, TTF_Font *font)
{
    int i;

    for( i=0 ; i<=11 ; i++ )
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

void define_camera() {
    vec3 xaxis, yaxis, zaxis;
    vec3 target;

    target.x = csx;
    target.y = csy;
    target.z = csz;

    calculate_vp(&camera[0], D, teta, fi);
    calculate_zaxis(&camera[0], &target, &zaxis);
    camera[1].x = camera[0].x + zaxis.x;
    camera[1].y = camera[0].y + zaxis.y;
    camera[1].z = camera[0].z + zaxis.z;
    calculate_xaxis(&zaxis, &view_up, &xaxis);
    camera[2].x = camera[0].x + xaxis.x;
    camera[2].y = camera[0].y + xaxis.y;
    camera[2].z = camera[0].z + xaxis.z;
    calculate_yaxis(&zaxis, &xaxis, &yaxis);
    camera[3].x = camera[0].x + yaxis.x;
    camera[3].y = camera[0].y + yaxis.y;
    camera[3].z = camera[0].z + yaxis.z;
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

void draw_camera(SDL_Renderer *ren, float D, float teta, float fi, vec3 *view_up) {
    int c1x, c1y, c2x, c2y;
    float xe, ye, ze;

    general_trasf_view_up_vect(camera[0].x-csx, camera[0].y-csy, camera[0].z-csz, &xe, &ye, &ze, D, teta, fi, view_up);
    c1x = (int)(Sx * ((D * xe)/ze - xwmin) + xvmin + 0.5);
    c1y = (int)(Sy * (ywmin - (D * ye)/ze) + yvmax + 0.5);

    general_trasf_view_up_vect(camera[1].x-csx, camera[1].y-csy, camera[1].z-csz, &xe, &ye, &ze, D, teta, fi, view_up);
    c2x = (int)(Sx * ((D * xe)/ze - xwmin) + xvmin + 0.5);
    c2y = (int)(Sy * (ywmin - (D * ye)/ze) + yvmax + 0.5);
    SDL_SetRenderDrawColor(ren,255,0,0,255);
    SDL_RenderDrawLine(ren, c1x, c1y, c2x, c2y);

    general_trasf_view_up_vect(camera[2].x-csx, camera[2].y-csy, camera[2].z-csz, &xe, &ye, &ze, D, teta, fi, view_up);
    c2x = (int)(Sx * ((D * xe)/ze - xwmin) + xvmin + 0.5);
    c2y = (int)(Sy * (ywmin - (D * ye)/ze) + yvmax + 0.5);
    SDL_SetRenderDrawColor(ren,0,255,0,255);
    SDL_RenderDrawLine(ren, c1x, c1y, c2x, c2y);

    general_trasf_view_up_vect(camera[3].x-csx, camera[3].y-csy, camera[3].z-csz, &xe, &ye, &ze, D, teta, fi, view_up);
    c2x = (int)(Sx * ((D * xe)/ze - xwmin) + xvmin + 0.5);
    c2y = (int)(Sy * (ywmin - (D * ye)/ze) + yvmax + 0.5);
    SDL_SetRenderDrawColor(ren,0,0,255,255);
    SDL_RenderDrawLine(ren, c1x, c1y, c2x, c2y);

}

void draw_mesh(SDL_Renderer *ren, float D, float teta, float fi, int isObserver)
{
    int t,u,k;
    float xe,ye,ze;    /* coord. dell'osservatore */
    vec3 *curr_vup;

    s=D*tan(alpha);

    if(isObserver) {
        curr_vup = &obs_vup;
        s = 10;
    } else {
        curr_vup = &view_up;
    }

    /* si ricalcola il semilato della window in base a D e ad alpha */
    xwmin = -s ;
    xwmax = s;
    ywmin = -s;
    ywmax = s;

    /* fattori di scala per trasf. Window-Viewport */
    Sx = ((float)(xvmax) - (float)(xvmin))/(xwmax - xwmin);
    Sy = ((float)(yvmax) -(float)(yvmin))/(ywmax - ywmin);

    /* il piano di proiezione viene definito a passare per l'origine */
    di=D;
    for (k=0;k<nvert;k++)
    {
        general_trasf_view_up_vect(x[k]-csx, y[k]-csy, z[k]-csz, &xe, &ye, &ze, D, teta, fi, curr_vup);
        //trasf_prosp_gen(&init,x[k]-csx,y[k]-csy,z[k]-csz,&xe,&ye,&ze);
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

    if(isObserver)
        draw_camera(ren, D, teta, fi, curr_vup);

    SDL_RenderPresent(ren);
}


void refresh_scene(SDL_Renderer *ren, SDL_Renderer* obs_ren, SDL_Rect *sub_v) {
    SDL_SetRenderDrawColor(ren,255,255,255,255);
    SDL_RenderFillRect(ren, sub_v);
    SDL_SetRenderDrawColor(ren,0,0,0,255);
    SDL_SetRenderDrawColor(obs_ren,255,255,255,255);
    SDL_RenderFillRect(obs_ren, sub_v);
    SDL_SetRenderDrawColor(obs_ren,0,0,0,255);
    define_camera();
    draw_mesh(ren, D, teta, fi, 0);
    draw_mesh(obs_ren, 50, 20, 20, 1);
}

int main()
{
    SDL_Window *win, *wind;
    SDL_Renderer *ren, *rend;
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
    obs_vup.x = view_up.x = 0;
    obs_vup.y = view_up.y = 0;
    obs_vup.z = view_up.z = 1;


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

    // new window for drawing
    wind= SDL_CreateWindow("Polygon", 100, 100, sub_v.w, sub_v.h, SDL_WINDOW_SHOWN);
    if(wind==NULL){
        fprintf(stderr,"SDL_CreateWindow Error: %s\n",SDL_GetError());
        SDL_Quit();
        return 1;
    }
    rend = SDL_CreateRenderer(wind, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (rend == NULL){
        SDL_DestroyWindow(wind);
        fprintf(stderr,"SDL_CreateRenderer Error: %s\n",SDL_GetError());
        SDL_Quit();
        return 1;
    }
    SDL_SetRenderDrawColor(rend, 255, 255, 255, 255);
    SDL_RenderClear(rend);
    SDL_RenderPresent(rend);

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
    refresh_scene(ren, rend, &sub_v);

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
                refresh_scene(ren, rend, &sub_v);
                break;
            }

            clicked_opt_menu(menu,11,myevent.motion.x,myevent.motion.y,&choice);
            switch(choice) {
            case TETAWIN:
                tmpx = myevent.motion.x-(menu[TETAWIN].rect.x+menu[TETAWIN].rect.w/2);
                tmpy = myevent.motion.y-(menu[TETAWIN].rect.y+menu[TETAWIN].rect.h/2);
                teta = atan2(tmpy,tmpx);
                // clear teta rect
                SDL_SetRenderDrawColor(ren,255,255,255,255);
                SDL_RenderFillRect(ren,&(menu[TETAWIN].rect));
                SDL_SetRenderDrawColor(ren,0,0,0,255);
                GC_DrawText(ren, font, 0, 0, 0, 0, 255, 255, 255, 0, menu[TETAWIN].text,
                    menu[TETAWIN].rect.x, menu[TETAWIN].rect.y, shaded);
                SDL_RenderDrawRect(ren,&(menu[TETAWIN].rect));
                SDL_RenderDrawLine(ren, myevent.motion.x,myevent.motion.y,
                    menu[TETAWIN].rect.x+menu[TETAWIN].rect.w/2,menu[TETAWIN].rect.y+menu[TETAWIN].rect.h/2);

                refresh_scene(ren, rend, &sub_v);
                break;

            case FIWIN:
                tmpx = myevent.motion.x-(menu[FIWIN].rect.x+menu[FIWIN].rect.w/2);
                tmpy = myevent.motion.y-(menu[FIWIN].rect.y+menu[FIWIN].rect.h/2);
                SDL_SetRenderDrawColor(ren,255,255,255,255);
                SDL_RenderFillRect(ren,&(menu[FIWIN].rect));
                SDL_SetRenderDrawColor(ren,0,0,0,255);
                GC_DrawText(ren, font, 0, 0, 0, 0, 255, 255, 255, 0, menu[FIWIN].text,
                    menu[FIWIN].rect.x, menu[FIWIN].rect.y, shaded);
                SDL_RenderDrawRect(ren,&(menu[FIWIN].rect));
                SDL_RenderDrawLine (ren, myevent.motion.x,myevent.motion.y,
                    menu[FIWIN].rect.x+menu[FIWIN].rect.w/2,menu[FIWIN].rect.y+menu[FIWIN].rect.h/2);

                fi = atan2(tmpy,tmpx);
                refresh_scene(ren, rend, &sub_v);
                break;


            case VUPWIN:
                tmpx = myevent.motion.x-(menu[VUPWIN].rect.x+menu[VUPWIN].rect.w/2);
                tmpy = myevent.motion.y-(menu[VUPWIN].rect.y+menu[VUPWIN].rect.h/2);
                SDL_SetRenderDrawColor(ren,255,255,255,255);
                SDL_RenderFillRect(ren,&(menu[VUPWIN].rect));
                SDL_SetRenderDrawColor(ren,0,0,0,255);
                GC_DrawText(ren, font, 0, 0, 0, 0, 255, 255, 255, 0, menu[VUPWIN].text,
                    menu[VUPWIN].rect.x, menu[VUPWIN].rect.y, shaded);
                SDL_RenderDrawRect(ren,&(menu[VUPWIN].rect));
                SDL_RenderDrawLine (ren, myevent.motion.x,myevent.motion.y,
                    menu[VUPWIN].rect.x+menu[VUPWIN].rect.w/2,menu[VUPWIN].rect.y+menu[VUPWIN].rect.h/2);

                view_up.x = tmpx;
                view_up.z = tmpy;
                refresh_scene(ren, rend, &sub_v);
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
                refresh_scene(ren, rend, &sub_v);
                break;

            case LEFTWIN:
                ssx=-sin(teta);
                ssy=cos(teta);
                csx+=sr*ssx;
                csy+=sr*ssy;
                refresh_scene(ren, rend, &sub_v);
                break;

            case UPWIN:
                ssx=-cos(fi)*cos(teta);
                ssy=-cos(fi)*sin(teta);
                ssz=sin(fi);
                csx+=sr*ssx;
                csy+=sr*ssy;
                csz+=sr*ssz;
                refresh_scene(ren, rend, &sub_v);
                break;

            case DOWNWIN:
                ssx=-cos(fi)*cos(teta);
                ssy=-cos(fi)*sin(teta);
                ssz=sin(fi);
                csx-=sr*ssx;
                csy-=sr*ssy;
                csz-=sr*ssz;
                refresh_scene(ren, rend, &sub_v);
                break;

            case ZIWIN:
                if (alpha-salpha >0)
                    alpha-=salpha;
                refresh_scene(ren, rend, &sub_v);
                break;

            case ZOWIN:
                if (alpha+salpha <1.57)
                    alpha+=salpha;
                refresh_scene(ren, rend, &sub_v);
                break;

            case DZIWIN:
                if (D-Dstep>0)
                    D-=Dstep;
                refresh_scene(ren, rend, &sub_v);
                break;

            case DZOWIN:
                D+=Dstep;
                refresh_scene(ren, rend, &sub_v);
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
