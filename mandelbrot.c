/*
 * Mandelbrot Frame Rendering
 *
 * Aaron Moore, August 2020
 * 
 * Adapted from 'PROGRAM M'
 *
 * Written by Dan Moore in June 1995
 *
 */

/*
 * To build and run: `gcc mandelbrot.c -lm -lSDL2 -lSDL2_ttf -o mandelbrot && ./mandelbrot`
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// #include <graph.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "sdl_helpers.c"

#if defined SDL_VERSION
void renderFrame(char*, char*, int, int, int, int);
#endif

#if defined SDL_VERSION
#endif

/*
 * Macro RGB. Creates a 32 bit integer that represents an RGB color value.
 * Colors are 64-bit (as is enforced by the 3F mask). Their bytes are arranged
 * like so:
 *            [0][B][G][R]
 *          31^          ^0
 */
#define RGB( rd, g, b) (0x3F3F3FL & ( (long) (b) << 16 | (g) << 8 | (rd) ) )

/*
 * Todo: Consider refactor to use <complex.h> which supports complex numbers
 *       as part of the standard library cince C99. 
 */
typedef struct FCOMPLEX {double r,i;} fcomplex;

const int WIDTH = 580;
const int HEIGHT = 406;

int pix[580][406];          /* screen pixels */
FILE *fopen();              /* file open function */
FILE *fpout;                /* output file pointer */

int main()
{
    int ans;                 /* miscellaneous user response */
#if defined _VRES16COLOR
    long int bluehi;         /* the color bluehi created by macro RGB */
#elif defined SDL_VERSION
    SDL_Color bluehi;        /* the color bluehi as represented in SDL */
#endif
    double BOT;              /* bottom dimension of the rectangular area
                                of the complex plane to be examined */
    fcomplex C;              /* complex # at each pixel */
    float chunk;             /* for graphical chunks of progress */
    double fabs(double);     /* absolute value function */
    double gap;              /* distance between pixels (on complex plane) */
    int i, j, k, n, m;       /* loop indices, array subscripts, or flags */
    int ii, jj, kk;          /* loop indices, array subscripts, or flags */
    int len;                 /* string length */
    double limx, limy;       /* limit on size of rect. area of complex plane */
    int min;                 /* lowest k value */
    double pow(double, double);    /* computes arg1 raised to arg2 power */
    char progress[40];        /* info to user */
    double RE, IM;           /* real and imaginary parts for each pixel */
    char resp[20];           /* miscellaneous user response */
    char s[5];               /* char version of sofar for graphic display */
    double SID;              /* side dimension of rectangular area
                                of the complex plane to be examined */
    float sofar;             /* indication of the progress of calculations */
    double swX;              /* x-coordinate of southwest corner of square area
                                of the complex plane to be examined */
    double swY;              /* y-coordinate of southwest corner of square area
                                of the complex plane to be examined */
    float SZ;                /* "size" or modulus of complex number  */
    int x1;                  /* x-coord. of SW corner of progress rectangle */
    int x2;                  /* x-coord. of NE corner of progress rectangle */
    int y1;                  /* y-coord. of SW corner of progress rectangle */
    int y2;                  /* y-coord. of NE corner of progress rectangle */
    fcomplex Z;              /* complex solution of equation being tested  */

    fcomplex Complex(double,double);
    fcomplex Cmul(fcomplex,fcomplex);
    fcomplex Cadd(fcomplex,fcomplex);


    printf("\n\n\nMandelbrot Set Exploration Program");

    printf("\n\n%s\n%s\n%s",
           "choose (1) for a full size Mandelbrot set,",
           "    or (2) to magnify a section of the set.",
           "    -> ");
    scanf("%d", &ans);

    printf("\n\nplease specify an output filename (q to quit) -> ");
    scanf("%s", resp);
    if ((len = strlen(resp)) == 0 || (len == 1 && (resp[0] == 'q'
        || resp[0] == 'Q')))
        exit(0);
    if ((fpout = fopen(resp, "w")) == NULL) {
        printf("\n\nError opening file %s ...\nProgram terminated ...\n",
              resp);
        exit(0);
    }

    if ( ans == 1 ) {
        swX = -2.5;
        swY = -1.25;
        BOT = 3.5;
        SID = BOT * HEIGHT/WIDTH;
    }
    if ( ans == 2 ) {
        printf("\n\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s",
        "M will size the magnification to your rectangular screen.             ",
        "                                                                      ",
        "              .............................                           ",
        "              .                           .                           ",
        "              .                           .                           ",
        "              .                           .                           ",
        "              .                           .                           ",
        "              .                           .                           ",
        "              .                           .                           ",
        "              .............................                           ",
        "              ^         bottom                                        ",
        "           SW corner                                                  ");
        printf("\n\n\n%s\n%s\n%s\n%s\n%s\n%s\n%s",
        "Using complex plane coordinates, specify the SW corner of the area    ",
        "you want to magnify and the length of the bottom of the rectangle.    ",
        "                                                                      ",
        "First, the coordinates.  Specify the horizontal coordinate of SW:     ",
        "(X should be between -2.5 and 0.80)                                   ",
        "                                                                      ",
        "                 X = ");
        while(1) {
            scanf("%lf", &swX);
            if (swX <= 0.8 && swX >= -2.5) break;
            printf("\n\n%s\n%s",
            "This number is out of the permissible range.",
            "X should be between -2.5 and 0.80 -- try again: X = ");
        }
        printf("\n\n%s%lf","X = ", swX);
        printf("\n\n%s\n%s\n%s",
        "Now enter the vertical coordinate:  ",
        "(Y should be between -1.25 and 1.25)",
        "                 Y = ");
        while(1) {
            scanf("%lf", &swY);
            if (swY <= 1.25 && swY >= -1.25) break;
            printf("\n\n%s\n%s",
            "This number is out of the permissible range.",
            "Y should be between -1.25 and 1.25 -- try again: Y = ");
        }
        printf("\n\n%s%lf","Y = ", swY);
        printf("\n\nNow enter the rectangle bottom dimension: B = ");
        while(1) {
            scanf("%lf", &BOT);
            printf("\n\n%s%lf\n","B = ", BOT);
            SID = BOT * HEIGHT/WIDTH;
            limx = 0.8-swX;
            limy = 1.25-swY;
            if (BOT < limx && SID < limy) break;
            printf("\n\n%s\n%s\n%s",
            "This length is too large.  The rectangle you have specified",
            "covers area outside the full size Mandelbrot set.",
            "Please try again: B = ");
        }
    }
/**********************************************************
 *  do complex calculations and determine # of iterations:
 **********************************************************/
#if defined _VRES16COLOR
    bluehi = RGB( 31, 0, 63 );
    _remappalette( 9, bluehi);
#endif
    strcpy(progress,"percent of complex plane completed:");

    gap = BOT / WIDTH;
    RE = swX + 4*gap;
    min = 1000;

#if defined _VRES16COLOR
    _setvideomode(_VRES16COLOR);
    _setcolor(3);
    _registerfonts( "helvb.fon" );
    _setfont("b h10 w5 t'helv'");
    _moveto(50,200);                         /* display progress */
    _outgtext(progress);
    _moveto(197,247);
    _lineto(303,247);
    _lineto(303,263);                         /* box for progress graphic */
    _lineto(197,263);
    _lineto(197,247);
#elif defined SDL_VERSION
    setupGraphics("Mandelbrot", WIDTH, HEIGHT);
    bluehi = (SDL_Color) { 127, 0, 255, 255 };
    assignPalletColor(9, &bluehi);
#endif

    chunk = 1.0;
    x1 = 200;
    y1 = 260;
    x2 = 201;
    y2 = 250;

    for ( i = 0; i <= 578; i++) {            /* columns (real direction) */
        RE = RE + gap;
        IM = swY + 404*gap;
        sofar = ( (float)i / 578 )*100;
        sprintf (s, "%4.1f", sofar);


        if (sofar > chunk) {
            chunk = chunk+1;
#if defined _VRES16COLOR
            _setcolor(3);
            _moveto(230,230);
            _outgtext(s);
            _setcolor(9);
            _rectangle(_GFILLINTERIOR,x1,y1,x2,y2);
#elif defined SDL_VERSION
            renderFrame(progress, s, x1, y1, x2, y2);
#endif
            x1 = x2;
            x2 = x2+1;
        }

        for ( j = 0; j <= 404; j++) {       /* rows (imaginary direction) */
            IM = IM - gap;
            Z = Complex(0,0);
            C = Complex( RE, IM );
            for ( k = 0; k <= 1000; k++) {
                /*
                 * Mandelbrot Equatation: Zn+1 = Zn^2 + C
                 */
                Z = Cadd( ( Cmul( Z, Z ) ), C );

                /**
                 * Absolute Value of Z, |Z|.
                 * Absolute Value of N can be formulated as sqrt(N^2)
                 * For Complex Numbers, this essentially becomes the Distance Formula.
                 * Distance Formula: SZ^2 = R^2 + I^2
                 */
                SZ = sqrt( (Z.r * Z.r) + (Z.i * Z.i) );
                if ( SZ > 2 ) break;
            }
            pix[i][j] = k;
            if ( k < min ) min = k;
        }
    }

#if defined _VRES16COLOR
    _unregisterfonts();

    getch();                        /* pause */
    _setvideomode(_DEFAULTMODE);
#elif defined SDL_VERSION
    renderFrame(progress, "100", x1, y1, x2, y2);
    waitForExit();
    cleanupGraphics();
#endif

/*****************************
 *  write output to a file:
 ****************************/
    fprintf(fpout,"%23.20f\n%23.20f\n%22.20f\n%i\n",
                  swX, swY, BOT, min);

    for ( k = 0; k <= 36; k++) {
        i = k * 16;                         /* 16 columns of data at a time */
        for ( j = 0; j <= 404; j++) {       /* rows */
            if (k == 36) {
                fprintf(fpout,"%4i %4i %4i\n", pix[i][j],pix[i+1][j],pix[i+2][j]);
            }
            if (k < 36) {
                fprintf(fpout,
                "%4i %4i %4i %4i %4i %4i %4i %4i %4i %4i %4i %4i %4i %4i %4i %4i\n",
                pix[i][j],pix[i+1][j],pix[i+2][j],pix[i+3][j],pix[i+4][j],pix[i+5][j],
                pix[i+6][j],pix[i+7][j],pix[i+8][j],pix[i+9][j],pix[i+10][j],
                pix[i+11][j],pix[i+12][j],pix[i+13][j],pix[i+14][j],pix[i+15][j]);
            }
        }
    }

/*  data looks like this:

1000 1000 1000 1000 1000 1000 1000 1000 1000 1000 1000 1000 1000 1000 1000 1000
1000 1000 1000 1000 1000 1000 1000 1000 1000 1000 1000 1000 1000 1000 1000 1000
                     ...etc    */

}

fcomplex Complex(re,im)
double re, im;              /* real and imaginary parts of complex #'s */
{
    fcomplex c;
    c.r = re;
    c.i = im;
    return c;
}

/*
 * Complex number multiplcation.
 *
 * A demonstration of why the below
 * computation is correct:
 *  (A+Bi)*(C+Di)
 *  A*C + A*Di + Bi*C + Bi*Di
 *  A*C + A*Di + Bi*C + B*D*(i^2)
 *  A*C + A*Di + Bi*C + B*D*(-1)
 *  (A*C - B*D) + (A*Di + C*Bi)
 */
fcomplex Cmul(a,b)
fcomplex a,b;
{
    fcomplex c;
    c.r= a.r*b.r - a.i*b.i;
    c.i= a.i*b.r + a.r*b.i;
    return c;
}

/*
 * Complex number addition.
 * (A+Bi) + (C+Di)
 * (A+C) + (B+D)i
 */
fcomplex Cadd(a,b)
fcomplex a,b;
{
    fcomplex c;
    c.r = a.r + b.r;
    c.i = a.i + b.i;
    return c;
}

#if defined SDL_VERSION
void renderFrame(char *progress, char *s, int x1, int y1, int x2, int y2) {
    // Clear frame buffer
    newFrame();

    // Draw progress text
    setPalletColor(3);
    setPosition(50, 200);
    printText(progress);

    // Draw box
    setPosition(197, 247);
    drawLineAndSetPosition(303, 247);
    drawLineAndSetPosition(303, 263);
    drawLineAndSetPosition(197, 263);
    drawLineAndSetPosition(197, 247);

    // Draw rect
    setPalletColor(9);
    SDL_Rect fill ={
        x1, y1, x2 - x1, y2 - y1 
    }; 
    SDL_RenderFillRect(renderer, &fill);

    // Draw progress percent
    setPalletColor(3);
    setPosition(230, 230);
    printText(s);

    // Display frame buffer
    SDL_RenderPresent(renderer);
}
#endif
