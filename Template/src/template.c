#include <stdio.h>
#include <zconf.h>

#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_image.h>

#include "log.h"

#if !defined(__x86_64__)

#include "buttons.h"

#endif

#define SCREEN_WIDTH        (1920/2)
#define SCREEN_HEIGHT    (1080/2)
#define SCREEN_BPP    32

static SDL_Surface *screen = NULL;
static SDL_Event event;
static TTF_Font *font;

static void _sdl_draw() {
    SDL_Rect dstrect = {225, 150, 0, 0};
    SDL_Surface *sc_buttons = IMG_Load("template.png");
    if (sc_buttons) SDL_BlitSurface(sc_buttons, NULL, screen, &dstrect);
    SDL_FreeSurface(sc_buttons);
}

static void _sdl_print(char *msg) {
    SDL_Color c_black = {45, 56, 74};
    SDL_Color c_yellow = {255, 255, 0};
    SDL_Rect dstrect = {20, 20, 0, 0};

    SDL_Surface *btsu = TTF_RenderUTF8_Shaded(font, msg, c_yellow, c_black);
    SDL_BlitSurface(btsu, NULL, screen, &dstrect);//draw on the screen

    SDL_FreeSurface(btsu);  //remove surface before exit //!!!!!!!
    SDL_Flip(screen);
}

static void _sdl_del(void) {
    _sdl_print(
            "                                                                                                                                      ");//empty lines to delete
}

static int InitSDL(SDL_Surface **screen) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0 || TTF_Init() < 0 || IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG | IMG_INIT_TIF) < 0)
        return 1;
    SDL_EnableUNICODE(0x1);
    SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);
    if (!(*screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, 0)))
        return 2;
    if (!(font = TTF_OpenFont("shadow.ttf", 14)))
        return 3;
    SDL_FillRect(*screen, &(SDL_Rect) {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT}, SDL_MapRGB((*screen)->format, 45, 56, 74));
    SDL_Flip(*screen);
    return 0;
}


int _Game_Main_(const char *path, const char *udn __attribute__ ((unused))) __attribute__ ((noinline));

int _Game_Main_(const char *path, const char *udn __attribute__ ((unused))) {
    debug("Started on path %s", path);
    if (InitSDL(&screen)) {
        debug("Error starting SDL %s", SDL_GetError());
        return 1;
    }
    _sdl_draw();
    uint8_t exit = 0;
    while (!exit) {
        if (SDL_PollEvent(&event)) {
            if (event.type == SDL_KEYDOWN) {
                _sdl_del();
                switch (event.key.keysym.sym) {
                    case SDLK_RETURN:
                        _sdl_print("Enter");
                        break;
                    case SDLK_UP:
                        _sdl_print("UP");
                        break;
                    case SDLK_DOWN:
                        _sdl_print("DOWN");
                        break;
                    case SDLK_RIGHT:
                        _sdl_print("RIGHT");
                        break;
                    case SDLK_LEFT:
                        _sdl_print("LEFT");
                        break;
                    case SDLK_BACKSPACE:
                    case SDLK_ESCAPE:
                    case SDLK_F10:
                        exit = 0x1;
                        break;
                    default:
                        break;
                }
            }
        }
        SDL_Flip(screen);
        SDL_Delay(1);
    }
    SDL_FreeSurface(screen);
    SDL_Quit();
    return 0;
}

#if !defined(__x86_64__)

static int button_pressed(int key) __attribute__ ((noinline));

static int button_pressed(const int key) {
    SDL_Event sdl_event;
    get_sdl_event(&sdl_event, key);
    SDL_PushEvent(&sdl_event);
    return (key);        // return destination button code to be processed by exeDSP
}

void injection(int a, int key, int arg_r2) __attribute__ ((noinline));

void injection(int a, int key, int arg_r2) {
    // CAUTION! Do not add/modify code here... or modify injection indexes in loader

    // [0][1] gcc should put "STMFD SP!, {R4,LR}" and "MOV R4,[R1]" here
    asm volatile(
    "STMFD   SP!, {R0,R2,R3,R12}\n"        // [2] store some registers
    );

    // first, we have to execute our function
    button_pressed(key);                // [3][4] calling local function with key variable in R0, returning R0

    // then, prepare for calling and call previous injection function
    asm volatile(
    "MOV     R1, R0\n"            // [5] remapped keycode
            "MOV     R6, R0\n"            // [6] remapped keycode
            "LDMFD   SP!, {R0,R2,R3,R12}\n"        // [7] restore registers stored before
            "LDMFD   SP!, {R4,LR}\n"        // [8] restore registers...
            "MOV     R7, R2\n"            // [9] This is original command that we overwrite (and replace at next injection)
            "LDR     R3, [R8]\n"            // [10] This is original command that we overwrite (and replace at next injection)
            "LDR     PC, =(_ZN9KeyCommon17SendKeyPressInputEii + 0x18 )\n"    // [11] return after injection (not reached at next injection)
    );
    // and that's all; the rest (returning to exeDSP) is already programmed in the first injected function
}

#endif

#if defined(__x86_64__)

int main(int argc __attribute__ ((unused)), char *argv[] __attribute__ ((unused))) {
    char cur_path[256];
    if (getcwd(cur_path, sizeof(cur_path)) == NULL) strcpy(cur_path, "/");
    _Game_Main_(cur_path, "");
    return 0;
}

#endif