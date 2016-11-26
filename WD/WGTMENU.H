/* 
   WordUp Graphics Toolkit V4.0  Drop-down Menu Routines
   Copyright 1994 Chris Egerter
*/

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	char *choice[10];
	int menux,menuy;
	int color;
	int bordercolor;
	int textcolor;
	} menulist; 

extern menulist dropdown[10];

extern int menubarcolor;
extern int menubartextcolor;
extern int bordercolor;
extern int highlightcolor;
extern int mouseinstalled;
extern char menuhotkey;

extern wgtfont menufont;

int    checkmenu (void);
void   initdropdowns (void);
void   removemenubar (void);
void   showmenubar (void);

#ifdef __cplusplus
}
#endif
