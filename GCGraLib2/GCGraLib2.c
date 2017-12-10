/*
 * file GCGraLib2.c
 * by Giulio Casciola 2015
 */
#include <string.h>
//#include <SDL2/SDL.h>
//#include <SDL2/SDL_ttf.h>
//#include <SDL2/SDL_image.h>
//#include <SDL2/SDL_video.h>
#include "GCGraLib2.h"

/************************************************************************
GC_DrawText: funzione di disegno di testo su SDL_Renderer;
Input:
-(arg.1) puntatore alla struttura finestra in cui si vuole disegnare;
-(arg.2) puntatore alla fonte da usare;
-(arg.3-6) quadrupla RGBA del colore per il testo;
-(arg.7-10) quadrupla RGBA del colore di sfondo del testo;
-(arg.11) stringa di testo da disegnare;
-(arg.12-13) posizione (ascissa e ordinata) schermo in cui mettere
             la stringa (vertice alto sinistro della matrice di pixel);
-(arg.14) qualita' di rendering del font: "solid", "shaded", "blended";
***************************************************************************/
void GC_DrawText(SDL_Renderer *ren, TTF_Font *fonttodraw, char fgR, char fgG, char fgB, char fgA,
              char bgR, char bgG, char bgB, char bgA, char text[], int x, int y,
              enum textquality quality)
{
SDL_Color tmpfontcolor = {fgR,fgG,fgB,fgA};
SDL_Color tmpfontbgcolor = {bgR, bgG, bgB, bgA};
SDL_Rect dstrect;
SDL_Surface *resulting_text;
SDL_Texture *tex;

  resulting_text=NULL;
  if (quality == solid)
    resulting_text = TTF_RenderText_Solid(fonttodraw, text, tmpfontcolor);
  else
    if (quality == shaded)
       resulting_text = TTF_RenderText_Shaded(fonttodraw, text, tmpfontcolor, tmpfontbgcolor);
    else
      if (quality == blended)
         resulting_text = TTF_RenderText_Blended(fonttodraw, text, tmpfontcolor);

  if ( resulting_text != NULL ) 
  {
    dstrect.x = x;
    dstrect.y = y;
    dstrect.w = resulting_text->w;
    dstrect.h = resulting_text->h;
    tex = SDL_CreateTextureFromSurface(ren, resulting_text);
    SDL_RenderCopy(ren, tex, NULL, &dstrect);
  }
  SDL_DestroyTexture(tex);
  SDL_FreeSurface(resulting_text);
}

/**********************************************************************
GC_DrawRect: funzione di disegno di un rettangolo su SDL_Renderer;
Input: 
-(arg.1) puntatore alla struttura finestra in cui si vuole disegnare;
-(arg.2-3) coordiante vertice alto sinistro del rettangolo;
-(arg.4-5) ampiezze del rettangolo;
**********************************************************************/
void GC_DrawRect(SDL_Renderer *ren,int ax,int ay,int width,int height)
{
SDL_Rect rect;

    rect.x=ax;
    rect.y=ay;
    rect.w=width;
    rect.h=height;
    SDL_RenderDrawRect(ren,&rect);
}

/**********************************************************************
GC_DrawRect1: funzione di disegno di un rettangolo su SDL_Surface;
Input:
-(arg.1) puntatore alla struttura finestra in cui si vuole disegnare;
-(arg.2-3) coordiante vertice alto sinistro del rettangolo;
-(arg.4-5) ampiezze del rettangolo;
-(arg.6) colore di disegno.
**********************************************************************/
void GC_DrawRect1(SDL_Surface *s,int ax,int ay,int width,int height, Uint32 c)
{
/*
  GC_DrawLine1(s,ax,ay,ax+width,ay,c);
  GC_DrawLine1(s,ax+width,ay,ax+width,ay+height,c);
  GC_DrawLine1(s,ax+width,ay+height,ax,ay+height,c);
  GC_DrawLine1(s,ax,ay+height,ax,ay,c);
*/
  GC_HorizLine1(s, ax, ax+width, ay, c);
  GC_HorizLine1(s, ax, ax+width, ay+height, c);
  GC_VerticLine1(s, ax, ay, ay+height, c);
  GC_VerticLine1(s, ax+width, ay, ay+height, c);
}

/*******************************************************************************
GC_LoadImage: funzione per caricare un'immagine in formato PPM, PNG o JPG da file;
tale immagine verra' memorizzata in una struttura di tipo SDL_Surface allocata
dalla funzione stessa.
Input: 
-(arg.1) puntatore al file immagine da leggere;
-(arg.2) esito del caricamento (0 fallito, 1 riuscito)
Output:
valore di ritorno: puntatore alla struttura SDL_Surface allocata per contenere 
l'immagine.
*******************************************************************************/
SDL_Surface* GC_LoadImage(char *file, int *exitstate)
{
SDL_Surface *tmp;

 tmp = IMG_Load(file);
 *exitstate = 0;
 if (tmp == NULL)
 {
   fprintf(stderr, "Error: '%s' could not be opened: %s\n", file, IMG_GetError());
 }
 else
 {
   *exitstate = 1;
 }
 return tmp;
}

/********************************************************************************
GC_DrawImage: funzione di disegno di un'immagine su SDL_Surface;
Input: 
-(arg.1) puntatore alla struttura immagine che si vuole disegnare;
-(arg.2-3) coordiante vertice alto sinistro dell'immagine che si vuole disegnare;
-(arg.4-5) ampiezze del rettangolo immagine che si vuole disegnare;
-(arg.6) puntatore alla struttura finestra in cui si vuole disegnare;
-(arg.7-8) coordiante vertice alto sinistro del rettangolo finestra in cui si 
vuole disegnare;
*********************************************************************************/
void GC_DrawImage(SDL_Surface *srcimg, int sx, int sy, int sw, int sh, SDL_Renderer *ren,
               int dx, int dy)
{
SDL_Rect src, dst;
SDL_Texture *tex;

  if (srcimg == NULL)
    return; //If theres no image, or its 100% transparent.

  src.x = sx;  src.y = sy;  src.w = sw;  src.h = sh;
  dst.x = dx;  dst.y = dy;  dst.w = sw;  dst.h = sh;
  //SDL_BlitSurface(srcimg, &src, dstimg, &dst);
  tex = SDL_CreateTextureFromSurface(ren, srcimg);
  SDL_RenderCopy(ren, tex, &src, &dst);
  SDL_DestroyTexture(tex);
}

static int iabs(int i) 
{ 
  if(i<0) return(-i); 
  else return(i); 
}

static inline int incdir(int a,int b)
{
  if(a>b) return(-1);
  else return(1);
}

static inline void swap(int *a, int *b)
{
  int tmp=*a;
  *a=*b;
  *b=tmp;
}

/***********************************************************************
GC_GetPixelImage: funzione di lettura di un pixel di un'immagine;
 * Return the pixel value at (x, y); useful for PPM, JPG and PNG formats
 * that use BIG-ENDIAN!!!
 * NOTE: The surface/image must be locked before calling this!
Input: 
-(arg.1) puntatore alla struttura immagine a cui si vuole accedere;
-(arg.2-3) coordiante del pixel che si vuole leggere;
Output:
valore di ritorno: colore del pixel.
************************************************************************/
Uint32 GC_GetPixelImage(SDL_Surface *image, int x, int y)
{
int bpp = image->format->BytesPerPixel;
/* Here p is the address to the pixel we want to retrieve */
Uint8 *p = (Uint8 *)image->pixels + y * image->pitch + x * bpp;

  switch(bpp) {
  case 1:
    return *p;
  case 2:
    return *(Uint16 *)p;
  case 3:
    if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
      return p[2] << 16 | p[1] << 8 | p[0];
    else
      return p[2] | p[1] << 8 | p[0] << 16;
  case 4:
    return *(Uint32 *)p;
  default:
    return 0; /* shouldn't happen, but avoids warnings */
  }
}

/********************************************************************
GC_GetPixel: funzione di lettura di un pixel da un SDL_Renderer;
 * Return the pixel value at (x, y)
 * NOTE: The surface must be locked before calling this!
Input: 
-(arg.1) puntatore alla struttura finestra a cui si vuole accedere;
-(arg.2-3) coordiante del pixel che si vuole leggere;
Output:
valore di ritorno: colore del pixel.
********************************************************************/
Uint32 GC_GetPixel(SDL_Renderer *ren, int x, int y)
{
SDL_Rect rect;
void *pixels=NULL;
int pitch=0;

rect.x=x;
rect.y=y;
rect.w=0;
rect.h=0;

SDL_RenderReadPixels(ren, &rect, 0, pixels, pitch);
return *((Uint32 *)pixels);

}

/***********************************************************************
GC_PutPixel: funzione di scrittura di un pixel su un SDL_Renderer;
 * Set the pixel at (x, y) to the given value
 * NOTE: The surface must be locked before calling this!
Input: 
-(arg.1) puntatore alla struttura finestra a cui si vuole accedere;
-(arg.2-3) coordiante del pixel a cui si vuole accedere;
-(arg.4) colore che si vuole scrivere.
***********************************************************************
void GC_PutPixel(SDL_Renderer *ren, int x, int y)
{
  SDL_RenderDrawPoint(ren,x,y);
}
***********************************************************************/

/********************************************************************
GC_GetPixel1: funzione di lettura di un pixel da un SDL_Surface;
 * Return the pixel value at (x, y)
 * NOTE: The surface must be locked before calling this!
Input:
-(arg.1) puntatore alla struttura finestra a cui si vuole accedere;
-(arg.2-3) coordiante del pixel che si vuole leggere;
Output:
valore di ritorno: colore del pixel.
********************************************************************/
Uint32 GC_GetPixel1(SDL_Surface *surface, int x, int y)
{
int bpp = surface->format->BytesPerPixel;
/* Here p is the address to the pixel we want to retrieve */
Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

  switch(bpp) {
  case 1:
    return *p;
  case 2:
    return *(Uint16 *)p;
  case 3:
    if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
      return p[0] << 16 | p[1] << 8 | p[2];
    else
//original
      return p[0] | p[1] << 8 | p[2] << 16;
//new
//      printf("getpixel %d %d %d\n",p[2],p[1],p[0]);
      return p[2] | p[1] << 8 | p[0] << 16;
  case 4:
    return *(Uint32 *)p;
  default:
    return 0; /* shouldn't happen, but avoids warnings */
  }
}

/*********************************************************************
GC_HorizLine1: funzione di disegno di una linea orizzontale su SDL_Surface;
 * Quickly draws horizontal lines, taking advantage of the fact that
 * characters are grouped by horizontal line in image[][].  After
 * checking for clipping it can simply fill x2-x characters in a row
 * with cur_color by calling memset.
Input:
-(arg.1) puntatore alla struttura finestra in cui si vuole disegnare;
-(arg.2-3) ascisse degli estremi della linea;
-(arg.4) ordinata della linea;
-(arg.5) colore di disegno.
***********************************************************************/
void GC_HorizLine1(SDL_Surface *s, int x, int x2, int y, Uint32 color)
{
SDL_Rect r;

  r.y=y;
  r.h=1;

  if(x2<x)
  {
    r.x=x2;
    r.w=x-x2;
  }
  else
  {
    r.x=x;
    r.w=x2-x;
  }
  SDL_FillRect(s,&r,color);
}

/*********************************************************************
GC_VerticLine1: funzione di disegno di una linea verticale su SDL_Surface;
 * Quickly draws vertical lines.  After
 * checking for clipping it can simply fill y2-y characters in a column
 * with cur_color by calling memset.
Input:
-(arg.1) puntatore alla struttura finestra in cui si vuole disegnare;
-(arg.2) ascissa della linea;
-(arg.3-4) ordinate degli estremi della linea;
-(arg.5) colore di disegno.
***********************************************************************/
void GC_VerticLine1(SDL_Surface *s, int x, int y, int y2, Uint32 color)
{
  SDL_Rect r;

  r.x=x;
  r.w=1;
  if(y2<y)
  {
    r.y=y2;
    r.h=y-y2;
  }
  else
  {
    r.y=y;
    r.h=y2-y;
  }
  SDL_FillRect(s,&r,color);
}

/*******************************************************************
GC_DrawCircle: funzione di disegno di una circonferenza su SDL_Renderer;
* Converts one 1/8 of an arc into a full circle. *
Input: 
-(arg.1) puntatore alla struttura finestra in cui si vuole disegnare;
-(arg.2-3) coordinate centro della circonferenza;
-(arg.4) raggio della circonferenza;
********************************************************************/
void GC_DrawCircle(SDL_Renderer *ren, int x1, int y1, int ir) 
{
int x,y,d;

  x = ir;
  y = 0;
  d = 3 - 2*ir;
  while (y < x )
   {
    SDL_RenderDrawPoint(ren,x+x1,y+y1);
    SDL_RenderDrawPoint(ren,y+x1,x+y1);
    SDL_RenderDrawPoint(ren,-y+x1,x+y1);
    SDL_RenderDrawPoint(ren,-x+x1,y+y1);
    SDL_RenderDrawPoint(ren,-x+x1,-y+y1);
    SDL_RenderDrawPoint(ren,-y+x1,-x+y1);
    SDL_RenderDrawPoint(ren,y+x1,-x+y1);
    SDL_RenderDrawPoint(ren,x+x1,-y+y1);
    if (d < 0)
      d = d + 4*y + 6;
    else
     {
      d = d+4*(y-x)+10;
      x--;
     }
    y++;
  }
  if (x==y)
    {
     SDL_RenderDrawPoint(ren,x+x1,y+y1);
     SDL_RenderDrawPoint(ren,-x+x1,y+y1);
     SDL_RenderDrawPoint(ren,-x+x1,-y+y1);
     SDL_RenderDrawPoint(ren,x+x1,-y+y1);
    }
}

/********************************************************************
GC_FillCircle: funzione di disegno di un cerchio pieno su SDL_Renderer;
 * Calculates circle points with the integer midpoint algorithm.
 *
 * There are 8 reflections of the generated arc, 4 on the left
 * and for on the right.  Horizontal lines are drawn between
 * reflections of the same row to fill it.  Horizontal lines
 * are used because they are a special case that draws extremely
 * quickly.
 *
 * Overdraw is reduced by considering that the top-most and bottom-most 
 * reflections only need to be filled  when the row value changes;
 * when the column value alone changes, it just makes the line wider by
 * two pixels.
Input:
-(arg.1) puntatore alla struttura finestra in cui si vuole disegnare;
-(arg.2-3) coordinate centro del cerchio;
-(arg.4) raggio del cerchio;
***********************************************************************/
void GC_FillCircle(SDL_Renderer *ren, int xin, int yin, int rad)
{
  int x=0,y=rad,d=1-rad,deltaE=3,deltaSE=5-(rad*2);

  if(rad<0) return; // sanity checking

  SDL_RenderDrawLine(ren,xin-y,yin,xin+y,yin); // Center line

  while(y>x)
  {
    if(d<0)
    {
      d+=deltaE;
      deltaE+=2;
      deltaSE+=2;
    }
    else
    {
      // Only need to draw these lines when y changes
      SDL_RenderDrawLine(ren,xin-x,yin+y,xin+x,yin+y); // Bottom-most reflections
      SDL_RenderDrawLine(ren,xin-x,yin-y,xin+x,yin-y); // Top-most reflections
      d+=deltaSE;
      deltaE+=2;
      deltaSE+=4;
      y--;
    }

    x++;
    // These lines change y every time x increments.
    SDL_RenderDrawLine(ren,xin-y,yin-x,xin+y,yin-x); // Upper middle reflections
    SDL_RenderDrawLine(ren,xin-y,yin+x,xin+y,yin+x); // Lower middle reflections
  }
}

/*******************************************************************
GC_DrawCircle1: funzione di disegno di una circonferenza su SDL_Surface;
* Converts one 1/8 of an arc into a full circle. *
Input:
-(arg.1) puntatore alla struttura finestra in cui si vuole disegnare;
-(arg.2-3) coordinate centro della circonferenza;
-(arg.4) raggio della circonferenza;
-(arg.5) colore di disegno.
********************************************************************/
void GC_DrawCircle1(SDL_Surface *s, int x1, int y1, int ir, Uint32 color)
{
int x,y,d;

  SDL_LockSurface(s);
  x = ir;
  y = 0;
  d = 3 - 2*ir;
  while (y < x )
   {
    GC_PutPixel1(s,x+x1,y+y1,color);
    GC_PutPixel1(s,y+x1,x+y1,color);
    GC_PutPixel1(s,-y+x1,x+y1,color);
    GC_PutPixel1(s,-x+x1,y+y1,color);
    GC_PutPixel1(s,-x+x1,-y+y1,color);
    GC_PutPixel1(s,-y+x1,-x+y1,color);
    GC_PutPixel1(s,y+x1,-x+y1,color);
    GC_PutPixel1(s,x+x1,-y+y1,color);
    if (d < 0)
      d = d + 4*y + 6;
    else
     {
      d = d+4*(y-x)+10;
      x--;
     }
    y++;
  }
  if (x==y)
    {
     GC_PutPixel1(s,x+x1,y+y1,color);
     GC_PutPixel1(s,-x+x1,y+y1,color);
     GC_PutPixel1(s,-x+x1,-y+y1,color);
     GC_PutPixel1(s,x+x1,-y+y1,color);
    }
  SDL_UnlockSurface(s);
}

/********************************************************************
GC_FillCircle1: funzione di disegno di un cerchio pieno su SDL_Surface;
 * Calculates circle points with the integer midpoint algorithm.
 *
 * There are 8 reflections of the generated arc, 4 on the left
 * and for on the right.  Horizontal lines are drawn between
 * reflections of the same row to fill it.  Horizontal lines
 * are used because they are a special case that draws extremely
 * quickly.
 *
 * Overdraw is reduced by considering that the top-most and bottom-most
 * reflections only need to be filled  when the row value changes;
 * when the column value alone changes, it just makes the line wider by
 * two pixels.
Input:
-(arg.1) puntatore alla struttura finestra in cui si vuole disegnare;
-(arg.2-3) coordinate centro del cerchio;
-(arg.4) raggio del cerchio;
-(arg.5) colore di disegno.
***********************************************************************/
void GC_FillCircle1(SDL_Surface *s, int xin, int yin, int rad, Uint32 color)
{
  int x=0,y=rad,d=1-rad,deltaE=3,deltaSE=5-(rad*2);

  if(rad<0) return; // sanity checking

  GC_HorizLine1(s,xin-y,xin+y,yin,color); // Center line
  while(y>x)
  {
    if(d<0)
    {
      d+=deltaE;
      deltaE+=2;
      deltaSE+=2;
    }
    else
    {
      // Only need to draw these lines when y changes
      GC_HorizLine1(s,xin-x,xin+x,yin+y,color); // Bottom-most reflections
      GC_HorizLine1(s,xin-x,xin+x,yin-y,color); // Top-most reflections
      d+=deltaSE;
      deltaE+=2;
      deltaSE+=4;
      y--;
    }

    x++;
    // These lines change y every time x increments.
    GC_HorizLine1(s,xin-y,xin+y,yin-x,color); // Upper middle reflections
    GC_HorizLine1(s,xin-y,xin+y,yin+x,color); // Lower middle reflections
  }
}

/***********************************************************************
GC_DrawLine: Funzione di disegno di una linea;
Input:
-(arg.1) puntatore alla struttura finestra in cui si vuole disegnare;;
-(arg.2-3) coordinate primo estremo del linea;
-(arg.4-5) coordinate secondo estremo della linea;
************************************************************************
void GC_DrawLine(SDL_Renderer *ren, int x0, int y0, int x1, int y1)
{
  SDL_RenderDrawLine(ren,x0,y0,x1,y1);
}
************************************************************************/

/***********************************************************************
GC_PutPixel1: funzione di scrittura di un pixel su SDL_Surface;
 * Set the pixel at (x, y) to the given value
 * NOTE: The surface must be locked before calling this!
Input:
-(arg.1) puntatore alla struttura finestra a cui si vuole accedere;
-(arg.2-3) coordiante del pixel a cui si vuole accedere;
-(arg.4) colore che si vuole usare.
************************************************************************/
void GC_PutPixel1(SDL_Surface *surface, int x, int y, Uint32 pixel)
{
    int bpp = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to set */
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

    if((x>surface->w)||(y>surface->h)||(x<0)||(y<0)) return;

    switch(bpp) {
    case 1:
        *p = pixel;
        break;

    case 2:
        *(Uint16 *)p = pixel;
        break;
    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
            p[0] = (pixel >> 16) & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = pixel & 0xff;
        } else {
//versione originale
            p[0] = pixel & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = (pixel >> 16) & 0xff;
//versione modificata
//            p[2] = pixel & 0xff;
//            p[1] = (pixel >> 8) & 0xff;
//            p[0] = (pixel >> 16) & 0xff;
        }
        break;

    case 4:
        *(Uint32 *)p = pixel;
        break;
    }
}

/***********************************************************************
GC_DrawLine1: Funzione di disegno di una linea su SDL_Surface;
*Algoritmo di Linea di Bresenham*
Input:
-(arg.1) puntatore alla struttura finestra in cui si vuole disegnare;;
-(arg.2-3) coordinate primo estremo del linea;
-(arg.4-5) coordinate secondo estremo della linea;
-(arg.6) colore di disegno.
************************************************************************/
void GC_DrawLine1(SDL_Surface *s, int x0, int y0, int x1, int y1, Uint32 color)
{
  // absolute values of dx and dy, so as to not screw up calculation
  int dx=iabs(x1-x0),dy=iabs(y1-y0),x=x0,y=y0;
  // When true, the loop will iterate through y instead of x
  int reverse=dy>dx;
  // These record which direction the line should go
  int xdir=incdir(x0,x1),ydir=incdir(y0,y1);
  int d,incrE,incrNE;

  SDL_LockSurface(s);
  // Swap dx and dy if reversed, so as to not fubar equation
  if(reverse) swap(&dy,&dx);

  // Initialize.  If
  d=(dy*2)-dx;
  incrE=dy*2;
  incrNE=(dy-dx)*2;
  // Draw first pixel
  GC_PutPixel1(s,x,y,color);
  if(reverse)
    while(y!=y1) // Iterate through y
    {
      y+=ydir;
      if(d<=0)
        d+=incrE;
      else
      {
        x+=xdir;
        d+=incrNE;
      }
      // Draw next pixel
      GC_PutPixel1(s,x,y,color);
    }
  else
    while(x!=x1) // Iterate through x
    {
      x+=xdir;
      if(d<=0)
        d+=incrE;
      else
      {
        y+=ydir;
        d+=incrNE;
      }
      // Draw pixel
      GC_PutPixel1(s,x,y,color);
    }
  SDL_UnlockSurface(s);
}
