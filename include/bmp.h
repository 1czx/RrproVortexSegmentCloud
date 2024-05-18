typedef unsigned short WORD;
typedef long DWORD;
typedef int LONG;

WORD bfType;
typedef struct tagBITMAPFILEHEADER{
    DWORD	    bfSize;
    WORD		bfReserved1;
    WORD		bfReserved2;
    DWORD	    bfOffBits;
}	BITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER{
    DWORD	biSize;
    LONG	biWidth;
    LONG	biHeight;
    WORD	biPlanes;
    WORD	biBitCount;
    DWORD	biCompression;
    DWORD	biSizeImage;
    LONG	biXPelsPerMeter;
    LONG	biYPelsPerMeter;
    DWORD	biClrUsed;
    DWORD	biClrImportant;
}	BITMAPINFOHEADER;

typedef struct pixelinfor{
    unsigned char rgb_r;
    unsigned char rgb_g;
    unsigned char rgb_b;
    unsigned char rgb_reserved;
}   RGBQUAD;


unsigned char* bmp_orig;

int bmp_width;
int bmp_height;

void save_bmp(unsigned int width, unsigned int height,unsigned char* bmp_to, char* filename){
    //to write the bmp named as filename
    BITMAPFILEHEADER bmfh;
    BITMAPINFOHEADER bmih;
    bmfh.bfType = 0x4D42;
    bmfh.bfReserved1 = 0;
    bmfh.bfReserved2 = 0;
    bmfh.bfOffBits = 54;






    int line = (bmih.biWidth * bmih.biBitCount /8 +3) /4 *4;
    FILE *sp = fopen(filename, "wb");
    fwrite(&bfType, sizeof(WORD), 1, sp);
    fwrite(&bmfh, sizeof(BITMAPFILEHEADER), 1, sp);
    fwrite(&bmih, sizeof(BITMAPINFOHEADER), 1, sp);

    fwrite(bmp_to, line * bmih.biHeight, 1, sp);
    fclose(sp);
}