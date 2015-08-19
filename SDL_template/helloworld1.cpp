#include <iostream>
#include <SDL/SDL.h>
#include "eventhandling.h"

using namespace std;

int my_main();

int Game_Main(const char *path, const char *udn __attribute__ ((unused))) {
    return my_main();
}

int main(int argc, char *argv[]) {
    return my_main();
}


int my_main() {
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_Surface *screen = SDL_SetVideoMode(1920, 1080, 32, SDL_HWSURFACE + SDL_FULLSCREEN + SDL_DOUBLEBUF);
    //SDL_Surface* screen = SDL_SetVideoMode(1024,800, 32, SDL_HWSURFACE+SDL_DOUBLEBUF);
    EventHandling handl;
    handl.initLog("keys.txt");
    while (handl.HandleEvents()) {

        SDL_WaitEvent(handl.getEvent());
        SDL_UpdateRect(screen, 0, 0, 0, 0);
    }
    SDL_FreeSurface(screen);
    SDL_Quit();
    return 0;
}
