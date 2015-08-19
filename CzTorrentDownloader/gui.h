//
// Created by aen on 19.8.2015.
//

#ifndef CZTORRENTDOWNLOADER_GUI_H
#define CZTORRENTDOWNLOADER_GUI_H

#include <stdbool.h>
#include "cztorrent.h"
#include <SDL/SDL_video.h>
#include "fonts.h"

enum Screens {
    Find, Selection, TYPING
};
enum selectable {
    TEXT, VYHLADAT, FILMY, HUDBA, SERIALY, _1, _2, _3, _4, _5, _6, _7, _8, _9, _B, _0, _OK
};

enum buttons {
    SAVE = 0,
    NSAVE,
    LOGO,
    SEARCH,
    VYHLADAT0,
    VYHLADAT1,
    HUDBA0,
    HUDBA1,
    SERIALY0,
    SERIALY1,
    FILMY0,
    FILMY1,
    CHECK0,
    CHECK1,
    MESS,
    _10,
    _11,
    _20,
    _21,
    _30,
    _31,
    _40,
    _41,
    _50,
    _51,
    _60,
    _61,
    _70,
    _71,
    _80,
    _81,
    _90,
    _91,
    _B0,
    _B1,
    _00,
    _01,
    _OK0,
    _OK1
};


void GUI_init();

void GUI_clean();

void GUI_drawMessageBox(char *text, SDL_Surface *screen);

bool GUI_events(SDL_Surface *screen, Torrents *torrents, char *dir, CURL *myHandle);

#endif //CZTORRENTDOWNLOADER_GUI_H
