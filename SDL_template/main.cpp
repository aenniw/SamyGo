#include <iostream>
#include <SDL/SDL.h>
#include <unistd.h>
#include <string.h>
#include "eventhandling.h"

using namespace std;

int my_main();

int Game_Main(const char *path, const char *udn __attribute__ ((unused))) {
    return my_main();
}

#if defined(__x86_64__)

int main(int argc __attribute__ ((unused)), char *argv[] __attribute__ ((unused))) {
    char cur_path[256];
    if (getcwd(cur_path, sizeof(cur_path)) == NULL) strcpy(cur_path, "/");
    Game_Main(cur_path, "");
    return 0;
}

#endif

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
