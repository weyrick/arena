// LOWLEV.H, include file for LOWLEV.C

void Plot_Pixel(int x, int y, unsigned char color, unsigned char *buf);  // Plot a pixel (fast)
void LoadPalette(char *pfilename);      // Load (WGT) palette
void OverlaySky(unsigned char far *image, unsigned char *scrbuf, int offset);  // Puts sky in
