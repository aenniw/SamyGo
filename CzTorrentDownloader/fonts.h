#ifndef CZTORRENTDOWNLOADER_FONTS_H

#define CZTORRENTDOWNLOADER_FONTS_H
#define FontDir "./font/28/"

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <stdbool.h>
#include <dirent.h>


bool init_Font();

void clean_font();

int print_char(unsigned long code, int x, int y, int size, SDL_Surface *screen);

void sdl_print(char *line, int x, int y, SDL_Surface *screen);

#endif //CZTORRENTDOWNLOADER_FONTS_H
