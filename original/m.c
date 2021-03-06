/*
      PROGRAM M

      written by dan moore in June 1995

 */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <graph.h>
#define RGB( rd, g, b) (0x3F3F3FL & ( (long) (b) << 16 | (g) << 8 | (rd) ) )

typedef struct FCOMPLEX {double r,i;} fcomplex;

int huge pix[580][406];     /* screen pixels */
FILE *fopen();              /* file open function */
FILE *fpout;                /* output file pointer */

main()
{
   int ans;                 /* miscellaneous user response */
   long int bluehi;         /* the color bluehi created by macro RGB */
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
      SID = BOT * 406/580;
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
      printf("\n\n\n%s\n%s\n%s\n%s\n%s\n%s",
      "Using complex plane coordinates, specify the SW corner of the area    ",
      "you want to magnify and the length of the bottom of the rectangle.    ",
      "                                                                      ",
      "First, the coordinates.  Specify the horizontal coordinate of SW:     ",
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
      printf("\n\nNow enter the vertical coordinate:  Y = ");
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
      printf("\n\n%s%lf","B = ", BOT);
         SID = BOT * 406/580;
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
   bluehi = RGB( 31, 0, 63 );
   _remappalette( 9, bluehi);

   strcpy(progress,"percent of complex plane completed:");

   gap = BOT / 580;
   RE = swX + 4*gap;
   min = 1000;

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
            _setcolor(3);
            _moveto(230,230);
            _outgtext(s);
            _setcolor(9);
            _rectangle(_GFILLINTERIOR,x1,y1,x2,y2);
            x1 = x2;
            x2 = x2+1;
        }

        for ( j = 0; j <= 404; j++) {       /* rows (imaginary direction) */
             IM = IM - gap;
             Z = Complex(0,0);
             C = Complex( RE, IM );
             for ( k = 0; k <= 1000; k++) {
                  Z = Cadd( ( Cmul( Z, Z ) ), C );
                  SZ = sqrt( (Z.r * Z.r) + (Z.i * Z.i) );
                  if ( SZ > 2 ) break;
             }
             pix[i][j] = k;
             if ( k < min ) min = k;
        }
   }

   _unregisterfonts();

   getch();                        /* pause */
   _setvideomode(_DEFAULTMODE);

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

fcomplex Cmul(a,b)
fcomplex a,b;
{
   fcomplex c;
   c.r=a.r*b.r-a.i*b.i;
   c.i=a.i*b.r+a.r*b.i;
   return c;
}

fcomplex Cadd(a,b)
fcomplex a,b;
{
   fcomplex c;
   c.r=a.r+b.r;
   c.i=a.i+b.i;
   return c;
}
