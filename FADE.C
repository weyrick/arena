/* Portions of this program are based on the book "Bit-Mapped-Graphics" by 
Steve Rimmer. excelent book for learning picture formats.  Way too fluffy in 
places way too technical others pretty decent overall though.  Once              
you understand the program, try fading from one screen into another directly.
*/

#include<stdio.h>
#include <alloc.h>
#include <dos.h>


typedef struct {
	char    manufacturer;
	char    version;
	char    encoding;
	char    bits_per_pixel;
	int     xmin,ymin;
	int     xmax,ymax;
	int     hres;
	int     vres;
	char    pallete[48];
	char    reserved;
	char    colour_planes;
	int     bytes_per_line;
	int     pallete_type;
	char    filler[58];
		} PCXHEAD;
PCXHEAD   header;
/* The variable header is a struct that will contain all the usefull picture
information shown in the above fields.  */
unsigned int width,depth;    /* will hold the header info for width and depth*/
unsigned int bytes;

char palette[768];

void deinit(void);/* return to normal           */
void setVGApalette(char *p); /*set the vga pallete   */
void ReadBuffer_A(FILE *fp,char *buffer);
FILE *open_pic(char *filename);


typedef int counter_type;

main(argc,argv)
	int argc;
	char *argv[];
{       int     c;
	FILE *input;
	char *buffer;
	counter_type   temparg=0;
	
   if(argc > 1)
    {
	for(temparg=1; temparg< argc; temparg++)
	{
	   input = open_pic(argv[temparg]);
	   if (input != NULL){
		if((buffer = (char *) malloc(64000)) == NULL){
			printf("not enough memory to perform read operation");
		}
	/* initialize vgamode 320x200 in 256 color mode                       */
	asm{
	   mov ax,13h
	   int 10h
	   };
	      setVGApalette(palette);        
	      ReadBuffer_A(input,buffer);
	      fclose(input);
	      free(buffer);}
	 else exit(1);
	 }
    }
deinit();
return 0;
}

FILE  *open_pic(char *filename)
{

	FILE *fp;

	if((fp=fopen(filename,"rb")) != NULL){
			/* read in the header */
	     if(fread((char *) &header,1,sizeof(PCXHEAD),fp)
		 == sizeof(PCXHEAD)){
	     /*check to make sure it's a picture */
		   if(header.manufacturer==0x0a &&
		      header.version ==5){

			if(!fseek(fp,-769L,SEEK_END)){
			   if(fgetc(fp) == 0x0c &&
			    fread(palette,1,768,fp) == 768) {
			      fseek(fp,128L,SEEK_SET);
			      /*allocate a big buffer */
				width = (header.xmax -
				header.xmin) +1;
				depth = (header.ymax-
				header.ymin)+1;
				bytes=header.bytes_per_line;
					} else
			puts("error reading pallete");
		}else puts ("error seeking to pallete");
	}else printf("not a 256 color pcx file.\n");
     }else printf("error reading %s.\n",filename);
/*  fclose(fp); */
 } else printf ("Error opening %s.\n",filename);

return fp;
}


void  ReadBuffer_A(FILE *fp, char *buffer)
{
	char *p;
	int size,x,c,count;
	long int n;
	long int i=0;
	long int   inc;
	c=0;
	p = MK_FP(0xa000,0);   /* MAKE P, A POINTER TO THE VIDEO SCREEN */
/* THE PCX INFORMATION IS STORED IN A SOMEWHAT PACKED FORMAT, IF THERE ARE         
50 PIXELS OF BLACK THEN THE PCX FILE WILL CONTAIN THE NUMBER 50 FOLLOWED BY THE 
PIXEL INFORMATION FOR THE COLOR BLACK. THIS LOOP INTERPRETS AND UNPACKS THAT
DATA                                                            */
	for (i=0;i<64000;++i){
		c=fgetc(fp) & 0xff;  /* if it's a run of bytes field */

		if((c & 0xc0) == 0xc0) { /* and off the high bits */
			x=c & 0x3f;
			/* run the byte */
			c=fgetc(fp);
			while(x--) buffer[i++]=c;
			i--;
			}
			/*else just store it */
			else buffer[i]=c;
		}

      
/* iterate the random number routine 80,000 times to get as much fade in as
possible and to slow it down a little.  If you notice the i+=32000                            
That's because the Rand() function only returns a number in the range of 2exp15 -1
so we actually draw two dots onto the screen at a time, 1 in the upper half
of the screen, and 1 in the lower half.  The effect is still good       */
for(n=0; n <120000 ; n++){        
	     i = (rand() % 32000);  
	     p[i] = buffer[i];
	     i+=32000;
	     p[i]=buffer[i];
		
	}         
/* once the for loop is done there will still be about 30-40 dots left undone
at this point we just copy the entire buffer onto screen which effectively fills
these dots in.                                                          */
memcpy(p,buffer,64000); /*COPY 64000 POSITIONS OF BUFFER ONTO THE SCREEN */

/*0 out the buffer for a fade-out                                       */

memset(buffer,0,64000); /*COPY 0 INTO 64000 POSITIONS OF THE BUFFER */

/* add a delay loop so we can see the screen for a sec before we fade-out */
for(n=0; n < 120000;n++){
	for(i=0;i < 10;i++);};
/*  Same principle as above---fade-out                                  */
for(n=0; n <120000 ; n++){        
	     i = (rand() % 32000);  
	     p[i] = buffer[i];
	     i+=32000;
	     p[i]=buffer[i];
		
  }         
memcpy(p,buffer,64000);         

}




void  deinit(void)   /*turn off graphics card */
{
     asm{
	mov ax,3h
	int 10h
	};

}

void setVGApalette(char *p)   /*set the VGA palette to RGB buffer p */

{
	union REGS r;
	struct SREGS sr;
	int i;

	/* convert eight bits to six bits */
	for (i=0;i<768;i++) p[i]=p[i] >> 2;

	r.x.ax=0x1012;
	r.x.bx=0;
	r.x.cx=256;
	r.x.dx=FP_OFF(p);
	sr.es=FP_SEG(p);
	int86x(0x10,&r,&r,&sr);
}





