/**************************************************************************
 * World Design v.1à                                                      *
 * Polygon Object Creation/Placement Utility                              *
 * Coded By Shannon Weyrick                                               *
 * Copyright (c) 1994 by Alternate View Graphics                          *
 **************************************************************************/

#include <dos.h>
#include <stdio.h>
#include <dos.h>
#include <conio.h>
#include <stdlib.h>
#include <string.h>
#include "wgt4.h"
#include "wgtmenu.h"
#include "mshape.h"

#define COORDS 1
#define TRUE 1
#define FALSE 0
#define MAXPOLYGONS 20
#define MAXVERTICES 20
#define MAXVRTXNUM 10

#define HIGHTLIGHTCOLOR 172
#define LOCALORIGINCOLOR 139

void DoInsert(void);
void spinprompt(void);
void InitDropdowns(void);
int Initialize(void);
void CleanUp(void);
void ClipOn(void);
void ClipOff(void);
void UpdateStatusBar(int spec);
void MessageBar(char *string);
void erasegrid(void);
void setgrid(void);
int checkmouse(void);
void PromptBar(int length);
void ClearPrompt(int length);
void PromptBar2(int length);
void ClearPrompt2(int length);
int getnumvert(void);
int getworldz(void);
int getvertz(void);
int choosevert(int choice1, int choice2);
void DumpObjects(void);
void GetAllPolygons(void);
int CheckForHit(void);
int ExportObject(int objectnum);
int ImportObject(void);
int getnumber(FILE *f);
char nextchar(FILE *f);
void UpdateScreen(int hightlight, int color);
void EraseObject(int objectnum);
void MoveObject(int objectnum);
void eraseorigin(void);
void putorigin(void);
void EraseAllObjects(void);
void SlideDisplay(void);
void GotoCoords(int x, int y, int highlight);
void GotoOrigin(void);
int CheckForVert(int scx, int scy, int length, int width);
void SelectObject(void);

struct vertex
 {
   int x, y, z;
   int sx,sy;      // Screen X,Y
   int sxo, syo;   // Screen x,y when centered at origin
 };

struct polygon
 {
   int edges, vrtxnum[MAXVRTXNUM], color;
 };

struct object
 {
   int worldx, worldy, worldz;
   int rotx, roty, rotz;
   int scale;
   int numvertices;
   int numpolygons;
   struct polygon polygons[MAXPOLYGONS];
   struct vertex vertices[MAXVERTICES];
   int backplane;
 };

struct object objects[50];

char *version=".1à";        // Current Program Version
int oldmode;		    // old video mode
char *menubar[10]={" File "," Edit "," Objects "," View  "," Misc ",NULL,NULL,NULL,NULL,NULL};
int menuchoice;
int gridsnap;
int localscale;
int coordx, coordy;
int xcoordx, xcoordy;  // coordx, coordy without the offset added to it
int oldmx,oldmy;
int insertmode;
int numobjects;
int onobject;
int onvertex;
int onpolygon;
int onvrtxnum;
char *answer;
int i;
int oldmousex;
int oldmousey;
int createstage;
int allverticesused;
int all_done_w_polygons;
int first, second;
int edgesmade;
int scale=1;
int offsetx=0, offsety=0;
int selected_object=-1;
const SCREENORIGINX=160;
const SCREENORIGINY=100;

color pal[256];
wgtfont mnufnt;
block background;

void main(void)
{
 int result, temp, errorcheck;
 char ch;


  result=Initialize();
   if (result!=1) exit(0);  // Init failed

do   // Main program loop
 {
    menuchoice=checkmenu();
    temp=checkmouse();  temp++;
    if (kbhit()!=0)
	{
	 ch=getch();
	 if (ch=='g') // grid snap higher
	   {
	    if (gridsnap<50)
	     {
	      erasegrid();gridsnap+=5; setgrid(); putorigin();
	      if (numobjects>0) UpdateScreen(-1,0); // put objects over new grid
	      UpdateStatusBar(0);
	     }
	   }
	 if (ch=='G')  // grid snap lower
	   {
	    if (gridsnap>5)
	     {
	      erasegrid(); gridsnap-=5; setgrid(); putorigin();
	      if (numobjects>0) UpdateScreen(-1,0); // put objects over new grid
	      UpdateStatusBar(0);
	     }
	   }
	 if (ch==27) menuchoice=5;  // Escape
	 if (ch=='e') // shortcut to export object
	  {
	  errorcheck=ExportObject(0);
	  if (errorcheck!=1) {sound(300); delay(1000); nosound();}
	  }
	 if (ch=='o') GotoOrigin();
	 if (ch=='m') MoveObject(selected_object);
	}
     if (menuchoice==3)  // Export Object
       { if (numobjects>0) ExportObject(onobject); }
     if (menuchoice==4)  // Import Object
       {
	ImportObject();
       }

 if (insertmode==TRUE)   // Inserting New Object
   {
    DoInsert();
   }
 }
while (menuchoice!=5);

  CleanUp();
  DumpObjects();
}

void DoInsert(void)
{
     numobjects++;
     onobject=numobjects-1; //-1 becasue objects start at 0, not 1 like numobjects
     moff();
     wsetcolor(LOCALORIGINCOLOR);
     wfastputpixel(mx,my); // Plot objects local origin in orange
   do {  // Make sure there's at least 3 and no more the MAXVERTICES vertices
     objects[onobject].numvertices=getnumvert();  // Ask for numver of vrtces
      } while ((objects[onobject].numvertices<4) || (objects[onobject].numvertices>MAXVERTICES));
     objects[onobject].worldx=coordx;
     objects[onobject].worldy=coordy;
     objects[onobject].worldz=getworldz(); // Ask for objects world Z coord
     objects[onobject].rotx=0;  // Rotation for object initialy
     objects[onobject].roty=0;  // Set to 0
     objects[onobject].rotz=0;
     objects[onobject].scale=0;  // Scale initialy set to 0
     objects[onobject].backplane=1; // Backplane removal set to yes
     mon();
  /***  Place Vertexes of object in relation to objects local origin ***/
     MessageBar("     Placing Vertices..");
     oldmousex=mx;
     oldmousey=my;
     onvertex=0;
     coordx=0;
     coordy=0;
     createstage=1;
     do
      {
	if (checkmouse()==1)
	 {
	   wsetcolor(195);
	   moff();
	   wfastputpixel(mx,my);
	   mon();
	   objects[onobject].vertices[onvertex].x=coordx;
	   objects[onobject].vertices[onvertex].y=coordy;
	   objects[onobject].vertices[onvertex].sx=mx;
	   objects[onobject].vertices[onvertex].sy=my;
	   objects[onobject].vertices[onvertex].z=getvertz();
	   onvertex++;
	 }
      }
     while (onvertex!=objects[onobject].numvertices);
     onpolygon=0;
     onvertex=0;
 /** Creat Faces To The Object Using Polygons w/Vertices from above **/
     MessageBar("Creating Polygons..[BUT2]=NxtPlygn");
     do
      {
	GetAllPolygons();
      }
     while (all_done_w_polygons==FALSE);
     MessageBar("");
     msetxy(oldmousex,oldmousey);
     coordx=objects[onobject].worldx;
     coordy=objects[onobject].worldy;
     insertmode=FALSE;
}


void spinprompt(void)   // Some neat-o little pause prompt
{
 char ch;
    do
     {
      textcolor(15);
      cprintf("\b\\");
      delay(20);
      textcolor(7);
      cprintf("\b|");
      delay(20);
      textcolor(8);
      cprintf("\b/");
      delay(20);
      textcolor(15);
      cprintf("\b-");
      delay(20);
      textcolor(7);
      cprintf("\b\\");
      delay(20);
      textcolor(8);
      cprintf("\b|");
      delay(20);
      textcolor(15);
      cprintf("\b-");
      delay(20);
      textcolor(7);
      cprintf("\b/");
      delay(20);
     }
    while (!kbhit());
 ch=getch();
 ch++;
}

void SetupDropdowns(void)
{
  menubarcolor=9;
  menubartextcolor=28;
  bordercolor=14;
  highlightcolor=38;

  menufont=mnufnt;

  dropdown[0].choice[0]="New";
  dropdown[0].choice[1]="Load World";
  dropdown[0].choice[2]="Save World";
  dropdown[0].choice[3]="Export Object";
  dropdown[0].choice[4]="Import Object";
  dropdown[0].choice[5]="Quit";

  dropdown[1].choice[0]="View";
  dropdown[1].choice[1]="Copy";
  dropdown[1].choice[2]="Cut";
  dropdown[1].choice[3]="Paste";

  dropdown[2].choice[0]="Polygon";
  dropdown[2].choice[1]="Rectangle";
  dropdown[2].choice[2]="EditBox";

  dropdown[3].choice[0]="Render Object";
  dropdown[3].choice[1]="Render World";

  dropdown[4].choice[0]="World Stats";

  initdropdowns();

  dropdown[0].color=9;
  dropdown[0].bordercolor=14;
  dropdown[0].textcolor=21;
  dropdown[1].textcolor=21;
  dropdown[2].textcolor=21;
  dropdown[3].textcolor=21;

  showmenubar();
}


int Initialize(void)
{
 char ch;
 int checkformouse;

//   Initalize Variables:
  localscale=1;
  gridsnap=15;
  coordx=0;
  coordy=0;
  xcoordx=0;
  xcoordy=0;
  first=50;
  second=50;
  edgesmade=0;
  onobject=-1;
  numobjects=0;

  textcolor(11);
  if (vgadetected!=0) // Set VGA Mode if detected
   {
    cprintf("\r\nWorld Design v%s\r\n",version); textcolor(15);
    cprintf("Copyright (c) 1994 Alternate View S/W\r\n");
    checkformouse=minit();
    textcolor(9);
    if (checkformouse<1) {textcolor(12); cprintf("Mouse Not Found!"); exit(0);}
    else {cprintf("%i Button Mouse Found\r\n",checkformouse);}
    textcolor(1);
    cprintf("VGA Detected.. Press A Key To Enter World Design..-");
//    spinprompt();
    oldmode=wgetmode();
    vga256();
    mnufnt = wloadfont("wdmenu.wfn");
    wloadpalette("digwar.pal",pal);
    wsetpalette(0,255,pal);
    SetupDropdowns();
    setgrid();
    UpdateStatusBar(0);
    wtextcolor(44);
    wgtprintf(234,1,mnufnt,"World Design v%s",version);
    mouseshape(5,4,cursor1);
    msetspeed(14,14);
    msetthreshhold(10);
    putorigin();  // lay down the origin
    mon();
    msetbounds(1,1,320,190);
    msetxy(160,100);
    oldmx=160;
    oldmy=100;
    return(1);
   }
  else {cprintf("VGA Card not detected!");}
  return(1);
}

void CleanUp(void)
{
  removemenubar();
  wfreefont(mnufnt);
  wfreeblock(background);
  wsetmode(oldmode);
  mdeinit();
  clrscr();
  printf("Clean Exit..");
}

void ClipOn(void) // turn clipping on
{
  wclip(0,9,319,190); // Clip to boundaries
}

void ClipOff(void)  // turn clipping off (to draw to status bar, etc)
{
    wclip(0,0,319,199); // Clipping off
}

void UpdateStatusBar(int spec)
{
   ClipOff();  // turn off clipping to write to status bar
 if (spec==0)
  {
   wsetcolor(9);
   wbar(1,192,320,200);
   wtextcolor(38);
   wsetcolor(14);
   wfline(1,192,320,192);
   wgtprintf(10,194,mnufnt,"%i,%i",coordx,coordy);
   wtextcolor(89);
   wgtprintf(255,194,mnufnt,"Grid: %i",gridsnap);
  }

 if (spec!=0)
   {
     if (spec==1)
      {
       wsetcolor(9);
       wtextcolor(38);
       wbar(1,193,70,200);
       wgtprintf(10,194,mnufnt,"%i,%i",coordx,coordy);
      }
    }
 ClipOn();  // turn clipping back on
}

void MessageBar(char *string)
{
  ClipOff();
  wsetcolor(9);
  wtextcolor(141);
  wbar(75,193,254,200);
  wgtprintf(75,194,mnufnt,"%s",string);
  ClipOn();
}

void erasegrid(void)
{
 int i;

 moff();
 wsetcolor(0);
 for (i=-995+offsetx; i<=1000; i+=gridsnap)
 {
  if ((i>=0) && (i<=320))
   wline(i,11,i,190);
 }
 for (i=-995+offsety; i<=1000; i+=gridsnap)
 {
  if ((i>=1) && (i<=190))
   wline(1,i,320,i);
 }
 mon();
}


void setgrid(void)
{
 int i;

 moff();
 wsetcolor(25);
 for (i=-995+offsetx; i<1000; i+=gridsnap)
 {
  // do some quick clipping
  if ((i>=0) && (i<=320))
   wline(i,11,i,190);
  if (i==SCREENORIGINX+offsetx) {wsetcolor(22); wline(i,11,i,190); wsetcolor(25);}
 }
 for (i=-995+offsety; i<1000; i+=gridsnap)
 {
  // do some quick clipping
  if ((i>=10) && (i<=190))
   wline(1,i,320,i);
  if (i==SCREENORIGINY+offsety) {wsetcolor(22); wline(1,i,320,i); wsetcolor(25);}
 }
 mon();
}

int checkmouse(void)
{
 int diff;

  if ((but==1) && (wget_lctrl()==1))   // User is panning the display
   {
     SlideDisplay();
   }
  if ((but==1) && (my>11) && (my<190))
   {
    SelectObject();
    noclick();
    return 1;
   }
 if (oldmx!=mx)
  {
   if (oldmx>mx) {diff=oldmx-mx; coordx+=diff; xcoordx+=diff;}
   if (oldmx<mx) {diff=mx-oldmx; coordx-=diff; xcoordx-=diff;}
   oldmx=mx;
   UpdateStatusBar(COORDS);
  }
 if (oldmy!=my)
  {
   if (oldmy>my) {diff=oldmy-my; coordy+=diff; xcoordy+=diff;}
   if (oldmy<my) {diff=my-oldmy; coordy-=diff; xcoordy-=diff;}
   oldmy=my;
   UpdateStatusBar(COORDS);
  }
 if (but==2)
  {
   MessageBar("       Inserting Object.. ");
   insertmode=TRUE;
   noclick();
  }
 return 0;
}

void PromptBar(int length)
{
  wsetcolor(9);
  wbar(1,182,length,190);
  wsetcolor(1);
  wline(1,182,length,183);
  wsetcolor(13);
  wline(1,190,length,190);
}

void ClearPrompt(int length)
{
  wsetcolor(0);
  wbar(1,182,length,190);
  setgrid();
}

void PromptBar2(int length)
{
  wsetcolor(9);
  wbar(240,182,240+length,190);
  wsetcolor(1);
  wline(240,182,240+length,183);
  wsetcolor(13);
  wline(240,190,240+length,190);
}

void ClearPrompt2(int length)
{
  wsetcolor(0);
  wbar(240,182,240+length,190);
  setgrid();
}


int getnumvert(void)
{
 char *answer;
 int ans;
     PromptBar(120); wtextcolor(0); wtextbackground(9); wtexttransparent(0);
     wgtprintf(3,184,mnufnt,"Number Of Vertices: ");
     answer=(char *) malloc(3);
     strcpy(answer,"");
     wstring(100,183,answer,"1234567890",2);
     ans=atoi(answer);
     free(answer);
     ClearPrompt(120);
 return ans;
}

int getworldz(void)
{
 char *answer;
 int ans;
     PromptBar(130); wtextcolor(0); wtextbackground(9); wtexttransparent(0);
     wgtprintf(3,184,mnufnt,"Object's world Z: ");
     answer=(char *) malloc(5);
     strcpy(answer,"");
     wstring(90,183,answer,"1234567890-",4);
     ans=atoi(answer);
     free(answer);
     ClearPrompt(130);
 return ans;
}


int getvertz(void)
{
 char *answer;
 int ans;
     PromptBar(130); wtextcolor(0); wtextbackground(9); wtexttransparent(0);
     wgtprintf(3,184,mnufnt,"Vertex's Z Coord: ");
     answer=(char *) malloc(5);
     strcpy(answer,"");
     wstring(90,183,answer,"1234567890-",4);
     ans=atoi(answer);
     free(answer);
     ClearPrompt(130);
 return ans;
}

int choosevert(int choice1, int choice2)
{
 char *answer;
 int ans;
     PromptBar(130); wtextcolor(0); wtextbackground(9); wtexttransparent(0);
     wgtprintf(3,184,mnufnt,"Vertex %i or %i? ",choice1,choice2);
     answer=(char *) malloc(5);
     strcpy(answer,"");
     wstring(90,183,answer,"1234567890",4);
     ans=atoi(answer);
     free(answer);
     ClearPrompt(130);
 return ans;
}


void DumpObjects(void)
{
 int i,t,l;
 char ch;

 for (i=0; i<numobjects; i++)
  {
    printf("\r\nObject Dump Info:\r\n\r\n");
    printf("Object #%i\r\n",i);
    printf("Object World X,Y,Z: %i,%i,%i\r\n",objects[i].worldx,objects[i].worldy,objects[i].worldz);
    printf("Object Rotation: %i,%i,%i\r\n",objects[i].rotx,objects[i].roty,objects[i].rotz);
    printf("Object Scale: %i\r\n",objects[i].scale);
    printf("Object # Vertices: %i\r\n",objects[i].numvertices);
    for (t=0; t<objects[i].numvertices; t++)
     {
       printf("  Vrtx#%i ¯ %i,%i,%i  At Screen: %i,%i\r\n",t,objects[i].vertices[t].x,objects[i].vertices[t].y,objects[i].vertices[t].z,objects[i].vertices[t].sx,objects[i].vertices[t].sy);
     }
    printf("Object # Polygons: %i\r\n",objects[i].numpolygons);
    for (t=0; t<objects[i].numpolygons; t++)
     {
      printf("  Plygn#%i Includes These Vertices: %i, ",t,objects[i].polygons[t].edges);
      for (l=0; l<objects[i].polygons[t].edges; l++)
       {
	printf("%i,",objects[i].polygons[t].vrtxnum[l]);
       }
      printf(" %i\r\n",objects[i].polygons[t].color);
     }
    printf("BPSR: %i\r\n",objects[i].backplane);
  }
 ch=getch(); ch++;
}

void GetAllPolygons(void)
{
 int diff;
 int verthit;
 int firsthit=50, secondhit=50;  // Set to impossible vertex value (>20)
 char ch;

 wsetcolor(28);
 PromptBar(70);
 wgtprintf(3,184,mnufnt,"On Polygon#%i ",onpolygon);

do
 {
  if ((but==1) && (my>11) && (my<190))  // User clicked button one
   {
    if (onvertex<9) verthit=CheckForHit();    // See if we hit a vertex
     if (verthit!=-1)
       {
	 if (firsthit==50)
	  {firsthit=verthit;}
	 else
	  {secondhit=verthit;}
       }
     noclick();
   }
 if (secondhit!=50)  // Connect the two selected vertices and reset variables
  {
    objects[onobject].polygons[onpolygon].vrtxnum[onvertex]=firsthit;
    onvertex++;
    objects[onobject].polygons[onpolygon].vrtxnum[onvertex]=secondhit;
    wsetcolor(100);
    moff();
    wline(objects[onobject].vertices[firsthit].sx,objects[onobject].vertices[firsthit].sy,objects[onobject].vertices[secondhit].sx,objects[onobject].vertices[secondhit].sy);
    wsetcolor(195);
    wputpixel(objects[onobject].vertices[firsthit].sx,objects[onobject].vertices[firsthit].sy);
    wputpixel(objects[onobject].vertices[secondhit].sx,objects[onobject].vertices[secondhit].sy);
    mon();
    firsthit=secondhit;
    secondhit=50;
    edgesmade++;
  }
 if (but==2)  // Button two, next polygon
  {
   objects[onobject].polygons[onpolygon].edges=edgesmade;
   objects[onobject].polygons[onpolygon].color=11;
   objects[onobject].numpolygons++;
   edgesmade=0;
   onvertex=0;
   if (onpolygon<MAXPOLYGONS) onpolygon++;
   firsthit=50;
   secondhit=50;
   ClearPrompt(70);
   PromptBar(70);    // Update Status bar
   wgtprintf(3,184,mnufnt,"On Polygon#%i",onpolygon);
   ClearPrompt2(40);
   noclick();
  }
 if (but==3)   // Button three, or both 1&2 at same time = alldone
  {
    all_done_w_polygons=TRUE;
    ClearPrompt(70);
    ClearPrompt2(40);
    noclick();
  }
 if (kbhit()!=0)   // Exit from polygons if user hit "q"
  {
   ch=getch();
   if (ch==27) { all_done_w_polygons=TRUE; ClearPrompt(70); ClearPrompt2(40);}
  }
 if (oldmx!=mx)  // Moved mouse, update coords and display  (x dir)
  {
   if (oldmx>mx) {diff=oldmx-mx; coordx+=diff; xcoordx+=diff;}
   if (oldmx<mx) {diff=mx-oldmx; coordx-=diff; xcoordx-=diff;}
   oldmx=mx;
   UpdateStatusBar(COORDS);
  }
 if (oldmy!=my)  // Moved mouse, update coords and display  (y dir)
  {
   if (oldmy>my) {diff=oldmy-my; coordy+=diff; xcoordy+=diff;}
   if (oldmy<my) {diff=my-oldmy; coordy-=diff; xcoordy-=diff;}
   oldmy=my;
   UpdateStatusBar(COORDS);
  }
 } while (all_done_w_polygons==FALSE);
}

int CheckForHit(void)  // see if the user pressed button over a vertex
{
  int i;
  int hitone=0;
  int vertexhit=-1;
  int vertexhit2=-1;

  for (i=0; i<objects[onobject].numvertices; i++)
   {
    if ((objects[onobject].vertices[i].x==coordx) && (objects[onobject].vertices[i].y==coordy))
      {
       hitone++;
       if (vertexhit==-1) {vertexhit=i;} else {vertexhit2=i;}
       PromptBar2(40);
       wgtprintf(243,184,mnufnt,"Vrtx #%i",vertexhit);
       sound(800);
       delay(50);
       nosound();
      }
    }
 if (hitone>1)   // Two vertexes on same x,y, different z.. picks one
    {
      return choosevert(vertexhit,vertexhit2);
    }
 if (hitone==1) {return vertexhit;}
 else {return -1;}  // RETURN -1 IF HIT NOTHING!! (so not to interfere vith vertex #0)
}


int ExportObject(int objectnum)  // Export Object into an object text file (NOT A WORLD FILE)
{
  FILE *objectfile;
  int counter,counter2;
  char *tempstring;
  char *efilename;

  moff();
  background=wnewblock(0,0,319,199);
  mon();
  filefont=mnufnt;
  efilename=wfilesel("odf","Export Object",10,20,background);
  if ((objectfile=fopen(efilename,"w"))==NULL) return 0;
  fputs("*** Object Definition File ***\n",objectfile);
  fputs("*** Created With World Design v ",objectfile);
  fputs(version,objectfile);
  fputs("\n\n",objectfile);
  fputs("1,  * Number Of Objects In File\n\n",objectfile);
  fputs("* Object Data:\n\n",objectfile);
   itoa(objects[objectnum].numvertices,tempstring,10);
  fputs(tempstring,objectfile);
  fputs(",   * Number Of Vertices In Object\n\n",objectfile);
  fputs(" * Vertices For Object:\n\n",objectfile);
  for (counter=0; counter<objects[objectnum].numvertices; counter++)
   {
     fputs("          ",objectfile);
      itoa(objects[objectnum].vertices[counter].x,tempstring,10);
     fputs(tempstring,objectfile);
     fputs(",",objectfile);
      itoa(objects[objectnum].vertices[counter].y,tempstring,10);
     fputs(tempstring,objectfile);
     fputs(",",objectfile);
      itoa(objects[objectnum].vertices[counter].z,tempstring,10);
     fputs(tempstring,objectfile);
     fputs(", * Vertex #",objectfile);
      itoa(counter,tempstring,10);
     fputs(tempstring,objectfile);
     fputs("\n",objectfile);
    }
 fputs("\n",objectfile);
  itoa(objects[objectnum].numpolygons,tempstring,10);
 fputs(tempstring,objectfile);
 fputs(",   * Number Of Polygons In Object\n\n",objectfile);
 fputs(" * Polygons For Object:\n\n",objectfile);
  for (counter=0; counter<objects[objectnum].numpolygons; counter++)
   {
     fputs("          ",objectfile);
     itoa(objects[objectnum].polygons[counter].edges,tempstring,10);
     fputs(tempstring,objectfile);
     fputs(",",objectfile);
     for (counter2=0; counter2<objects[objectnum].polygons[counter].edges; counter2++)
      {
       itoa(objects[objectnum].polygons[counter].vrtxnum[counter2],tempstring,10);
       fputs(tempstring,objectfile);
       fputs(",",objectfile);
       }
     itoa(objects[objectnum].polygons[counter].color,tempstring,10);
     fputs(tempstring,objectfile);
     fputs(",\n",objectfile);
    }
 fputs("\n\n           ",objectfile);
 itoa(objects[objectnum].backplane,tempstring,10);
 fputs(tempstring,objectfile);
 fputs(",        * Backplane Surface Removal (1=Yes, 0=No)\n",objectfile);
 fclose(objectfile);
 return 1;  // Success
}

int ImportObject(void)
{
FILE *f;
char *ifilename;
int tempnumobjects;
int objnum, vertnum, polynum;

  moff();
  background=wnewblock(0,0,319,199);
  mon();
  filefont=mnufnt;
  if ((ifilename=wfilesel("odf","Import Object",10,20,background))==NULL) return(0);

  numobjects++;            // Add this object to the list
  onobject=numobjects-1;

  f=fopen(ifilename,"rt");
  tempnumobjects=getnumber(f);
  if (tempnumobjects>1) return(2); // Too many objects in file
  for (objnum=0; objnum<tempnumobjects; objnum++) {
    objects[onobject].numvertices=getnumber(f);
    for (vertnum=0; vertnum<objects[onobject].numvertices; vertnum++) {
      objects[onobject].vertices[vertnum].x=getnumber(f);
      objects[onobject].vertices[vertnum].sx=SCREENORIGINX+objects[onobject].vertices[vertnum].x;
       objects[onobject].vertices[vertnum].sxo=SCREENORIGINX+objects[onobject].vertices[vertnum].x;
       objects[onobject].vertices[vertnum].sx+=offsetx; // give it the offset
      objects[onobject].vertices[vertnum].y=getnumber(f);
      objects[onobject].vertices[vertnum].sy=SCREENORIGINY+objects[onobject].vertices[vertnum].y;
      objects[onobject].vertices[vertnum].syo=SCREENORIGINY+objects[onobject].vertices[vertnum].y;
       objects[onobject].vertices[vertnum].sy+=offsety; // give it the offset
      objects[onobject].vertices[vertnum].z=getnumber(f);
    }
    objects[onobject].numpolygons=getnumber(f);
    for (polynum=0; polynum<objects[onobject].numpolygons; polynum++) {
      objects[onobject].polygons[polynum].edges=getnumber(f);
      for (vertnum=0; vertnum<objects[onobject].polygons[polynum].edges; vertnum++) {
	objects[onobject].polygons[polynum].vrtxnum[vertnum]=getnumber(f);
      }
      objects[onobject].polygons[polynum].color=getnumber(f);
    }
    objects[onobject].backplane=getnumber(f);
  }
  fclose(f);
  MoveObject(onobject);
  return(0);
}

int getnumber(FILE *f)
{
  char ch;
  int sign=1;
  int num;


  num=0;
  if ((ch=nextchar(f))=='-') {
     sign=-1;
     ch=nextchar(f);
  }
  while (isdigit(ch)) {
    num=num*10+ch-'0';
    ch=nextchar(f);
  }
  return(num*sign);
}

char nextchar(FILE *f)
{
  char ch;

  while(!feof(f)) {
    while(isspace(ch=fgetc(f)));
    if (ch=='*')
       while((ch=fgetc(f))!='\n');
	 else return(ch);
  }
  return(0);
}

void UpdateScreen(int highlight, int color)  // Redraws all of the objects
{
 int objcnt, vrtcnt, polycnt, edgecnt;
 int point1, point2;
 int linesmade;
 int diffx, diffy;

setgrid();  // Put down the grid first, if there is one
putorigin(); // then the world origin;

// Then the objects:
for (objcnt=0; objcnt<numobjects; objcnt++)  // Cycle through all objects
{
 // first the local origin in orange
 diffx=SCREENORIGINX-objects[objcnt].worldx;
 diffy=SCREENORIGINY-objects[objcnt].worldy;
 wsetcolor(LOCALORIGINCOLOR);
 wputpixel(diffx+offsetx,diffy+offsety);

 for (polycnt=0; polycnt<objects[objcnt].numpolygons; polycnt++)
 {
  if (objcnt!=highlight) wsetcolor(objects[objcnt].polygons[polycnt].color);
  if (objcnt==highlight) wsetcolor(color);// Draw edges in requested color of hightlighted object
  if (objcnt==selected_object) wsetcolor(HIGHTLIGHTCOLOR);
  linesmade=0;
   for (edgecnt=0; edgecnt<objects[objcnt].polygons[polycnt].edges; edgecnt++)
   {
    point1=objects[objcnt].polygons[polycnt].vrtxnum[linesmade];
    linesmade++;
    if (linesmade==objects[objcnt].polygons[polycnt].edges) linesmade=0;
    point2=objects[objcnt].polygons[polycnt].vrtxnum[linesmade];
    wline(objects[objcnt].vertices[point1].sx,objects[objcnt].vertices[point1].sy,objects[objcnt].vertices[point2].sx,objects[objcnt].vertices[point2].sy);
   }  // End edge countdown
 } // End polygon countdown
 // put vertices here to you can see them
 wsetcolor(195);
 for (vrtcnt=0; vrtcnt<objects[objcnt].numvertices; vrtcnt++)
 {
  wputpixel(objects[objcnt].vertices[vrtcnt].sx,objects[objcnt].vertices[vrtcnt].sy);
 }
} // End object countdown
} // End routine

void EraseObject(int objectnum)
{
 int vrtcnt, polycnt, edgecnt;
 int point1, point2;
 int linesmade;
 int diffx, diffy;

 diffx=SCREENORIGINX-objects[objectnum].worldx;
 diffy=SCREENORIGINY-objects[objectnum].worldy;
 wsetcolor(0);
  // clear the local origin
 wputpixel(diffx+offsetx,diffy+offsety);

 for (vrtcnt=0; vrtcnt<objects[objectnum].numvertices; vrtcnt++)
 {
  wputpixel(objects[objectnum].vertices[vrtcnt].sx,objects[objectnum].vertices[vrtcnt].sy);
 }
 for (polycnt=0; polycnt<objects[objectnum].numpolygons; polycnt++)
 {
  wsetcolor(0);
  linesmade=0;
   for (edgecnt=0; edgecnt<objects[objectnum].polygons[polycnt].edges; edgecnt++)
   {
    point1=objects[objectnum].polygons[polycnt].vrtxnum[linesmade];
    linesmade++;
    if (linesmade==objects[objectnum].polygons[polycnt].edges) linesmade=0;
    point2=objects[objectnum].polygons[polycnt].vrtxnum[linesmade];
    wline(objects[objectnum].vertices[point1].sx,objects[objectnum].vertices[point1].sy,objects[objectnum].vertices[point2].sx,objects[objectnum].vertices[point2].sy);
   }  // End edge countdown
 } // End polygon countdown
}



void MoveObject(int objectnum)
{
 int diff, i;

  noclick();
  moff(); // Turn off mouse cursor so it doesn't get in the way
  if ((objects[objectnum].worldx==0) && (objects[objectnum].worldy==0))
   {
    GotoOrigin(); // goto the origin
   }
  else
   {
    GotoCoords(objects[objectnum].worldx,objects[objectnum].worldy,objectnum);
   }
  msetbounds(0,9,319,190);
  UpdateScreen(objectnum,HIGHTLIGHTCOLOR);

MessageBar("    Moving Object...");
do
{
 if (oldmx!=mx)
  {
   if (oldmx>mx)
    {EraseObject(objectnum);
      diff=oldmx-mx;
      coordx+=diff;
      xcoordx+=diff;
      for (i=0; i<objects[objectnum].numvertices; i++) // move all vertices
       {objects[objectnum].worldx=coordx;
	objects[objectnum].vertices[i].sx-=diff;
	objects[objectnum].vertices[i].sxo-=diff;
       }
      UpdateScreen(objectnum, HIGHTLIGHTCOLOR);
    }
   if (oldmx<mx)
    {EraseObject(objectnum);
     diff=mx-oldmx;
     coordx-=diff;
     xcoordx-=diff;
      for (i=0; i<objects[objectnum].numvertices; i++) // move all vertices
       {objects[objectnum].worldx=coordx;
	objects[objectnum].vertices[i].sx+=diff;
	objects[objectnum].vertices[i].sxo+=diff;
       }
      UpdateScreen(objectnum, HIGHTLIGHTCOLOR);
    }
   oldmx=mx;
   UpdateStatusBar(COORDS);
  }
 if (oldmy!=my)
  {
   if (oldmy>my)
    { EraseObject(objectnum);
      diff=oldmy-my;
      coordy+=diff;
      xcoordy+=diff;
      for (i=0; i<objects[objectnum].numvertices; i++) // move all vertices' x
       {objects[objectnum].worldy=coordy;
	objects[objectnum].vertices[i].sy-=diff;
	objects[objectnum].vertices[i].syo-=diff;
       }
      UpdateScreen(objectnum, HIGHTLIGHTCOLOR);
    }
   if (oldmy<my)
    { EraseObject(objectnum);
      diff=my-oldmy;
      coordy-=diff;
      xcoordy-=diff;
      for (i=0; i<objects[objectnum].numvertices; i++) // move all vertices' y
       {objects[objectnum].worldy=coordy;
	objects[objectnum].vertices[i].sy+=diff;
	objects[objectnum].vertices[i].syo+=diff;
       }
      UpdateScreen(objectnum, HIGHTLIGHTCOLOR);
    }
   oldmy=my;
   UpdateStatusBar(COORDS);
  }
  if (but==2)  // scootch over to the news coords
   {
    GotoCoords(objects[objectnum].worldx,objects[objectnum].worldy,objectnum);
    noclick();
   }
 } while (but!=1); // end while
 noclick();
 UpdateScreen(-1,0);
 msetbounds(1,1,320,190);
 mon();  // turn mouse back on
 MessageBar("");
 GotoCoords(objects[objectnum].worldx,objects[objectnum].worldy,-1);
} // end routine

void eraseorigin(void)  // erase the world origin
{
 wsetcolor(0);
 wputpixel(SCREENORIGINX+offsetx,SCREENORIGINY+offsety); // then the origin
 wcircle(SCREENORIGINX+offsetx,SCREENORIGINY+offsety,5);
}

void putorigin(void)  // put down the origin
{
 // first do some quick clipping so we don't waste our time
 if ( ((SCREENORIGINX+offsetx) > 0) && ((SCREENORIGINY+offsetx) < 320) &&
      ((SCREENORIGINX+offsety) > 10) && ((SCREENORIGINY+offsety) < 190))
  {
   wsetcolor(17);
   wputpixel(SCREENORIGINX+offsetx,SCREENORIGINY+offsety);  // Plot world origin 0,0
   wcircle(SCREENORIGINX+offsetx,SCREENORIGINY+offsety,5);
  }
}

void EraseAllObjects(void)  // erase all the objects at their current x,y
{
 int obj;

 for (obj=0; obj<numobjects; obj++)  // cycle through all objects
  {
   EraseObject(obj);
  }
}

void SlideDisplay(void)   // Pan display with mouse
{
 int diff;
 int obj,vert;

MessageBar("   Panning Display...");
moff();
do
{
 if (oldmx!=mx)   // change in x direction
  {
   if (oldmx>mx) // Moved mouse left
    {
     diff=oldmx-mx;
     eraseorigin();
     EraseAllObjects();
     erasegrid();
     offsetx-=diff;
     xcoordx+=diff;
     for (obj=0; obj<numobjects; obj++)  // cycle through all objects
     {
      for (vert=0; vert<objects[obj].numvertices; vert++)// Cycle through vrts
      {
      objects[obj].vertices[vert].sx-=diff;
      }
     }
    }
   if (oldmx<mx)  // Moved mouse right
    {
      diff=mx-oldmx;
      eraseorigin();
      EraseAllObjects();
      erasegrid();
      offsetx+=diff;
      xcoordx-=diff;
     for (obj=0; obj<numobjects; obj++)  // cycle through all objects
     {
      for (vert=0; vert<objects[obj].numvertices; vert++)// Cycle through vrts
      {
      objects[obj].vertices[vert].sx+=diff;
      }
     }
    }
   oldmx=mx;
   UpdateScreen(-1,0);  // show objects
   coordx=xcoordx+offsetx;
   UpdateStatusBar(COORDS);
  }
 if (oldmy!=my)  // change in y direction
  {
   if (oldmy>my)  // Moved mouse up
    {
     diff=oldmy-my;
     eraseorigin();
     EraseAllObjects();
     erasegrid();
     offsety-=diff;
     xcoordy+=diff;
     for (obj=0; obj<numobjects; obj++)  // cycle through all objects
     {
      for (vert=0; vert<objects[obj].numvertices; vert++)// Cycle through vrts
      {
      objects[obj].vertices[vert].sy-=diff;
      }
     }
    }
   if (oldmy<my)   // Moved mouse down
    {
      diff=my-oldmy;
      eraseorigin();
      EraseAllObjects();
      erasegrid();
      offsety+=diff;
      xcoordy-=diff;
     for (obj=0; obj<numobjects; obj++)  // cycle through all objects
     {
      for (vert=0; vert<objects[obj].numvertices; vert++)// Cycle through vrts
      {
      objects[obj].vertices[vert].sy+=diff;
      }
     }
    }
   oldmy=my;
   UpdateScreen(-1,0);  // show objects
   coordy=xcoordy+offsety;
   UpdateStatusBar(COORDS);
  }
} while (but==1);  // Continue panning until user releases button 1
mon();
MessageBar("");
}

void RefreshObjectsSXSY(void)
{
/* This routine is used when the objects sx and sy values are to be moved
by a routine such as ShowOrigin() or GotoCoords() and need to be refreshed.
									   */

  int obj,vert;

     for (obj=0; obj<numobjects; obj++)  // cycle through all objects
     {
//      EraseObject(obj);
      for (vert=0; vert<objects[obj].numvertices; vert++)// Cycle through vrts
      {
       objects[obj].vertices[vert].sx=objects[obj].vertices[vert].sxo+offsetx;
       objects[obj].vertices[vert].sy=objects[obj].vertices[vert].syo+offsety;
      }
     }
}

void GotoCoords(int x, int y, int highlight)
{
  moff();
  EraseAllObjects();
  eraseorigin();
  erasegrid();
  offsetx=x;
  offsety=y;
  coordx=x;
  coordy=y;
  xcoordx=x;
  xcoordy=y;
  msetxy(SCREENORIGINX,SCREENORIGINY);
  mx=SCREENORIGINX;
  my=SCREENORIGINY;
  oldmx=SCREENORIGINX;
  oldmy=SCREENORIGINY;
  RefreshObjectsSXSY();  // Refresh the objects screen pointers
  UpdateScreen(highlight,HIGHTLIGHTCOLOR);
  UpdateStatusBar(COORDS);
  mon();
}

void GotoOrigin(void) // Goto origin
{
  moff();
  EraseAllObjects();
  eraseorigin();
  erasegrid();
  offsetx=0;
  offsety=0;
  coordx=0;
  coordy=0;
  xcoordx=0;
  xcoordy=0;
  msetxy(SCREENORIGINX,SCREENORIGINY);
  mx=SCREENORIGINX;
  my=SCREENORIGINY;
  oldmx=SCREENORIGINX;
  oldmy=SCREENORIGINY;
  RefreshObjectsSXSY();  // Refresh the objects screen pointers
  UpdateScreen(-1,0);
  UpdateStatusBar(COORDS);
  mon();
}

int CheckForVert(int scx, int scy, int length, int width)
{
 /* check for vertices in a box defined by length,width starting at
    coordinates scx, scy. It will run through all of the objects vertices
    list to check for a hit.   				*/
 int x,y;
 int obj, vert;

 length+=scx;
 width+=scy;

 for (obj=0; obj<numobjects; obj++) // go through objects
  {
   for (vert=0; vert<objects[obj].numvertices; vert++)  // through vertices
    {
     for (x=scx; x<length; x++) // through x direction of box (length)
      {
       for (y=scy; y<width; y++) // through y direction of box (width)
	{
	 if ((objects[obj].vertices[vert].sx==x) && (objects[obj].vertices[vert].sy==y))
	 return obj;  // we have a hit in object "obj"
	}
      }
    }
  }
 return -1;  // no hit, return a -1
}


void SelectObject(void)  // selects an object by boxing in a vertex
{
 int startx, starty, endx, endy;
 int changed;
 int diff;
 int objhit; // object that will contain first virtices found to be boxed in by rectangle

 startx=mx; starty=my; endx=mx; endy=my;
 do
  {
   if (mx<startx) msetxy(startx,my);
   if ((oldmx!=mx) && (mx>startx))
   {
    moff();
    wsetcolor(0);  // erase old rectangle
    wrectangle(startx,starty,endx,endy);
    mon();
    if (oldmx>mx) {diff=oldmx-mx; coordx+=diff; xcoordx+=diff; endx-=diff;}
    if (oldmx<mx) {diff=mx-oldmx; coordx-=diff; xcoordx-=diff; endx+=diff;}
    oldmx=mx;
    changed=1;
   }
  if (my<starty) msetxy(mx,starty);
  if ((oldmy!=my) && (my>starty))
   {
    moff();
    wsetcolor(0);  // erase old rectangle
    wrectangle(startx,starty,endx,endy);
    mon();
    if (oldmy>my) {diff=oldmy-my; coordy+=diff; xcoordy+=diff; endy-=diff;}
    if (oldmy<my) {diff=my-oldmy; coordy-=diff; xcoordy-=diff; endy+=diff;}
    oldmy=my;
    changed=1;
   }
   if (changed==1)
    {
     moff();
     wsetcolor(50); // draw new rectangle
     wrectangle(startx,starty,endx,endy);
     changed=0;
     UpdateStatusBar(COORDS);
     UpdateScreen(-1,0);
     mon();
    }
  }
  while (but==1);

  objhit=CheckForVert(startx, starty, endx-startx, endy-starty); // check the rectangle for a hit
  if (objhit!=-1) selected_object=objhit; // if hit, make object selected

  moff();
  wsetcolor(0); // get rid of ractangle
  wrectangle(startx,starty,endx,endy);
  UpdateScreen(selected_object,HIGHTLIGHTCOLOR);
  mon();
}