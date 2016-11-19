/***************************************************************************
 *                                  Arena                                  *
 *                           [View Control Module]                         *
 * Filename: VIEWCTRL.C                                                    *
 * Version: .1à                                                            *
 * Creation Date: 08/31/94                                                 *
 *                                                                         *
 * Copyright (c) 1994 Alternate View S/W                                   *
 * Coding By: Shannon Weyrick                                              *
 ***************************************************************************/

#include <math.h>
#include <conio.h>
#include <stdio.h>
#include <dos.h>
#include <bcd.h>
#include "lowlev.h"
#include "fix.h"
#include "arena.h"
#include "poly.h"
#include "view.h"
#include "drawpoly.h"
#include "loadpoly.h"
#include "screen.h"
#include "loadpoly.h"
#include "pcx.h"
#include "viewctrl.h"


// *********************** Structure Descriptors *********************** //

world_type world;            // World descriptor
view_type curview;           // View descriptor
pcx_struct sky;              // Sky PCX

// ****************** C++ Object Instance Declerations ***************** //
View winview;                // View object  (CPP)
Pcx skypic;                  // Pcx object   (CPP)

// ***************************  Variables ****************************** //

int DEBUG_MODE=0;              // Whether debug mode is on or off
int numframes=0;               // Number of frames that have been drawn
float viewx, viewz;            // Actual position of view

// ************************* View Variables **************************** //

unsigned char *scrbuf;  // Pointer to the double buffer
int oldmode;            // Old video mode pointer
int skyoffset=20;          // Offset to scroll sky

void SetupGfxSystem(void)
{
  oldmode=*(int *)MK_FP(0x40,0x49); // Save old video mode
  setgmode(0x13);                   // Set new video mode (320x200x256)
  scrbuf=new unsigned char[64000];  // Create offscreen buffer
}

void ShutdownGfxSystem(void)
{
  setgmode(oldmode);                 // Set origin video mode (text)
}

void InitViewSystem(int db)
{
  DEBUG_MODE=db;  // Whether debug is on or off, passed from main()
  // Load world
  int polycount=loadpoly(&world,"arena1.pdb");
  // Load palette
  LoadPalette("arnamain.pal");
  // Load sky
  skypic.load("cloud2.pcx",&sky);
  // Set initial position and oriention of view
  curview.copx=0;
  curview.copy=-30;
  curview.copz=0;
  curview.xangle=0;
  curview.yangle=0;
  curview.zangle=0;
  viewx=0;
  viewz=0;
  // Set viewport parameters
  winview.setview(XORIGIN,YORIGIN,WIND_X,WIND_Y,WIND_X2,WIND_Y2,
		  FOCAL_DISTANCE,GROUNDCOLOR,SKYCOLOR,scrbuf);
  // Establish world database
  winview.setworld(world,polycount);
}

void Show_Palette(void)
{
 int x,y;
 int cnt=0;
 /************************* DISPLAY PALETTE **************************/
 // Shows a little palette in the top left corner of the screen
 for (x=0; x<18; x+=2)
  {
  for (y=0; y<56; y+=2)
    {
      Plot_Pixel(x,y,cnt,scrbuf);
      Plot_Pixel(x+1,y,cnt,scrbuf);
      Plot_Pixel(x,y+1,cnt,scrbuf);
      Plot_Pixel(x+1,y+1,cnt,scrbuf);
      cnt++;
    }
  }
}

void UpdateView(void)
{
 OverlaySky(sky.image,scrbuf,skyoffset);        // Display sky  (lowlev.c)
 winview.display(curview,1);          // Display the current view
 if (DEBUG_MODE) Show_Palette();      // Show palette if in debug mode
 putwindow(WIND_X,WIND_Y,WIND_X2,WIND_Y2,scrbuf);  // Put buffer onto the screen
 numframes++;                    // Add to number of frames drawn
}

int ReturnFrames(void)
{
  return(numframes);    // Return to caller number of frames that have been
			// drawn
}


int GetAllInput(void)
{
/* This functions, right now, gets input with getch().
   Plan on having custom keyboard interput handler, which will be checked
   from this routine and will be in a seperate module. That routine will
   update a structure, and based on the structure this routine will loop
   though all the game objects (whether local player, remote player, or
   computer) and update their object data structure.                    */
 char ch;
 float dz,dx;
 int SPEED=15;

 dx=0;
 dz=0;

/* curview.yangle++;   // Spin!
 skyoffset++;
 if (curview.yangle>NUMBER_OF_DEGREES-1) curview.yangle=0;
 if (curview.yangle<0) curview.yangle=NUMBER_OF_DEGREES-1;
 if (skyoffset<1) skyoffset+=320;
 if (skyoffset>319) skyoffset-=320;  */


 if (kbhit()!=0)
  {
 ch=getch();
 if (ch=='j') {curview.yangle++; skyoffset++;}
 if (ch=='k') {curview.yangle--; skyoffset--;}
  if (curview.yangle>NUMBER_OF_DEGREES-1) curview.yangle=0;
  if (curview.yangle<0) curview.yangle=NUMBER_OF_DEGREES-1;
  if (skyoffset<1) skyoffset+=320;
  if (skyoffset>319) skyoffset-=320;
 if (ch=='i')  // forward
   {
    dz=cos(.024543692*curview.yangle);
    dx=-sin(.024543692*curview.yangle);
    viewx+=dx*SPEED;
    viewz+=dz*SPEED;
   }
 if (ch=='m')  // backward
    {
    dz=-cos(.024543692*curview.yangle);
    dx=sin(.024543692*curview.yangle);
    viewx+=dx*SPEED;
    viewz+=dz*SPEED;
    }
    bcd tempx=bcd(viewx,0);   // Round off viewx
    bcd tempz=bcd(viewz,0);   // Round off viewz
    curview.copx=(int)real(tempx);
    curview.copz=(int)real(tempz);
  if (DEBUG_MODE)
   {
    gotoxy(1,24);
    printf("x:%i,z:%i,ya:%i        ",curview.copx,curview.copz,curview.yangle);
   }
 if (ch==27) return(QUIT);
 }
  return(0);
}

