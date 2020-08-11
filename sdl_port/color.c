/*
 * Mandelbrot Frame Coloring
 *
 * Aaron Moore, August 2020
 *
 * Adapted from 'PROGRAM COLOR'
 *
 * written by dan moore in June 1995
 *
 */

/*
 * To build and run: `gcc sdl_port/color.c -lm -lSDL2 -lSDL2_ttf -o color && ./color`
 * (must be done in the root project folder)
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#include <graph.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "sdl_helpers.c"

#define RGB( rd, g, b) (0x3F3F3FL & ( (long) (b) << 16 | (g) << 8 | (rd) ) )

const int SCREEN_WIDTH = 700;
const int SCREEN_HEIGHT = 500;

const int WIDTH = 580;
const int HEIGHT = 406;

int pix[580][406];          /* screen pixels */
FILE *fopen();              /* file open function */
FILE *fpin;                 /* input file pointer */

int main(int argc, char *argv[])
{
#if defined _VRES16COLOR
    long int violet;         /* the color violet created by macro RGB */
    long int bluehi;         /* the color bluehi created by macro RGB */
    long int bluelo;         /* the color bluelo created by macro RGB */
    long int greenhi;        /* the color greenhi created by macro RGB */
    long int greenlo;        /* the color greenlo created by macro RGB */
    long int yellow;         /* the color yellow created by macro RGB */
    long int yellowlo;       /* the color yellowlo created by macro RGB */
    long int orange;         /* the color orange created by macro RGB */
    long int redhi;          /* the color redhi created by macro RGB */
    long int pink;           /* the color pink created by macro RGB */
#elif defined SDL_VERSION
    SDL_Color violet;         /* the color violet as represented in SDL */
    SDL_Color bluehi;         /* the color bluehi as represented in SDL */
    SDL_Color bluelo;         /* the color bluelo as represented in SDL */
    SDL_Color greenhi;        /* the color greenhi as represented in SDL */
    SDL_Color greenlo;        /* the color greenlo as represented in SDL */
    SDL_Color yellow;         /* the color yellow as represented in SDL */
    SDL_Color yellowlo;       /* the color yellowlo as represented in SDL */
    SDL_Color orange;         /* the color orange as represented in SDL */
    SDL_Color redhi;          /* the color redhi as represented in SDL */
    SDL_Color pink;           /* the color pink as represented in SDL */
#endif

    int ans;                 /* miscellaneous user response */
    double BOT;              /* bottom dimension of the rectangular area
                                of the complex plane to be examined */
    int dif;                 /* k differential for assigning color */
    int div[16];             /* divisions for assigning color */
    double fabs(double);     /* absolute value function */
    int i, j, k, n, m;       /* loop indices, array subscripts, or flags */
    int ii, jj, kk;          /* loop indices, array subscripts, or flags */
    char IV[50];             /* interval between ticks on either axis */
    int len;                 /* string length */
    double limx, limy;       /* limit on size of rect. area of complex plane */
    int min;                 /* lowest k value */
    char OX[15];             /* origin x-coordinate for labeling */
    char OY[15];             /* origin y-coordinate for labeling */
    int paint[16];           /* sixteen screen colors used */
    int rd, g, b;            /* red, green, and blue for macro RGB */
    char resp[20];           /* miscellaneous user response */
    double SID;              /* side dimension of rectangular area
                                of the complex plane to be examined */
    double swX;              /* x-coordinate of southwest corner of square area
                                of the complex plane to be examined */
    double swY;              /* y-coordinate of southwest corner of square area
                                of the complex plane to be examined */
    char title1[100];        /* info under x-axis  */
    char title2[100];        /* info under x-axis  */

    printf("\n\n\nMandelbrot set screen coloring program");

    if (argc == 2) {
        strcpy(resp, argv[1]);
    } else {
        printf("\n\nplease specify an input filename (q to quit) -> ");
        scanf("%s", resp);
        if ((len = strlen(resp)) == 0 || (len == 1 && (resp[0] == 'q'
            || resp[0] == 'Q')))
           exit(0);
    }
    if ((fpin = fopen(resp, "r")) == NULL) {
        printf("\n\nError opening file %s ...\nProgram terminated ...\n",
               resp);
        exit(0);
    }
/*
 *    read input file
 */
    printf("\n%s","   reading input file...");

    fscanf(fpin, "%lf %lf %lf %i", &swX, &swY, &BOT, &min);

    for ( k = 0; k <= 36; k++) {
        i = k * 16;                         /* 16 columns of data at a time */
        for ( j = 0; j <= 404; j++) {       /* rows */
            if (k == 36) {
                fscanf(fpin,"%i %i %i\n",
                       &pix[i][j], &pix[i+1][j], &pix[i+2][j]);
            }
            if (k < 36) {
                fscanf(fpin,
                "%i %i %i %i %i %i %i %i %i %i %i %i %i %i %i %i\n",
                &pix[i][j], &pix[i+1][j], &pix[i+2][j], &pix[i+3][j],
                &pix[i+4][j], &pix[i+5][j], &pix[i+6][j], &pix[i+7][j],
                &pix[i+8][j], &pix[i+9][j], &pix[i+10][j], &pix[i+11][j],
                &pix[i+12][j], &pix[i+13][j], &pix[i+14][j], &pix[i+15][j]);
            }
       }
   }
   printf("\n%s","   preparing to color screen...\n");
/*
 *   prepare graph labels
 */
    SID = BOT * HEIGHT/WIDTH;
    sprintf (IV, "%22.20f", (BOT/10));
    sprintf(OX, "%12.9f", swX);
    sprintf(OY, "%12.9f", swY);
    strcpy(title1,"ORIGIN:   X = ");
    strcat(title1,OX);
    strcat(title1,"     Y = ");
    strcat(title1,OY);
    strcpy(title2,"grid interval = ");
    strcat(title2,IV);

/*****************************
 *  assign colors to pixels:
 ****************************/
    dif = 1000 - min;
    div[0] =  min + (int)floor( (float)dif * .010);    /*  white     */
    div[1] =  min + (int)floor( (float)dif * .015);    /*  brown     */
    div[2] =  min + (int)floor( (float)dif * .020);    /*  red       */
    div[3] =  min + (int)floor( (float)dif * .030);    /*  redhi     */
    div[4] =  min + (int)floor( (float)dif * .040);    /*  orange    */
    div[5] =  min + (int)floor( (float)dif * .050);    /*  yellowlo  */
    div[6] =  min + (int)floor( (float)dif * .060);    /*  yellow    */
    div[7] =  min + (int)floor( (float)dif * .080);    /*  greenlo   */
    div[8] =  min + (int)floor( (float)dif * .100);    /*  green     */
    div[9] =  min + (int)floor( (float)dif * .150);    /*  greenhi   */
    div[10] =  min + (int)floor( (float)dif * .200);   /*  cyan      */
    div[11] =  min + (int)floor( (float)dif * .250);   /*  bluelo    */
    div[12] =  min + (int)floor( (float)dif * .300);   /*  blue      */
    div[13] =  min + (int)floor( (float)dif * .350);   /*  bluehi    */
    div[14] =  min + (int)floor( (float)dif * .400);   /*  magenta   */

    for ( i = 0; i <= 578; i++) {
        for ( j = 0; j <= 404; j++) {
            k = 0;
            if (pix[i][j] > 999) {
                pix[i][j] = 0;                      /* BLACK pixels */
                k = 1;
            }
            if (pix[i][j] < div[0] && k == 0) {
                pix[i][j] = 16;                     /* WHITE pixels */
                k = 1;
            }
            if (pix[i][j] < div[1] && k == 0) {
                pix[i][j] = 15;                     /* BROWN pixels */
                k = 1;
            }
            if (pix[i][j] < div[2] && k == 0) {
                pix[i][j] = 14;                     /* RED pixels */
                k = 1;
            }
            if (pix[i][j] < div[3] && k == 0) {
                pix[i][j] = 13;                     /* REDHI pixels */
                k = 1;
            }
            if (pix[i][j] < div[4] && k == 0) {
                pix[i][j] = 12;                     /* ORANGE pixels */
                k = 1;
            }
            if (pix[i][j] < div[5] && k == 0) {
                pix[i][j] = 11;                     /* YELLOWLO pixels */
                k = 1;
            }
            if (pix[i][j] < div[6] && k == 0) {
                pix[i][j] = 10;                     /* YELLOW pixels */
                k = 1;
            }
            if (pix[i][j] < div[7] && k == 0) {
                pix[i][j] = 9;                     /* GREENLO pixels */
                k = 1;
            }
            if (pix[i][j] < div[8] && k == 0) {
                pix[i][j] = 8;                     /* GREEN pixels */
                k = 1;
            }
            if (pix[i][j] < div[9] && k == 0) {
                pix[i][j] = 7;                     /* GREENHI pixels */
                k = 1;
            }
            if (pix[i][j] < div[10] && k == 0) {
                pix[i][j] = 6;                     /* CYAN pixels */
                k = 1;
            }
            if (pix[i][j] < div[11] && k == 0) {
                pix[i][j] = 5;                     /* BLUELO pixels */
                k = 1;
            }
            if (pix[i][j] < div[12] && k == 0) {
                pix[i][j] = 4;                     /* BLUE pixels */
                k = 1;
            }
            if (pix[i][j] < div[13] && k == 0) {
                pix[i][j] = 3;                     /* BLUEHI pixels */
                k = 1;
            }
            if (pix[i][j] < div[14] && k == 0) {
                pix[i][j] = 2;                     /* MAGENTA pixels */
                k = 1;
            }
            if (pix[i][j] < 1000 && k == 0) {
                pix[i][j] = 1;                     /* VIOLET pixels */
                k = 1;
            }
        }
    }

/******************************************
 *  draw graph lines and labels:
 ******************************************/

#if defined _VRES16COLOR
    _setvideomode(_VRES16COLOR);

    _setcolor(3);
    _moveto(50,8);       /* 6 pixels north of the northwest corner  */
    _lineto(50,420);     /* southwest corner (drawing left side)    */
    _lineto(630,420);    /* southeast corner (drawing bottom)       */
    _lineto(630,14);     /* northeast corner (drawing right side)   */
    _lineto(44,14);      /* 6 pixels west of nw corner (drawing top)  */
    _moveto(44,72);
    _lineto(50,72);       /*  left side tick marks  */
    _moveto(44,130);
    _lineto(50,130);
    _moveto(44,188);
    _lineto(50,188);
    _moveto(44,246);
    _lineto(50,246);
    _moveto(44,304);
    _lineto(50,304);
    _moveto(44,362);
    _lineto(50,362);
    _moveto(44,420);
    _lineto(50,420);
    _lineto(50,426);      /*  bottom tick marks  */
    _moveto(108,420);
    _lineto(108,426);
    _moveto(166,420);
    _lineto(166,426);
    _moveto(224,420);
    _lineto(224,426);
    _moveto(282,420);
    _lineto(282,426);
    _moveto(340,420);
    _lineto(340,426);
    _moveto(398,420);
    _lineto(398,426);
    _moveto(456,420);
    _lineto(456,426);
    _moveto(514,420);
    _lineto(514,426);
    _moveto(572,420);
    _lineto(572,426);
    _moveto(630,420);
    _lineto(630,426);
    _moveto(630,420);
    _lineto(636,420);     /*  right side tick marks  */
    _moveto(630,362);
    _lineto(637,362);
    _moveto(630,304);
    _lineto(636,304);
    _moveto(630,246);
    _lineto(636,246);
    _moveto(630,188);
    _lineto(636,188);
    _moveto(630,130);
    _lineto(636,130);
    _moveto(630,72);
    _lineto(636,72);
    _moveto(630,14);
    _lineto(636,14);
    _moveto(630,14);
    _lineto(630,8);      /*  top tick marks  */
    _moveto(572,14);
    _lineto(572,8);
    _moveto(514,14);
    _lineto(514,8);
    _moveto(456,14);
    _lineto(456,8);
    _moveto(398,14);
    _lineto(398,8);
    _moveto(340,14);
    _lineto(340,8);
    _moveto(282,14);
    _lineto(282,8);
    _moveto(224,14);
    _lineto(224,8);
    _moveto(166,14);
    _lineto(166,8);
    _moveto(108,14);
    _lineto(108,8); 
#elif defined SDL_VERSION
    setupGraphics("Mandelbrot", SCREEN_WIDTH, SCREEN_HEIGHT);

    setPalletColor(3);
    setPosition(50,8);                  /* 6 pixels north of the northwest corner  */
    drawLineAndSetPosition(50,420);     /* southwest corner (drawing left side)    */
    drawLineAndSetPosition(630,420);    /* southeast corner (drawing bottom)       */
    drawLineAndSetPosition(630,14);     /* northeast corner (drawing right side)   */
    drawLineAndSetPosition(44,14);      /* 6 pixels west of nw corner (drawing top)  */
    setPosition(44,72);
    drawLineAndSetPosition(50,72);      /*  left side tick marks  */
    setPosition(44,130);
    drawLineAndSetPosition(50,130);
    setPosition(44,188);
    drawLineAndSetPosition(50,188);
    setPosition(44,246);
    drawLineAndSetPosition(50,246);
    setPosition(44,304);
    drawLineAndSetPosition(50,304);
    setPosition(44,362);
    drawLineAndSetPosition(50,362);
    setPosition(44,420);
    drawLineAndSetPosition(50,420);
    drawLineAndSetPosition(50,426);     /*  bottom tick marks  */
    setPosition(108,420);
    drawLineAndSetPosition(108,426);
    setPosition(166,420);
    drawLineAndSetPosition(166,426);
    setPosition(224,420);
    drawLineAndSetPosition(224,426);
    setPosition(282,420);
    drawLineAndSetPosition(282,426);
    setPosition(340,420);
    drawLineAndSetPosition(340,426);
    setPosition(398,420);
    drawLineAndSetPosition(398,426);
    setPosition(456,420);
    drawLineAndSetPosition(456,426);
    setPosition(514,420);
    drawLineAndSetPosition(514,426);
    setPosition(572,420);
    drawLineAndSetPosition(572,426);
    setPosition(630,420);
    drawLineAndSetPosition(630,426);
    setPosition(630,420);
    drawLineAndSetPosition(636,420);    /*  right side tick marks  */
    setPosition(630,362);
    drawLineAndSetPosition(636,362);
    setPosition(630,304);
    drawLineAndSetPosition(636,304);
    setPosition(630,246);
    drawLineAndSetPosition(636,246);
    setPosition(630,188);
    drawLineAndSetPosition(636,188);
    setPosition(630,130);
    drawLineAndSetPosition(636,130);
    setPosition(630,72);
    drawLineAndSetPosition(636,72);
    setPosition(630,14);
    drawLineAndSetPosition(636,14);
    setPosition(630,14);
    drawLineAndSetPosition(630,8);      /*  top tick marks  */
    setPosition(572,14);
    drawLineAndSetPosition(572,8);
    setPosition(514,14);
    drawLineAndSetPosition(514,8);
    setPosition(456,14);
    drawLineAndSetPosition(456,8);
    setPosition(398,14);
    drawLineAndSetPosition(398,8);
    setPosition(340,14);
    drawLineAndSetPosition(340,8);
    setPosition(282,14);
    drawLineAndSetPosition(282,8);
    setPosition(224,14);
    drawLineAndSetPosition(224,8);
    setPosition(166,14);
    drawLineAndSetPosition(166,8);
    setPosition(108,14);
    drawLineAndSetPosition(108,8); 
#endif

/*
 *   set font to helvetica
 */ 
#if defined _VRES16COLOR
    _registerfonts( "helvb.fon" );
    _setfont("b h10 w5 t'helv'");

    _moveto(35,9);         /* y-axis labels */
    _outgtext("7");
    _moveto(35,67);
    _outgtext("6");
    _moveto(35,125);
    _outgtext("5");
    _moveto(35,183);
    _outgtext("4");
    _moveto(35,241);
    _outgtext("3");
    _moveto(35,299);
    _outgtext("2");
    _moveto(35,357);
    _outgtext("1");
    _moveto(35,414);
    _outgtext("0");
    _moveto(47,429);
    _outgtext("0");        /* x-axis labels */
    _moveto(105,429);
    _outgtext("1");
    _moveto(163,429);
    _outgtext("2");
    _moveto(221,429);
    _outgtext("3");
    _moveto(279,429);
    _outgtext("4");
    _moveto(337,429);
    _outgtext("5");
    _moveto(395,429);
    _outgtext("6");
    _moveto(453,429);
    _outgtext("7");
    _moveto(511,429);
    _outgtext("8");
    _moveto(569,429);
    _outgtext("9");
    _moveto(627,429);
    _outgtext("10");

    _moveto(100,447);      /* info under the x-axis */
    _outgtext(title1);
    _moveto(100,463);
    _outgtext(title2);

    _unregisterfonts();
#elif defined SDL_VERSION
    setPosition(35,9);         /* y-axis labels */
    printText("7");
    setPosition(35,67);
    printText("6");
    setPosition(35,125);
    printText("5");
    setPosition(35,183);
    printText("4");
    setPosition(35,241);
    printText("3");
    setPosition(35,299);
    printText("2");
    setPosition(35,357);
    printText("1");
    setPosition(35,414);
    printText("0");
    setPosition(47,429);
    printText("0");        /* x-axis labels */
    setPosition(105,429);
    printText("1");
    setPosition(163,429);
    printText("2");
    setPosition(221,429);
    printText("3");
    setPosition(279,429);
    printText("4");
    setPosition(337,429);
    printText("5");
    setPosition(395,429);
    printText("6");
    setPosition(453,429);
    printText("7");
    setPosition(511,429);
    printText("8");
    setPosition(569,429);
    printText("9");
    setPosition(627,429);
    printText("10");

    setPosition(100,447);      /* info under the x-axis */
    printText(title1);
    setPosition(100,463);
    printText(title2);
#endif

/**********************
 *  color the screen:
 **********************/
#if defined _VRES16COLOR
    violet = RGB( 63, 0, 31 );
    bluehi = RGB( 31, 0, 63 );
    bluelo = RGB( 0, 31, 63 );
    greenhi = RGB( 0, 63, 31 );
    greenlo = RGB( 31, 63, 0 );
    yellow = RGB( 63, 63, 0 );
    yellowlo = RGB( 63, 42, 0 );
    orange = RGB( 63, 34, 0 );
    redhi = RGB( 63, 26, 0 );
    _remappalette( 8, violet);
    _remappalette( 9, bluehi);
    _remappalette( 10, bluelo);
    _remappalette( 11, greenhi);
    _remappalette( 12, greenlo);
    _remappalette( 14, yellow);
    _remappalette( 13, yellowlo);
    _remappalette( 15, orange);
    _remappalette( 7, redhi);
#elif defined SDL_VERSION
    // Used this function to convert from 4 bit to 8 bit per color channel:
    // fn = (x) => (((x + 1) / 64) * 256) - 1
    violet =   (SDL_Color) { 255,   0, 127, 255 };
    bluehi =   (SDL_Color) { 127,   0, 255, 255 };
    bluelo =   (SDL_Color) {   0, 127, 255, 255 };
    greenhi =  (SDL_Color) {   0, 255, 127, 255 };
    greenlo =  (SDL_Color) { 127, 255,   0, 255 };
    yellow =   (SDL_Color) { 255, 255,   0, 255 };
    yellowlo = (SDL_Color) { 255, 171,   0, 255 };
    orange =   (SDL_Color) { 255, 139,   0, 255 };
    redhi =    (SDL_Color) { 255, 107,   0, 255 };
    assignPalletColor( 8, &violet);
    assignPalletColor( 5, &(SDL_Color){ 171,   0, 171, 255 }); // MAGENTA
    assignPalletColor( 9, &bluehi);
    assignPalletColor( 1, &(SDL_Color){   0,   0, 171, 255 }); // BLUE
    assignPalletColor(10, &bluelo);
    assignPalletColor( 3, &(SDL_Color){   0, 171, 171, 255 }); // CYAN
    assignPalletColor(11, &greenhi);
    assignPalletColor( 2, &(SDL_Color){   0, 171,   0, 255 }); // GREEN
    assignPalletColor(12, &greenlo);
    assignPalletColor(14, &yellow);
    assignPalletColor(13, &yellowlo);
    assignPalletColor(15, &orange);
    assignPalletColor( 7, &redhi);
    assignPalletColor( 4, &(SDL_Color){ 171,   0,   0, 255 }); // RED
    assignPalletColor( 6, &(SDL_Color){ 171,  87,   0, 255 }); // BROWN
#endif

    paint[0] = 8;     /* violet   */
    paint[1] = 5;     /* MAGENTA  */
    paint[2] = 9;     /* bluehi   */
    paint[3] = 1;     /* BLUE     */
    paint[4] = 10;    /* bluelo   */
    paint[5] = 3;     /* CYAN     */
    paint[6] = 11;    /* greenhi  */
    paint[7] = 2;     /* GREEN    */
    paint[8] = 12;    /* greenlo  */
    paint[9] = 14;    /* yellow   */
    paint[10] = 13;   /* yellowlo */
    paint[11] = 15;   /* ORANGE   */
    paint[12] = 7;    /* redhi    */
    paint[13] = 4;    /* RED      */
    paint[14] = 8;    /* violet   */
    paint[15] = 6;    /* brown    */

    for ( m = 0; m <= 15; m++) {
#if defined _VRES16COLOR
        _setcolor(paint[m]);
#elif defined SDL_VERSION
        setPalletColor(paint[m]);
#endif
        for ( i = 0; i <= 578; i++) {
            for ( j = 0; j <= 404; j++) {
#if defined _VRES16COLOR
                if (pix[i][j] == (m+1) ) _setpixel(i+51,j+15);
#elif defined SDL_VERSION
                if (pix[i][j] == (m+1)) colorPixel(i+51,j+15);
#endif
            }
        }
    }

#if defined _VRES16COLOR
    getch();                        /* pause */
    _setvideomode(_DEFAULTMODE);
#elif defined SDL_VERSION
    SDL_RenderPresent(renderer);
    waitForExit();
    cleanupGraphics();
#endif
}

