#include <stdio.h>
#include <SDL2/SDL.h>

#if defined SDL_VERSION
void setupGraphics(char*, int, int);
void cleanupGraphics();
void cleanupAndExit(char*, const char*); 
void waitForExit();
void newFrame();
void setColor(Uint8, Uint8, Uint8);
void setPosition(int, int);
void printText(char*);
void drawLineAndSetPosition(int, int);
void renderFrame(char*, char*, int, int, int, int);

SDL_Window *window = 0;
SDL_Renderer *renderer = 0;
TTF_Font *font = 0;
SDL_Color color = { 0x00, 0x00, 0x00, 0xFF };
SDL_Point position = { 0, 0 };
#endif

#if defined SDL_VERSION
void setupGraphics(char *title, int width, int height) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        cleanupAndExit("Unable to initialize SDL", SDL_GetError());
    }
    if (TTF_Init() != 0) {
        cleanupAndExit("Unable to initialize SDL_ttf", TTF_GetError());
    }
    window = SDL_CreateWindow(
        title,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        width,
        height,
        0
    );
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);

    font = TTF_OpenFont("freefont-ttf/sfd/FreeSans.ttf", 24);
    if (!font) {
        cleanupAndExit("Unable to load font.", TTF_GetError());
    }
}
void cleanupGraphics() {
    if (window) {
        SDL_DestroyWindow(window);
        window = 0;
    }
    if (renderer) {
        SDL_DestroyRenderer(renderer);
        renderer = 0;
    }
    if (font) {
        TTF_CloseFont(font);
        font = 0;
    }
    SDL_Quit();
}
void cleanupAndExit(char *message, const char *errMessage) {
    printf("%s: %s\n", message, errMessage);
    cleanupGraphics();
    printf("Exiting.\n");
    exit(0);
}
void waitForExit() {
    SDL_Event e;
    int waiting = 1;
    while (waiting) {
        if (!SDL_PollEvent(&e)) continue;
        if (e.type == SDL_QUIT) {
            waiting = 0;
        } else if (e.type == SDL_KEYDOWN) {
            if (e.key.keysym.sym == SDLK_ESCAPE) {
                waiting = 0;
            }
        }
    }
}
void newFrame() {
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
    SDL_RenderClear(renderer);
}
void setColor(Uint8 r, Uint8 g, Uint8 b) {
    color.r = r;
    color.g = g;
    color.b = b;
    SDL_SetRenderDrawColor(renderer, r, g, b, 255);
}
void setPosition(int x, int y) {
    position.x = x;
    position.y = y;
}
void printText(char *text) {
    // Render text
    SDL_Surface *textSurface = TTF_RenderText_Solid(
        font,
        text,
        color
    );
    if (!textSurface) {
        cleanupAndExit("Unable to render text.", TTF_GetError());
    }
    SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    if (!textTexture) {
        cleanupAndExit("Unable create a text texture.", TTF_GetError());
    }

    // Draw rendered text to screen buffer
    SDL_Rect dest = { position.x, position.y, textSurface->w, textSurface->h };
    SDL_RenderCopy(renderer, textTexture, 0, &dest);

    // Clean up
    SDL_DestroyTexture(textTexture);
    SDL_FreeSurface(textSurface);
}
void drawLineAndSetPosition(int x, int y) {
    SDL_RenderDrawLine(renderer, position.x, position.y, x, y);
    setPosition(x, y);
}
#endif
