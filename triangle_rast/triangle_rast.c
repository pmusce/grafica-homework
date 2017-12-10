#include "triangle_rast.h"

int             	polx[3],poly[3];
extern int             edges , startedge , endedge , scandec;
extern float           scan;

EdgeTable edge_table[3];

/*                                                                    */
/*                          POLYINSERT                                */
/*                                                                    */
/*  Questa procedura ordina i lati del poligono e memorizza le infor- */
/*  mazioni del nuovo lato;e" un ordinamento ad inserzione.           */
/*  DATI IN INGRESSO                                                  */
/*    x1 , y1 , x2 , y2 : coordinate degli estremi del lato.      */
/*                                                                    */
void polyinsert(int edges,float x1,float y1,float x2,float y2)
{
	int j;
	float   ym;

	j=edges-1;
	ym = MAX(y1, y2);
	while ((j != 0) && (edge_table[j-1].yymax < ym))
	{
		edge_table[j].yymax = edge_table[j-1].yymax;
		edge_table[j].yymin = edge_table[j-1].yymin;
		edge_table[j].dx = edge_table[j-1].dx;
		edge_table[j].xa = edge_table[j-1].xa;
		j=j-1;
	}
	/* insert in edge table */
	edge_table[j].yymax = ym;
	edge_table[j].dx = (x2-x1)/(y2-y1);
	if (y1 > y2)
	{
		edge_table[j].yymin=y2;
		edge_table[j].xa=x1;
	}
	else
	{
		edge_table[j].yymin=y1;
		edge_table[j].xa=x2;
	}

	for(j=0; j<edges; j++)
		printf("ET %d: %f %f %f %f\n", j, edge_table[j].yymax, edge_table[j].dx, edge_table[j].yymin, edge_table[j].xa );
}
/* polyinsert */

/*                                                                    */
/*                             LOADPOL                                */
/*                                                                    */
/*  Questa procedura ordina i lati del poligono secondo il valore del-*/
/*  le ordinate piu" grande.Inoltre memorizza e recupera delle infor- */
/*  mazioni relative ai lati eccetto quelli orrizontali.              */
/*                                                                    */
void loadpol(int n)
{
	float x1 , x2 , y1 , y2;
	int   k;
	x1=polx[2];
	y1=poly[2];
	edges=0;
	for (k=0; k<n; k++)
	{
		x2=polx[k];
		y2=poly[k];
		if (y1==y2)
			x1=x2;
		else
		{
			edges=edges+1;
			polyinsert(edges,x1,y1,x2,y2);
			x1=x2;
			y1=y2;
		}
	}
}
/* loadpol */

/*                                                                    */
/*                            INCLUDE                                 */
/*                                                                    */
/*  Questa procedura aggiunge nuovi lati al gruppo che si sta conside-*/
/*  rando.                                                            */
/*                                                                    */
void includ()
{
	while ((endedge <= edges) && (edge_table[endedge].yymax >= scan))
	{
		edge_table[endedge].dx = edge_table[endedge].dx * (-scandec);
		endedge=endedge+1;
	}
}
/* includ */
void xchange(float *a, float *b)
{
	float t;

	t=*a;
	*a=*b;
	*b=t;
}
/* xchange */

/*                                                                    */
/*                           XSORT                                    */
/*                                                                    */
/*  Questa procedura mette un elemento nella posizione corretta usando*/
/*  un algoritmo a bolla.                                             */
/*                                                                    */
void xsort(int kk)
{
	int l;

	l=kk;
	while ((l > startedge) && (edge_table[l].xa < edge_table[l-1].xa))
	{
		xchange(&(edge_table[l].yymin),&(edge_table[l-1].yymin));
		xchange(&(edge_table[l].xa),&(edge_table[l-1].xa));
		xchange(&(edge_table[l].dx),&(edge_table[l-1].dx));
		l=l-1;
	}
}
/* xsort */

/*                                                                    */
/*                        UPDATEXVALUES                               */
/*                                                                    */
/*  Questa procedura rimuove i lati che non devono essere piu" consi- */
/*  derati;inoltre determina i punti di intersezione dei restanti lati*/
/*  con la scanline.                                                  */
/*                                                                    */
void updatexvalues()
{
	int stopedge , beginedge , k , i;

	stopedge=endedge-1;
	beginedge=startedge;
	for (k=beginedge; k<=stopedge; k++)
	{
		if (edge_table[k].yymin < scan)
		{
			edge_table[k].xa=edge_table[k].xa+edge_table[k].dx;
			xsort(k);
		}
		else
		{
			startedge=startedge+1;
			if (startedge <= k)
				for (i=k; i>=startedge; i--)
				{

					edge_table[i].yymin = edge_table[i-1].yymin;
					edge_table[i].xa = edge_table[i-1].xa;
					edge_table[i].dx = edge_table[i-1].dx;
				}
		}
	}
}
/* updatexvalue */

/*                                                                    */
/*                               FILLSCAN                             */
/*                                                                    */
/*  Questa procedura disegna i segmenti individuati dalle intersezioni*/
/*  trovate con la procedura updatexvalues.                           */
/*                                                                    */
void fillscan(SDL_Renderer *ren)
{
	int nx , j , k , y , x1 , x2;

	nx=(int)((endedge-startedge) / 2.0);
	j=startedge;
	y=(int)scan;
	for (k=1; k<=nx; k++)
	{
		if (edge_table[j+1].xa-edge_table[j].xa >= 0.5)
		{
			x1=(int)(edge_table[j].xa+0.5);
			x2=(int)(edge_table[j+1].xa+0.5);
			SDL_RenderDrawLine(ren, x1,y,x2,y);
		}
		j=j+2;
	}
}
/* fillscan */

/*                             SCANCONV                               */
/*                                                                    */
/*  Questa procedura fa lo scan conversion di un poligono.            */
/*  DATI IN INGRESSO                                                  */
/*    n     : numero dei vertici del poligono.                        */
/*    polx  : vettori contenenti le ascisse e le ordinate dei vertici */
/*    poly    del poligono.                                           */
/*                                                                    */
void scanconv(SDL_Renderer *ren, int n)
{
	scandec=1;
	loadpol(n);
	if (edges > 1)
	{
		scan=edge_table[0].yymax-0.5;
		startedge=0;
		endedge=0;
		includ();
		updatexvalues();
		while (endedge != startedge)
		{
			printf("Scan: %f\n", scan);
			fillscan(ren);
			scan=scan-scandec;
			includ();
			updatexvalues();
		}
	}

	SDL_RenderDrawLine(ren, polx[0],poly[0],polx[1],poly[1]);
	SDL_RenderDrawLine(ren, polx[0],poly[0],polx[2],poly[2]);
	SDL_RenderDrawLine(ren, polx[1],poly[1],polx[2],poly[2]);
}
/* scan_conv */
