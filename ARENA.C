/***************************************************************************
 *                                  Arena                                  *
 *                               [Main Module]                             *
 * Filename: ARENA.C                                                       *
 * Version: .1à                                                            *
 * Creation Date: 08/31/94                                                 *
 *                                                                         *
 * Copyright (c) 1994 Alternate View S/W                                   *
 * Coding By: Shannon Weyrick                                              *
 ***************************************************************************/

#include <stdio.h>
#include <dos.h>
#include <bios.h>
#include <conio.h>
#include <stdlib.h>
#include <mem.h>
#include <math.h>
#include "timer.h"
#include "poly.h"
#include "view.h"
#include "drawpoly.h"
#include "loadpoly.h"
#include "screen.h"
#include "viewctrl.h"
#include "lowlev.h"
#include "arena.h"

int DEBUG=FALSE;       // Debug mode, 1=On, 0=Off
Timer FrameTime;       // Define Timer class variable

void ParseCommandLine(int argc, char *argv[]);  // Parse command line

// ********************* Main Program Entry Point ********************* //
void main(int argc, char *argv[])
{
 int gameover=0;                  // Is game over?
 char ch;			  // temp key variable
 float endframetime;
 int numframesdrawn;

  ParseCommandLine(argc, argv);   // Parse the command line arguments
  SetupGfxSystem();               // Initialize the graphics system
  InitViewSystem(DEBUG);          // Initialize the view system

  FrameTime.start();		  // Start the timer

 do    // Start main game loop
  {
   UpdateView();             // Update the view
   if (GetAllInput()==QUIT)  // Get input from everywhere (local,serial,etc)
      gameover=TRUE;
  } while (gameover==FALSE);       // Keep looping until game is over

  FrameTime.stop(); 		   // Stop the timer
  endframetime=FrameTime.time();   // Get the value of timer
  numframesdrawn=ReturnFrames();   // Get the # frames drawn since start

  ShutdownGfxSystem();             // Shutdown the graphics system

  printf("Eft: %f\r\n",endframetime); // Print ending frametime value
  printf("Frd: %i\r\n",numframesdrawn);
  printf("Fps: %f\r\n",numframesdrawn/endframetime);
  ch=getch(); ch++;
}


void ParseCommandLine(int argc, char *argv[])
{
  // Parses command line
  if (argc>1)
    {
      if (*argv[1]=='d') DEBUG=TRUE;
    }
}
