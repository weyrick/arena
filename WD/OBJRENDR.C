// Object Renderer Routine
#include <stdio.h>
#include <dos.h>
#include <conio.h>
#include <stdlib.h>
#include "screen.h"
#include "poly.h"
#include "drawpoly.h"
#include "wd.h"
#include "objrendr.h"


void RenderObject(world_type world, unsigned char far *screen_buffer,int objectnum)
{
  int scalefactor=1;
  int zdistance=600;
  int done=0;
  int key;
  float xangle=0, yangle=0, zangle=0;
  float xrot=0, yrot=0, zrot=0;

  while (!done)
  {
    clrwin(0,0,WIND_WIDTH,WIND_HEIGHT,screen_buffer);
    inittrans();
    scale(scalefactor);
    rotate(xangle,yangle,zangle);


    // Rotate object the increment value
    xangle+=xrot;
    yangle+=yrot;
    zangle+=zrot;

    // Check for 256 degree wrap around

		if (xangle>255) xangle=0;
		if (xangle<0) xangle=255;
		if (yangle>255) yangle=0;
		if (yangle<0) yangle=255;
		if (zangle>255) zangle=0;
		if (zangle<0) zangle=255;

    // Translate object:
		translate(0,0,zdistance);

    // Transform object with matrix:
		transform(&world.obj[objectnum]);

    // Do perspective projection:
		project(&world.obj[objectnum],400,XORIGIN,YORIGIN);

    // Draw the object:
		draw_object(world.obj[objectnum],screen_buffer);

    // Put it on the video display:
		putwindow(0,0,320,200,screen_buffer);

    // Watch for user input:
		if (kbhit()) {  // If input received....
			key=getch();  // Read the key code
			switch(key) {
				case 27:  done=1;  // ESC Key, exit

				case 55:

	  // "7": Speed up x rotation

					xrot++;
					break;

				case 52:

	  // "4": Stop x rotation

					xrot=0;
					break;

				case 49:

	  // "1": Slow down x rotation

					xrot--;
					break;

				case 56:

	  // "8": Speed up y rotation

					yrot++;
					break;

				case 53:

	  // "5": Stop y rotation

					yrot=0;
					break;

				case 50:

	  // "2": Slow down y rotation

					yrot--;
					break;

				case 57:

	  // "9": Speed up z rotation

					zrot--;
					break;

				case 54:

	  // "6": Stop z rotation

					zrot=0;
					break;

				case 51:

	  // "3": Slow down z rotation

					zrot--;
					break;

				case '+':

	  // "+": Increase distance

					zdistance+=5;
					break;

				case '-':

	  // "-": Decrease distance

					if (zdistance>495) zdistance-=5;
					break;
			}
		}
	}


}
