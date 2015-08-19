//
// Created by aen on 19.8.2015.
//

#include "fonts.h"
#include "commons.h"

SDL_Surface **FONT_img;
bool init = false;
int fontMax = 0, fontMin = 0;

bool init_Font() {
    DIR *d = NULL;
    struct dirent *dir = NULL;
    if ((d = opendir(FontDir))) {
        fontMax = 0;
        while ((dir = readdir(d)) != NULL)
            if (strstr(dir->d_name, ".png"))
                fontMax++;
        closedir(d);
        fontMax -= fontMin - 1;
        FONT_img = calloc(fontMax, sizeof(char *));
        init = true;
        return true;
    }
    return false;
}


void clean_font() {
    while (fontMax > 0) {
        free(FONT_img[fontMax]);
        fontMax--;
    }

}


int print_char(unsigned long code, int x, int y, int size, SDL_Surface *screen) {
    if (code == ' ')return 4 * SPACING; else if (code == '\t')return 8 * SPACING; else if (
            code >= (unsigned int) fontMin && code <= (unsigned int) fontMax) {
        if (FONT_img[code - fontMin] == NULL) {
            char *p = malloc(sizeof(char) * (25));
            p[0] = '\0';
            strcat(p, FontDir);
            sprintf(p + 10, "0X%04lX", code);
            p[17] = '\0';
            strcat(p, ".png");
            FONT_img[code - fontMin] = IMG_Load(p);
            free(p);
        }
        if (FONT_img[code - fontMin] != NULL) {
            switch (code) {
                case '-':
                    SDL_BlitSurface(FONT_img[code - fontMin], NULL, screen,
                                    &(SDL_Rect) {x, y + (size - FONT_img[code - fontMin]->h) - size * 0.25f, 0, 0});
                    break;
                case 'p':
                case 'g':
                case 'j':
                case 'q':
                case 'y':
                case 253:
                    SDL_BlitSurface(FONT_img[code - fontMin], NULL, screen,
                                    &(SDL_Rect) {x, y + (size - FONT_img[code - fontMin]->h) + size * 0.275f, 0, 0});
                    break;
                case 'J':
                case 'Q':
                    SDL_BlitSurface(FONT_img[code - fontMin], NULL, screen,
                                    &(SDL_Rect) {x, y + (size - FONT_img[code - fontMin]->h) + size * 0.2f, 0, 0});
                    break;
                default:
                    SDL_BlitSurface(FONT_img[code - fontMin], NULL, screen,
                                    &(SDL_Rect) {x, y + (size - FONT_img[code - fontMin]->h), 0, 0});
                    break;
            }
            return FONT_img[code - fontMin]->w + SPACING;
        }
    }
    return 0;
}


void sdl_print(char *line, int x, int y, SDL_Surface *screen) {
    if (!init) if (!init_Font()) return;
    if (line == NULL)return;
    while (*line != '\0') {
        if (((*line >> 7) & 1) == 0) {
            x += print_char(*line, x, y, SIZE, screen);
        } else {
            unsigned long code = 0;
            int i = 0, seq = 0, bit = 31;
            for (i = 7; i >= 0; --i) {
                if (((*line >> i) & 1) == 0 && seq == 0) {
                    seq = 7 - i;
                } else if (seq != 0) {
                    code |= ((*line >> i) & 1) << bit;
                    bit--;
                }
            }
            do {
                line++;
                for (i = 5; i >= 0; --i) {
                    code |= ((*line >> i) & 1) << bit;
                    bit--;
                }
                seq--;
            } while (seq > 1);
            while (bit >= 0) {
                code *= 0.5;
                bit--;
            }
            x += print_char(code, x, y, SIZE, screen);
        }
        line++;
    }
}