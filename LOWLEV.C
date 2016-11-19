// Low Level Routines

#include <alloc.h>
#include <dos.h>
#include <stdlib.h>
#include <stdio.h>
#include <io.h>
#include "mem.h"
#include "screen.h"
#include "arena.h"
#include "lowlev.h"



void Plot_Pixel(int x, int y, unsigned char color, unsigned char *buf)
{
  // Plot pixel fast using binary shifting
 buf[ ((y<<8) + (y<<6)) + x] = color;
}

void LoadPalette(char *pfilename)
{
  unsigned char palette[768];
  FILE *pfile;

  if ((pfile=fopen(pfilename,"rb")) == NULL) exit(0); // Open palette file
  fread(palette,1,768,pfile);  // Read in palette
  fclose(pfile);                       // Close the file

  setpalette(palette,0,256);           // Set the palette (screen.asm)
}

void OverlaySky(unsigned char far *skyimage, unsigned char *scrbuf, int offset)
{
/* This routine puts the first 32000 bytes of a pcx file on the screen.
  This should be the first half of a pcx file.. for the sky.
  NOTE: This routine should be called BEFORE the view has been updated..*/
 int counter;

 // Clear the viewport
 clrwin(0,0,320,170,scrbuf);
 // Copy to memory
// memcpy(scrbuf,skyimage,32000);   // Copy the memory

 for (counter=0; counter<100; counter++)
  {
    memcpy(scrbuf+offset,skyimage,320-offset);
    memcpy(scrbuf,skyimage+320-offset,offset);
    skyimage+=320;
    scrbuf+=320;
  }

}