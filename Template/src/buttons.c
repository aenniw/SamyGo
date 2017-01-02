#include "buttons.h"

void get_sdl_event(SDL_Event *event, int key) {
    event->type = SDL_KEYDOWN;
    switch (key) {
        case KEY_NUM0:
            event->key.keysym.sym = SDLK_KP0;
            break;
        case KEY_NUM1:
            event->key.keysym.sym = SDLK_KP1;
            break;
        case KEY_NUM2:
            event->key.keysym.sym = SDLK_KP2;
            break;
        case KEY_NUM3:
            event->key.keysym.sym = SDLK_KP3;
            break;
        case KEY_NUM4:
            event->key.keysym.sym = SDLK_KP4;
            break;
        case KEY_NUM5:
            event->key.keysym.sym = SDLK_KP5;
            break;
        case KEY_NUM6:
            event->key.keysym.sym = SDLK_KP6;
            break;
        case KEY_NUM7:
            event->key.keysym.sym = SDLK_KP7;
            break;
        case KEY_NUM8:
            event->key.keysym.sym = SDLK_KP8;
            break;
        case KEY_NUM9:
            event->key.keysym.sym = SDLK_KP9;
            break;
        case KEY_EXIT:
            event->key.keysym.sym = SDLK_ESCAPE;
            break;
        case KEY_ENTER:
            event->key.keysym.sym = SDLK_RETURN;
            break;
        case KEY_CUR_UP:
            event->key.keysym.sym = SDLK_UP;
            break;
        case KEY_CUR_DOWN:
            event->key.keysym.sym = SDLK_DOWN;
            break;
        case KEY_CUR_LEFT:
            event->key.keysym.sym = SDLK_LEFT;
            break;
        case KEY_CUR_RIGHT:
            event->key.keysym.sym = SDLK_RIGHT;
            break;
        default:
            event->type = TV_EVENT;
            event->user.code = key;
            break;
    }
}