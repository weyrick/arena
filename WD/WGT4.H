/* 
   WordUp Graphics Toolkit V4.0 Header File
   Copyright 1994 Chris Egerter
*/

#include <stdio.h>

//#ifdef __cplusplus
//  "C" {
//#endif

typedef unsigned char far * block;
typedef unsigned char far * wgtfont;
typedef unsigned char far * wgtmap;

/* Text grid settings */
#define TEXTGRID_OFF 0
#define TEXTGRID_ON 1

/* Text transparency settings */
#define TEXTFG 0
#define TEXTBG 1
#define TEXTFGBG 2

/* Image transfer operations */
#define NORMAL 0
#define XRAY 1

/* CPU detection results */
#define CPUOLD     0
#define CPU286     2
#define CPU386     3
#define CPU486     4 
#define CPUPENTIUM 5

/* whollowpoly types */
#define CLOSED_POLY 0
#define OPEN_POLY 1

/* Flip block operations */
#define VERTICAL 0
#define HORIZONTAL 1    


/* A single palette register definition */
typedef struct 
 {
  unsigned char r,g,b;
 } color;


/* Polygon point definition */
typedef struct
    {
    int x,y;   /* Coordinate on the screen */
    int sx,sy; /* Coordinate on the texture if texture is used,
                  otherwise sx = color point */
            /* sx is used for Gouraud shading as well (color of the point) */
    } tpolypoint;

/* Scan converted polygon routines */
typedef struct {
    char *pointbuffer[200];     /* Holds a buffer of x coordinates for each
                                   row of the polygon. */
    unsigned numpoints[200];    /* Holds the size of the point buffer in
                                   bytes. */   
    } wscanpoly;



/* WGTLIB variables */
extern fpos_t lfpos;
extern FILE *libf;
extern int  lresult;
extern long lsize;
extern char password[16];
extern char *wgtlibrary;

/* Keyboard Interrupt */
extern int kbdon[128];
extern int keypressed;

/* Mouse variables */
extern int but;
extern int mx;
extern int my;
extern void far (*mouseintr)(void);

/* Clipping boundaries */
extern int bx,by,tx,ty;

/* Current drawing color */
extern unsigned char currentcolor;

/* Software cursor flashing speed */
extern int curspeed;

/* Software cursor coordinates */
extern int xc,yc;

/* Pointer to the active drawing page */
extern block abuf;

/* EMS variables */
extern int      wpageframe;
extern int      emshandle;
extern block    emsptr;
extern unsigned emsoffset;
extern int      emspage;
extern long     emscurpage;
extern long     maxemspage;


/* Function Prototypes */
int    checkpassword (char *temp);
void   findfile (char *filename);
void   installkbd (void);
void * lib2buf (char *loadfile);
void   mdeinit (void);
int    minit (void);
void   moff (void);
void   mon (void);
void   mouseshape (int colhot, int rowhot, void far *bitmap);
void   mread (void);
void   msetbounds (int x, int y, int x2, int y2);
void   msetspeed (int x, int y);
void   msetthreshhold (int speed);
void   msetxy (int x, int y);
void   noclick (void);
void   readheader (void);
void   setlib (char *libraryname);
void   setpassword (char *newpass);
void   uninstallkbd (void);
void   vga256 (void);
char   vgadetected (void);
void   wbar (int x, int y, int x2, int y2);
void   wbezier (tpolypoint *rawpts, int numraw, tpolypoint *curvepts, 
                int numcurve);
void   wbutt (int x, int y, int x2, int y2);
void   wcircle (int x_center, int y_center, int radius);
void   wclip (int x, int y, int x2, int y2);
void   wcls (unsigned char col);
void   wcolrotate (unsigned char start, unsigned char finish, int dir,
                   color *palt);
void   wcopyscreen (int x, int y, int x2, int y2, block ptr, int destx,
                    int desty, block to);
int    wdetectcpu (void);
void   wdraw_scanpoly (int x, int y, wscanpoly *newp, 
                    void (*customline)(int,int,int));
void   wellipse (int xc, int yc, int xr, int yr);
void   wems_close (void);
void   wems_copy (block sb, unsigned offset, unsigned size);
long   wems_free (void);
block  wems_getdata (block mydata);
void   wems_getframe (int pagegroup);
block  wems_getsprite (int spr, block *sprites);
int    wems_init (void);
int    wems_loadsprites (color *pal, char *filename, block *sprite,
                         int start, int end);
int    wems_map (int phys, int logi);
int    wems_open (int pages);
int    wems_present (void);
block  wems_storeblock (block spr);
block  wems_storedata (block sb, unsigned size);
void   wems_reset (void);
void   wfade (block myimage, int *patt, int speed);
void   wfade_between (unsigned char start, unsigned char finish, int speed,
                      color *pal1, color *pal2);
void   wfade_between_once (unsigned char start, unsigned char finish, 
                      color *pal1, color *pal2);
void   wfade_in (unsigned char start, unsigned char finish, int speed,
                 color *pal);
void   wfade_in_once (unsigned char start, unsigned char finish, 
                      color *pal, color *temppal);
void   wfade_out (unsigned char start, unsigned char end, int speed,
                  color *pal);
void   wfade_out_once (unsigned char start, unsigned char finish,
                       color *pal);
void   wfastputpixel (int x, int y);
void   wfill_circle (int x_center, int y_center, int radius);
void   wfill_ellipse (int xc, int yc, int xr, int yr);
void   wflashcursor (void);
void   wfline (int x, int y, int x2, int y2);
void   wflipblock (block pr, int dir);
void   wfree_scanpoly (wscanpoly *freep);
void   wfreeblock (block ptr);
void   wfreefont (wgtfont ptr);
void   wfreesprites (block *spr, int start, int end);
int    wget_capslock (void);
int    wget_lalt (void);
int    wget_lctrl (void);
int    wget_lshift (void);
int    wget_numlock (void);
int    wget_ralt (void);
int    wget_rctrl (void);
int    wget_rshift (void);
int    wget_scrlock (void);
int    wgetblockheight (block ptr);
int    wgetblockwidth (block ptr);
int    wgetmode (void);
unsigned char wgetpixel (int x, int y);
int    wgettextheight (char *printit, wgtfont pr);
int    wgettextwidth (char *printit, wgtfont pr);
void   wgouraudpoly (tpolypoint *vertexlist, int numvertex, int x, int y);
void   wgtprintf (int xloc, int yloc, wgtfont wgtprfon, char *fmt, ... );
void   whline (int x1, int x2, int y);
void   whollowpoly (tpolypoint *vertexlist, int numvertex, int x, int y,
                    int closemode);
void   wline (int x, int y, int x2, int y2);
block  wloadblock (char *filename);
block  wloadbmp (char *fname, color *pal);
block  wloadcel (char *filename, color *palload);
wgtfont wloadfont (char *fontfile);
block  wloadpak (char *filename);
void   wloadpalette (char *filepal, color *clr);
block  wloadpcx256 (char *filename, color *pal);
int    wloadsprites (color *pal, char *filename, block *spr,int start, 
                     int end);
block  wnewblock (int x, int y, int x2, int y2);
void   wnormscreen (void);
int    woutchar (int asc, int xc, int yc, wgtfont pr);
void   wouttextxy (int x, int y, wgtfont fnt, char *printit);
void   wpan (unsigned int panofs);
void   wputblock (int x, int y, block pr, int method);
void   wputpixel (int x, int y);
void   wreadpalette (unsigned char start, unsigned char finish, color *palc);
void   wrectangle (int x, int y, int x2, int y2);
void   wregionfill (int x, int y);
void   wremap (color *p1, block picture, color *p2);
void   wresize (int x, int y, int x2, int y2, block resizeblock, int mode);
void   wretrace (void);
int    wsaveblock (char *filename, block ptr);
void   wsavebmp (char *fname, block saveptr, color *pal);
int    wsavecel (char *filename, block savecel, color *palsave);
int    wsavepak (char *filename, block ptr);
void   wsavepalette (char *filepal, color *clr);
void   wsavepcx256 (char *filename, block savepcx, color *pal);
void   wscan_convertpoly (tpolypoint *vertexlist, int numvertex, 
                    wscanpoly *newp);
void   wset_capslock (int state);
void   wset_numlock (int state);
void   wset_scrlock (int state);
void   wsetcolor (unsigned char col);
void   wsetcursor (int start, int end);
void   wsetmode (int mode);
void   wsetpalette (unsigned char start, unsigned char finish, color *pal);
void   wsetrgb (unsigned char num, unsigned char red, unsigned char green, 
                unsigned char blue, color *pal);
void   wsetscreen (block ptr);
void   wskew (int x, int y, block blockname, int degrees);
void   wsline (int x, int y, int x2, int y2, int *lad);
void   wsolidpoly(tpolypoint *vertexlist, int numvertex, int x, int y,
                void (*customline)(int,int,int));
void   wstring (int x, int y, char *s, char *legal, int maxlength);
void   wstyleline (int x, int y, int x2, int y2, unsigned int style);
void   wtextbackground (unsigned char col);
void   wtextcolor (unsigned char col);
void   wtextgrid (int state);
void   wtexttransparent (int mode);
void   wtexturedpoly (tpolypoint *vertexlist, int numvertex, int x, int y,
                      block tex, int mode);
long   wtimer (struct time t, struct time t2);
void   wvertres (int x, int y, int y2, block ptr);
void   wwarp (int sx, int ex, int *tpy, int *bty, block ptr);
void   wwipe (int x, int y, int x2, int y2, block cpy);
void   wxorbox (int x, int y, int x2, int y2, unsigned char col);

/* File Selector */
char *wfilesel (char *mask, char *title, int x, int y, block background);
extern wgtfont filefont;

//#ifdef __cplusplus
//}
//#endif
