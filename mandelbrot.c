
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "complex.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>


/*
 * The Frame struct stores a single frame of the
 * Mandelbrot set. A frame is defined as the k
 * values at all points in the 580 x 406 grid
 * given an origin point and width. Depending on
 * the width, frames may be at different levels
 * of magnification.
 *
 * Frames also can have pointers to parents and
 * children, which are zoomed out or zoomed in
 * from the current frame, respectively.
 */

// Width and Height of a single mandelbrot set frame.
#define FRAME_WIDTH 580
#define FRAME_HEIGHT 406

struct Frame {
    struct Frame *parent;
    struct Frame *child;

    unsigned short k[FRAME_WIDTH][FRAME_HEIGHT]; // Values of k at each point.
    double min; // Minimum k value in this frame.

    // The origin is the bottom-left corner.
    double x; // Origin on the x axis.
    double y; // Origin on the y axis.

    double w; // Width.
};

struct Frame* renderFrame(struct Frame*, double, double, double);
void freeFrame(struct Frame*);

/*
 * Graphics
 */

// Width and Height of the window.
const int SCREEN_WIDTH = 700;
const int SCREEN_HEIGHT = 500;

const SDL_Point FRAME_ORIGIN = { 50, 14 };

struct Display {
    SDL_Window *window;
    SDL_Renderer *renderer;
    TTF_Font *font;
    SDL_Color color;
    SDL_Point position;
    SDL_Color pallet[255];
};

void destroyDisplayAndExit(struct Display*, char*, const char*);

struct Display* createDisplay() {
    struct Display *display = malloc(sizeof(struct Display));
    if (!display) {
        printf("Unable to allocate memory for display.\n");
        exit(0);
    }
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        destroyDisplayAndExit(display, "Unable to initialize SDL", SDL_GetError());
    }
    if (TTF_Init() != 0) {
        destroyDisplayAndExit(display, "Unable to initialize SDL_ttf", TTF_GetError());
    }
    display->window = SDL_CreateWindow(
        "Mandelbrot",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        0
    );
    if (!display->window) {
        destroyDisplayAndExit(display, "Unable to create window", SDL_GetError());
    }
    display->renderer = SDL_CreateRenderer(display->window, -1, SDL_RENDERER_ACCELERATED);
    if (!display->renderer) {
        destroyDisplayAndExit(display, "Unable to create renderer", SDL_GetError());
    }
    display->font = TTF_OpenFont("freefont-ttf/sfd/FreeSans.ttf", 12);
    if (!display->font) {
        destroyDisplayAndExit(display, "Unable to load font", TTF_GetError());
    }

    // Initialize Pallet
    for (int i = 0; i < 255; i++) {
        display->pallet[i] = (SDL_Color) { 0, 0, 0, 255 };
    }
}
void destroyDisplay(struct Display *display) {
    if (display->window) {
        SDL_DestroyWindow(display->window);
        display->window = 0;
    }
    if (display->renderer) {
        SDL_DestroyRenderer(display->renderer);
        display->renderer = 0;
    }
    if (display->font) {
        TTF_CloseFont(display->font);
        display->font = 0;
    }
    SDL_Quit();
}
void destroyDisplayAndExit(struct Display *display, char *message, const char *errMessage) {
    printf("%s: %s\n", message, errMessage);
    destroyDisplay(display);
    printf("Exiting.\n");
    exit(0);
}

void setColor(struct Display *display, Uint8 r, Uint8 g, Uint8 b) {
    display->color.r = r;
    display->color.g = g;
    display->color.b = b;
    SDL_SetRenderDrawColor(display->renderer, r, g, b, 255);
}
void assignPalletColor(struct Display *display, Uint8 c, SDL_Color *v) {
    display->pallet[c] = (SDL_Color) { v->r, v->g, v->b, 0xFF };
}
void setPalletColor(struct Display *display, Uint8 c) {
    SDL_Color p = display->pallet[c];
    setColor(display, p.r, p.g, p.b);
}
void setPosition(struct Display *display, int x, int y) {
    display->position.x = x;
    display->position.y = y;
}
void printText(struct Display *display, char *text) {
    // Render text
    SDL_Surface *textSurface = TTF_RenderText_Blended(
        display->font,
        text,
        display->color
    );
    if (!textSurface) {
        destroyDisplayAndExit(display, "Unable to render text.", TTF_GetError());
    }
    SDL_Texture *textTexture = SDL_CreateTextureFromSurface(display->renderer, textSurface);
    if (!textTexture) {
        destroyDisplayAndExit(display, "Unable create a text texture.", TTF_GetError());
    }

    // Draw rendered text to screen buffer
    SDL_Rect dest = {
        display->position.x,
        display->position.y,
        textSurface->w,
        textSurface->h
    };
    SDL_RenderCopy(display->renderer, textTexture, 0, &dest);

    // Clean up
    SDL_DestroyTexture(textTexture);
    SDL_FreeSurface(textSurface);
}
void drawLineAndSetPosition(struct Display *display, int x, int y) {
    SDL_RenderDrawLine(
        display->renderer,
        display->position.x,
        display->position.y,
        x,
        y
    );
    setPosition(display, x, y);
}
void colorPixel(struct Display *display, int x, int y) {
    SDL_RenderDrawPoint(display->renderer, x, y);
}
short useMin = 1;
void displayFrame(struct Display *display, struct Frame *frame) {

    // New Frame
    setColor(display, 0, 0, 0);
    SDL_RenderClear(display->renderer);
    setColor(display, 255, 255, 255);

    const short xStart = FRAME_ORIGIN.x;
    const short yStart = FRAME_ORIGIN.y;
    const short xEnd = FRAME_WIDTH + FRAME_ORIGIN.x;
    const short yEnd = FRAME_HEIGHT + FRAME_ORIGIN.y;
    
    // Draw Rectangle that will enclose the actual frame

    // Note: Using line drawing instead of rect because
    //       experimenting with them both, the latter did
    //       not align perfectly with the lines drawn for
    //       the interval markers, whereas the former did.

    setPosition(display, xStart, yStart);
    drawLineAndSetPosition(display, xStart, yEnd);
    drawLineAndSetPosition(display, xEnd, yEnd);
    drawLineAndSetPosition(display, xEnd, yStart);
    drawLineAndSetPosition(display, xStart, yStart);

    // Draw Interval Markers for grid

    const short markWidth = 6;

    // Draw Left interval markers
    for (short y = FRAME_ORIGIN.y; y <= yEnd; y += 58) {
        setPosition(display, xStart - markWidth, y);
        drawLineAndSetPosition(display, xStart, y);
    }
    // Draw Bottom Side interval markers
    for (short x = FRAME_ORIGIN.x; x <= xEnd; x += 58) {
        setPosition(display, x, yEnd + 6);
        drawLineAndSetPosition(display, x, yEnd);
    }
    // Draw Right interval markers
    for (short y = FRAME_ORIGIN.y; y <= yEnd; y += 58) {
        setPosition(display, xEnd + markWidth, y);
        drawLineAndSetPosition(display, xEnd, y);
    }
    // Draw Top Side interval markers
    for (short x = FRAME_ORIGIN.x; x <= xEnd; x += 58) {
        setPosition(display, x, yStart - 6);
        drawLineAndSetPosition(display, x, yStart);
    }

    // Draw Axis Labels
    
    char label[256];
    
    // x-axis (real)
    for (short x = 47, l = 0; x <= 627; x += 58, l++) {
        setPosition(display, x, 429);
        sprintf(label, "%i", l);
        printText(display, label);
    }
    // y-axis (imaginary)
    for (short y = 414, l = 0; y >= 8; y -= 58, l++) {
        setPosition(display, 35, y);
        sprintf(label, "%i", l);
        printText(display, label);
    }

    // Display Empty Frame (if no frame data is provided)
    if (!frame) {
        return;
    }

    // Draw Origin and Interval Labels

    // Origin
    setPosition(display, 100, 447);
    sprintf(label, "Origin:  X = %g  Y = %g", frame->x, frame->y);
    printText(display, label);

    // Interval
    setPosition(display, 100, 463);
    sprintf(label, "Grid Interval:  %g", frame->w / 10);
    printText(display, label);

    // Render Frame

    // Define colors
    SDL_Color black =    {   0,   0,   0, 255 };
    SDL_Color white =    { 255, 255, 255, 255 };
    SDL_Color brown =    { 171,  87,   0, 255 };
    SDL_Color violet =   { 255,   0, 127, 255 };
    SDL_Color red =      { 171,   0,   0, 255 };
    SDL_Color redhi =    { 255, 107,   0, 255 };
    SDL_Color orange =   { 255, 139,   0, 255 };
    SDL_Color yellowlo = { 255, 171,   0, 255 };
    SDL_Color yellow =   { 255, 255,   0, 255 };
    SDL_Color greenlo =  { 127, 255,   0, 255 };
    SDL_Color green =    {   0, 171,   0, 255 };
    SDL_Color greenhi =  {   0, 255, 127, 255 };
    SDL_Color cyan =     {   0, 171, 171, 255 }; 
    SDL_Color bluelo =   {   0, 127, 255, 255 };
    SDL_Color blue =     {   0,   0, 171, 255 };
    SDL_Color bluehi =   { 127,   0, 255, 255 };
    SDL_Color magenta =  { 171,   0, 171, 255 }; 
    
    // Define color bands
    short min = useMin ? frame->min : 0;
    short range = 1000 - min;
    short div[15];
    div[0] =  min + floor(range * .010); // brown  
    div[1] =  min + floor(range * .015); // violet   
    div[2] =  min + floor(range * .020); // red     
    div[3] =  min + floor(range * .030); // redhi   
    div[4] =  min + floor(range * .040); // orange  
    div[5] =  min + floor(range * .050); // yellowlo
    div[6] =  min + floor(range * .060); // yellow  
    div[7] =  min + floor(range * .080); // greenlo 
    div[8] =  min + floor(range * .100); // green   
    div[9] =  min + floor(range * .150); // greenhi 
    div[10] = min + floor(range * .200); // cyan    
    div[11] = min + floor(range * .250); // bluelo  
    div[12] = min + floor(range * .300); // blue    
    div[13] = min + floor(range * .350); // bluehi  
    div[14] = min + floor(range * .400); // magenta 

    // Color in frame
    for (short r = 0; r <= 578; r++) {
        for (short i = 0; i <= 404; i++) {
            short k = frame->k[r][i];
            SDL_Color c;
            // Determine b (color band)
            if (k > 999) {
                c = black;
            } else if (k < div[0]) {
                c = brown;
            } else if (k < div[1]) {
                c = violet;
            } else if (k < div[2]) {
                c = red;
            } else if (k < div[3]) {
                c = redhi;
            } else if (k < div[4]) {
                c = orange;
            } else if (k < div[5]) {
                c = yellowlo;
            } else if (k < div[6]) {
                c = yellow;
            } else if (k < div[7]) {
                c = greenlo;
            } else if (k < div[8]) {
                c = green;
            } else if (k < div[9]) {
                c = greenhi;
            } else if (k < div[10]) {
                c = cyan;
            } else if (k < div[11]) {
                c = bluelo;
            } else if (k < div[12]) {
                c = blue;
            } else if (k < div[13]) {
                c = bluehi;
            } else if (k < div[14]) {
                c = magenta;
            } else if (k < 1000) {
                c = violet;
            }

            setColor(display, c.r, c.g, c.b);
            colorPixel(display, r+51, i+15);
        }
    }
}

int main(int argc, char *argv[]) {
    double x = -2.5;
    double y = -1.25;
    double w = 3.5;

    // Set up Window
    struct Display *display = createDisplay();

    // Display Empty Graph
    displayFrame(display, NULL);
    SDL_RenderPresent(display->renderer);

    // Render Start Frame (fully zoomed out)
    struct Frame *start = renderFrame(NULL, x, y, w);
    struct Frame *current = start;
    displayFrame(display, current);
    SDL_RenderPresent(display->renderer);

    // Wait for Input
    SDL_Event e;
    short running = 1;
    short zooming = 0;
    SDL_Point zoomCenter = { 0, 0 };
    SDL_Rect zoom = { 0, 0, 0, 0 };
    while (running) {
        if (!SDL_WaitEvent(&e)) continue;
        if (e.type == SDL_QUIT) {
            running = 0;
        } else if (e.type == SDL_KEYDOWN) {
            if (e.key.keysym.sym == SDLK_ESCAPE) {
                running = 0;
            } else if (e.key.keysym.sym == SDLK_LEFT) {
                if (current->parent) {
                    current = current->parent;
                    displayFrame(display, current);
                    SDL_RenderPresent(display->renderer);
                }
            } else if (e.key.keysym.sym == SDLK_RIGHT) {
                if (current->child) {
                    current = current->child;
                    displayFrame(display, current);
                    SDL_RenderPresent(display->renderer);
                }
            } else if (e.key.keysym.sym == SDLK_EQUALS) {
                printf("plus\n");
            } else if (e.key.keysym.sym == SDLK_MINUS) {
                printf("minus\n");
            } else if (e.key.keysym.sym == SDLK_m) {
                useMin = !useMin;
                displayFrame(display, current);
                SDL_RenderPresent(display->renderer);
            }
        } else if (e.type == SDL_MOUSEBUTTONDOWN) {
            // Set zoom center
            zoomCenter.x = e.button.x;
            zoomCenter.y = e.button.y;
            zooming = 1;
        } else if (e.type == SDL_MOUSEMOTION) {
            if (!zooming) continue;

            // Compute zoom rect
            int diffX = abs(zoomCenter.x - e.motion.x);
            int diffY = (int) (((float) FRAME_HEIGHT / FRAME_WIDTH) * diffX);
            if (e.motion.y > zoomCenter.y + diffY || e.motion.y < zoomCenter.y - diffY) {
                diffY = abs(zoomCenter.y - e.motion.y);
                diffX = (int) (((float) FRAME_WIDTH / FRAME_HEIGHT) * diffY);
            }
            zoom.x = zoomCenter.x - diffX;
            zoom.y = zoomCenter.y - diffY;
            zoom.w = diffX * 2;
            zoom.h = diffY * 2;
            
            displayFrame(display, current);
            setColor(display, 255, 255, 255);
            SDL_RenderDrawRect(display->renderer, &zoom);
            SDL_RenderPresent(display->renderer);
        } else if (e.type == SDL_MOUSEBUTTONUP) {
            // Compute new frame parameters

            // Start with pixel coordinates relative to the frame origin
            double x = (double) zoom.x - FRAME_ORIGIN.x;
            double y = (double) zoom.y + zoom.h - FRAME_ORIGIN.y;
            double w = (double) zoom.w;
            double gap = current->w / FRAME_WIDTH;

            // Convert pixel coordinates to frame coordinates
            x = current->x + gap*x;
            y = current->y + gap*(FRAME_HEIGHT - y);
            w = gap*w;


            if (w != 0.0) {
                if (current->child) freeFrame(current->child);
                current->child = renderFrame(current, x, y, w);
                current = current->child;
                displayFrame(display, current);
                SDL_RenderPresent(display->renderer);
            }
            
            // Reset
            zooming = 0;
            zoom.x = 0;
            zoom.y = 0;
            zoom.w = 0;
            zoom.h = 0;
        }
    }

    // Clean Up and Exit
    destroyDisplay(display);
    display = 0;
    freeFrame(start);
    start = 0;
    current = 0;

    return 0;
}

struct Frame* renderFrame(struct Frame *parent, double originX, double originY, double frameWidth) {
    struct Frame *frame = malloc(sizeof(struct Frame));
    frame->parent = parent;
    frame->child = NULL;
    frame->x = originX;
    frame->y = originY;
    frame->w = frameWidth;

    // Render frame
    double gap = frameWidth / FRAME_WIDTH;
    double r = originX + 4*gap;
    frame->min = 1000; // Initialize to maximum k value

    for (short x = 0; x <= 578; x++) { // X-axis is the real axis
        r = r + gap;
        double i = frame->y + 404*gap;
        
        for (short y = 0; y <= 404; y++) {
            i = i - gap;
            struct Complex z = { 0.0, 0.0 };
            struct Complex c = { r, i };
            double magnitude;
            
            short k;
            for (k = 0; k <= 1000; k++) {
                /*
                 * Mandelbrot Equation: Zn+1 = Zn^2 + C
                 */
                z = cadd(cmul(z,z), c);

                /**
                 * Absolute Value of Z, |Z|.
                 * Absolute Value of N can be formulated as sqrt(N^2)
                 * For Complex Numbers, this essentially becomes the Distance Formula.
                 * Distance Formula: SZ^2 = R^2 + I^2
                 */
                magnitude = sqrt((z.r * z.r) + (z.i * z.i));
                if (magnitude > 2) break;
            }

            // Todo: use a function pointer to create a callback which allows
            //       the implementation of a progress bar?
            
            frame->k[x][y] = k;
            if (k < frame->min) frame->min = k;
        }
    }

    return frame;
}

void freeFrame(struct Frame *frame) {
    struct Frame *current = frame;

    // Find furthest out child
    while (current->child) current = current->child;
    
    // Free all children up to the current frame
    struct Frame *toFree;
    while (current != frame) {
        toFree = current;
        current = current->parent;
        free(toFree);
    }

    // Free the current frame
    if (frame->parent) frame->parent->child = 0;
    free(frame);
}
