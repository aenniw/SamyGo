//
// Created by aen on 19.8.2015.
//
#include "gui.h"


const unsigned int MAX_TORRENTS = 8;
Torrent *Selected_Torrent, *top, *bottom;
enum Screens activeScreen = Find;
char *findPattern;
const char *imgPaths[] = {"./img/diskette.png",
                          "./img/diskette0.png",
                          "./img/logo.png",
                          "./img/search.png",
                          "./img/vyhladat_0.png",
                          "./img/vyhladat_1.png",
                          "./img/hudba_0.png",
                          "./img/hudba_1.png",
                          "./img/serialy_0.png",
                          "./img/serialy_1.png",
                          "./img/filmy_0.png",
                          "./img/filmy_1.png",
                          "./img/checkbox_unchecked.png",
                          "./img/checkbox_checked.png",
                          "./img/message.png",
                          "./img/1_0.png",
                          "./img/1_1.png",
                          "./img/2_0.png",
                          "./img/2_1.png",
                          "./img/3_0.png",
                          "./img/3_1.png",
                          "./img/4_0.png",
                          "./img/4_1.png",
                          "./img/5_0.png",
                          "./img/5_1.png",
                          "./img/6_0.png",
                          "./img/6_1.png",
                          "./img/7_0.png",
                          "./img/7_1.png",
                          "./img/8_0.png",
                          "./img/8_1.png",
                          "./img/9_0.png",
                          "./img/9_1.png",
                          "./img/B_0.png",
                          "./img/B_1.png",
                          "./img/0_0.png",
                          "./img/0_1.png",
                          "./img/OK_0.png",
                          "./img/OK_1.png"};
enum selectable Selected_Button = -1;
SDL_Surface *GUI_img[_OK1 + 1];
bool checkBoxEnabled[3];
double maxDiff = 1.25;
time_t lastPush;

void GUI_init() {
    int i = 0;
    for (i = SAVE; i <= _OK1; i++) {
        GUI_img[i] = IMG_Load(imgPaths[i]);
        if (GUI_img[i] == NULL)
            fprintf(stdout, "%s", IMG_GetError());
    }
    for (i = 0; i < 3; i++)
        checkBoxEnabled[i] = false;
    lastPush = time(NULL);
}

void GUI_clean() {
    int i = 0;
    for (i = SAVE; i <= FILMY1; i++)
        SDL_FreeSurface(GUI_img[i]);
}

void GUI_drawMessageBox(char *text, SDL_Surface *screen) {
    SDL_BlitSurface(GUI_img[MESS], NULL, screen,
                    &(SDL_Rect) {SCREEN_WIDTH / 2 - GUI_img[MESS]->w / 2, SCREEN_HEIGHT / 2 - GUI_img[MESS]->h / 2,
                                 0, 0});
    sdl_print(text, SCREEN_WIDTH / 2 - 125, SCREEN_HEIGHT / 2 - 25, screen);
    SDL_Flip(screen);

}

void GUI_findTorrent(Torrents *torrents, CURL *myHandle) {
    freeTorrents(torrents);
    findTorrent(findPattern, myHandle);
    getTorrents(torrents);
    top = torrents->begin;
    Torrent *tmp = top;
    unsigned int i = 1;
    if (tmp != NULL)
        for (i = 1; i < MAX_TORRENTS; i++)
            if (tmp->next != NULL)
                tmp = tmp->next;
    bottom = tmp;
    Selected_Torrent = top;
}

void redrawKeys(SDL_Surface *screen) {
    float x = SCREEN_WIDTH / 2 + 1.5 * GUI_img[VYHLADAT0]->w, y =
            SCREEN_HEIGHT / 2 - GUI_img[VYHLADAT0]->h - 4, yOffsite = -43, xOffsite = -10;
    SDL_FillRect(screen,
                 &(SDL_Rect) {xOffsite + x, yOffsite + y, 3 * (GUI_img[_00]->w + 2) + 2, 4 * (GUI_img[_00]->h + 2) + 2},
                 SDL_MapRGB(screen->format, 45, 56, 74));
    unsigned int i = 0, mod = _10 % 3, line = -1, collum = 0, button = _1;
    for (i = _10; i < _OK1; i += 2) {
        if (i % 3 == mod) {
            line++;
            collum = 0;
        }
        if (Selected_Button == button)
            SDL_BlitSurface(GUI_img[i + 1], NULL, screen,
                            &(SDL_Rect) {xOffsite + 2 + x + (2 + GUI_img[i + 1]->w) * collum,
                                         yOffsite + 2 + y + (2 + GUI_img[i + 1]->h) * line, 0, 0});
        else
            SDL_BlitSurface(GUI_img[i], NULL, screen, &(SDL_Rect) {xOffsite + 2 + x + (2 + GUI_img[i]->w) * collum,
                                                                   yOffsite + 2 + y + (2 + GUI_img[i]->h) * line, 0,
                                                                   0});
        collum++;
        button++;
    }

}

void redrawCheckBox(SDL_Surface *screen) {
    SDL_FillRect(screen, &(SDL_Rect) {SCREEN_WIDTH / 2 - 200 - GUI_img[CHECK1]->w - 5,
                                      SCREEN_HEIGHT / 2 + GUI_img[FILMY0]->h / 2 + 2, GUI_img[CHECK1]->w,
                                      GUI_img[CHECK1]->h}, SDL_MapRGB(screen->format, 242, 243, 249));
    if (checkBoxEnabled[0])
        SDL_BlitSurface(GUI_img[CHECK1], NULL, screen, &(SDL_Rect) {SCREEN_WIDTH / 2 - 200 - GUI_img[CHECK1]->w - 5,
                                                                    SCREEN_HEIGHT / 2 + GUI_img[FILMY0]->h / 2 + 2, 0,
                                                                    0});
    else
        SDL_BlitSurface(GUI_img[CHECK0], NULL, screen, &(SDL_Rect) {SCREEN_WIDTH / 2 - 200 - GUI_img[CHECK0]->w - 5,
                                                                    SCREEN_HEIGHT / 2 + GUI_img[FILMY0]->h / 2 + 2, 0,
                                                                    0});
    SDL_FillRect(screen, &(SDL_Rect) {SCREEN_WIDTH / 2 - 160 + GUI_img[HUDBA0]->w + 15 - GUI_img[CHECK1]->w - 5,
                                      SCREEN_HEIGHT / 2 + GUI_img[HUDBA0]->h / 2 + 2, GUI_img[CHECK1]->w,
                                      GUI_img[CHECK1]->h}, SDL_MapRGB(screen->format, 242, 243, 249));
    if (checkBoxEnabled[1])
        SDL_BlitSurface(GUI_img[CHECK1], NULL, screen,
                        &(SDL_Rect) {SCREEN_WIDTH / 2 - 160 + GUI_img[HUDBA0]->w + 15 - GUI_img[CHECK1]->w - 5,
                                     SCREEN_HEIGHT / 2 + GUI_img[HUDBA0]->h / 2 + 2, 0, 0});
    else
        SDL_BlitSurface(GUI_img[CHECK0], NULL, screen,
                        &(SDL_Rect) {SCREEN_WIDTH / 2 - 160 + GUI_img[HUDBA0]->w + 15 - GUI_img[CHECK0]->w - 5,
                                     SCREEN_HEIGHT / 2 + GUI_img[HUDBA0]->h / 2 + 2, 0, 0});
    SDL_FillRect(screen, &(SDL_Rect) {
                         SCREEN_WIDTH / 2 - 120 + GUI_img[HUDBA0]->w + GUI_img[SERIALY0]->w + 30 - GUI_img[CHECK1]->w -
                         5,
                         SCREEN_HEIGHT / 2 + GUI_img[SERIALY0]->h / 2 + 2, GUI_img[CHECK1]->w, GUI_img[CHECK1]->h},
                 SDL_MapRGB(screen->format, 242, 243, 249));
    if (checkBoxEnabled[2])
        SDL_BlitSurface(GUI_img[CHECK1], NULL, screen, &(SDL_Rect) {
                SCREEN_WIDTH / 2 - 120 + GUI_img[HUDBA0]->w + GUI_img[SERIALY0]->w + 30 - GUI_img[CHECK1]->w - 5,
                SCREEN_HEIGHT / 2 + GUI_img[SERIALY0]->h / 2 + 2, 0, 0});
    else
        SDL_BlitSurface(GUI_img[CHECK0], NULL, screen, &(SDL_Rect) {
                SCREEN_WIDTH / 2 - 120 + GUI_img[HUDBA0]->w + GUI_img[SERIALY0]->w + 30 - GUI_img[CHECK0]->w - 5,
                SCREEN_HEIGHT / 2 + GUI_img[SERIALY0]->h / 2 + 2, 0, 0});
}

void redrawScreen(SDL_Surface *screen) {
    SDL_FillRect(screen, &(SDL_Rect) {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT}, SDL_MapRGB(screen->format, 45, 56, 74));
    if (activeScreen == Find) {
        SDL_BlitSurface(GUI_img[SEARCH], NULL, screen, &(SDL_Rect) {SCREEN_WIDTH / 2 - GUI_img[SEARCH]->w / 2,
                                                                    SCREEN_HEIGHT / 2 - GUI_img[SEARCH]->h / 2, 0, 0});
        SDL_BlitSurface(GUI_img[LOGO], NULL, screen, &(SDL_Rect) {0, 0, 0, 0});
        SDL_BlitSurface(GUI_img[VYHLADAT0], NULL, screen, &(SDL_Rect) {SCREEN_WIDTH / 2 + 1.5 * GUI_img[VYHLADAT0]->w,
                                                                       SCREEN_HEIGHT / 2 - GUI_img[VYHLADAT0]->h - 4, 0,
                                                                       0});
        sdl_print(findPattern, 240, SCREEN_HEIGHT / 2 - 44, screen);
        SDL_BlitSurface(GUI_img[FILMY0], NULL, screen,
                        &(SDL_Rect) {SCREEN_WIDTH / 2 - 200, SCREEN_HEIGHT / 2 + GUI_img[FILMY0]->h / 2, 0, 0});
        SDL_BlitSurface(GUI_img[HUDBA0], NULL, screen, &(SDL_Rect) {SCREEN_WIDTH / 2 - 160 + GUI_img[HUDBA0]->w + 15,
                                                                    SCREEN_HEIGHT / 2 + GUI_img[HUDBA0]->h / 2, 0, 0});
        SDL_BlitSurface(GUI_img[SERIALY0], NULL, screen,
                        &(SDL_Rect) {SCREEN_WIDTH / 2 - 120 + GUI_img[HUDBA0]->w + GUI_img[SERIALY0]->w + 30,
                                     SCREEN_HEIGHT / 2 + GUI_img[SERIALY0]->h / 2, 0, 0});
        redrawCheckBox(screen);
    } else if (activeScreen == Selection) {

    }
    SDL_Flip(screen);
}

void GUI_printTorrents(SDL_Surface *screen) {
    if (top == NULL || bottom == NULL) {
        SDL_Flip(screen);
        return;
    }
    Torrent *t = top;
    int line = 1, yMargin = 20;
    while (t != bottom->next) {
        SDL_FillRect(screen, &(SDL_Rect) {5, SIZE * line + SPACING + yMargin, SCREEN_WIDTH - 5, 2 * (SIZE + SPACING)},
                     SDL_MapRGB(screen->format, 200, 200, 255));
        if (t == Selected_Torrent) {
            SDL_FillRect(screen, &(SDL_Rect) {8, 3 + SIZE * line + SPACING + yMargin, SCREEN_WIDTH - 8,
                                              2 * (SIZE + SPACING) - 6}, SDL_MapRGB(screen->format, 220, 220, 246));
            SDL_BlitSurface(GUI_img[SAVE], NULL, screen, &(SDL_Rect) {0, line * SIZE - 10 + yMargin, 0, 0});
        } else {
            SDL_FillRect(screen, &(SDL_Rect) {8, 3 + SIZE * line + SPACING + yMargin, SCREEN_WIDTH - 8,
                                              2 * (SIZE + SPACING) - 6}, SDL_MapRGB(screen->format, 220, 220, 246));
            SDL_BlitSurface(GUI_img[NSAVE], NULL, screen, &(SDL_Rect) {11, line * SIZE - 10 + yMargin + 11, 0, 0});
        }
        sdl_print(t->name, GUI_img[SAVE]->w, SIZE * line + SPACING + yMargin, screen);
        char *l = malloc(
                sizeof(char) * (9 + strlen(t->size) + strlen(t->seeds) + strlen(t->peers) + strlen(t->active)));
        l[0] = '\0';
        strcat(l, t->size);
        strcat(l, "\t\t");
        strcat(l, t->seeds);
        strcat(l, " | ");
        strcat(l, t->peers);
        strcat(l, " | ");
        strcat(l, t->active);
        line++;
        sdl_print(l, GUI_img[SAVE]->w, SIZE * line + SPACING + yMargin, screen);
        free(l);
        t = t->next;
        line += 1.25f;
    }
    SDL_FillRect(screen, &(SDL_Rect) {200, SCREEN_HEIGHT - SIZE - 2 * SPACING + 4, SCREEN_WIDTH / 3 + 4,
                                      (SIZE + 2 * SPACING) + 4}, SDL_MapRGB(screen->format, 45, 56, 74));
    SDL_Flip(screen);
}

void selectedButton(enum selectable i, SDL_Surface *screen) {
    switch (Selected_Button) {
        case VYHLADAT:
            SDL_BlitSurface(GUI_img[VYHLADAT0], NULL, screen,
                            &(SDL_Rect) {SCREEN_WIDTH / 2 + 1.5 * GUI_img[VYHLADAT0]->w,
                                         SCREEN_HEIGHT / 2 - GUI_img[VYHLADAT0]->h - 4, 0, 0});
            break;
        case HUDBA:
            SDL_BlitSurface(GUI_img[HUDBA0], NULL, screen,
                            &(SDL_Rect) {SCREEN_WIDTH / 2 - 160 + GUI_img[HUDBA0]->w + 15,
                                         SCREEN_HEIGHT / 2 + GUI_img[HUDBA0]->h / 2, 0, 0});
            break;
        case FILMY:
            SDL_BlitSurface(GUI_img[FILMY0], NULL, screen,
                            &(SDL_Rect) {SCREEN_WIDTH / 2 - 200, SCREEN_HEIGHT / 2 + GUI_img[FILMY0]->h / 2, 0, 0});
            break;
        case SERIALY:
            SDL_BlitSurface(GUI_img[SERIALY0], NULL, screen,
                            &(SDL_Rect) {SCREEN_WIDTH / 2 - 120 + GUI_img[HUDBA0]->w + GUI_img[SERIALY0]->w + 30,
                                         SCREEN_HEIGHT / 2 + GUI_img[SERIALY0]->h / 2, 0, 0});
            break;
        case TEXT:

            break;
        default:
            break;
    }
    Selected_Button = i;
    switch (Selected_Button) {
        case VYHLADAT:
            SDL_BlitSurface(GUI_img[VYHLADAT1], NULL, screen,
                            &(SDL_Rect) {SCREEN_WIDTH / 2 + 1.5 * GUI_img[VYHLADAT1]->w,
                                         SCREEN_HEIGHT / 2 - GUI_img[VYHLADAT1]->h - 4, 0, 0});
            break;
        case HUDBA:
            SDL_BlitSurface(GUI_img[HUDBA1], NULL, screen,
                            &(SDL_Rect) {SCREEN_WIDTH / 2 - 160 + GUI_img[HUDBA1]->w + 15,
                                         SCREEN_HEIGHT / 2 + GUI_img[HUDBA1]->h / 2, 0, 0});
            break;
        case FILMY:
            SDL_BlitSurface(GUI_img[FILMY1], NULL, screen,
                            &(SDL_Rect) {SCREEN_WIDTH / 2 - 200, SCREEN_HEIGHT / 2 + GUI_img[FILMY1]->h / 2, 0, 0});
            break;
        case SERIALY:
            SDL_BlitSurface(GUI_img[SERIALY1], NULL, screen,
                            &(SDL_Rect) {SCREEN_WIDTH / 2 - 120 + GUI_img[HUDBA1]->w + GUI_img[SERIALY1]->w + 30,
                                         SCREEN_HEIGHT / 2 + GUI_img[SERIALY1]->h / 2, 0, 0});
            break;
        case TEXT:

            break;
        default:
            redrawKeys(screen);
            break;
    }
    SDL_Flip(screen);
}

void buttonLogic(enum selectable i, Torrents *torrents, SDL_Surface *screen, CURL *myHandle) {
    double diff = difftime(time(NULL), lastPush);
    lastPush = time(NULL);
    switch (i) {
        case TEXT:
            break;
        case VYHLADAT:
            Selected_Button = _5;
            activeScreen = TYPING;
            redrawKeys(screen);
            SDL_Flip(screen);
            break;
        case _OK:
            activeScreen = Selection;
            redrawScreen(screen);
            SDL_FillRect(screen, &(SDL_Rect) {0, 4, SCREEN_WIDTH / 3 + 2, (SIZE + 2 * SPACING) + 4},
                         SDL_MapRGB(screen->format, 220, 220, 220));
            SDL_FillRect(screen, &(SDL_Rect) {0, 6, SCREEN_WIDTH / 3, (SIZE + 2 * SPACING)},
                         SDL_MapRGB(screen->format, 245, 248, 246));
            sdl_print("Výsledky.", strlen("Výsledky") * SIZE / 2, 4, screen);
            GUI_findTorrent(torrents, myHandle);
            GUI_printTorrents(screen);
            break;
        case HUDBA:
            setOptions(Hudba);
            checkBoxEnabled[1] = !checkBoxEnabled[1];
            redrawCheckBox(screen);
            SDL_Flip(screen);
            break;
        case FILMY:
            setOptions(Filmy);
            setOptions(Filmy_Anime);
            setOptions(Filmy_Dokumenty);
            setOptions(Filmy_Kreslene);
            checkBoxEnabled[0] = !checkBoxEnabled[0];
            redrawCheckBox(screen);
            SDL_Flip(screen);
            break;
        case SERIALY:
            setOptions(Serialy);
            checkBoxEnabled[2] = !checkBoxEnabled[2];
            redrawCheckBox(screen);
            SDL_Flip(screen);
            break;
        case _B:
            if (strlen(findPattern) > 0) {
                findPattern[strlen(findPattern) - 1] = '\0';
                SDL_FillRect(screen,
                             &(SDL_Rect) {240, SCREEN_HEIGHT / 2 - 44 + SPACING + 1, 390, SIZE + 2 * SPACING - 2},
                             SDL_MapRGB(screen->format, 255, 255, 255));
                sdl_print(findPattern, 240, SCREEN_HEIGHT / 2 - 44, screen);
                SDL_Flip(screen);
            }
            break;
        case _1:
            if (diff <= maxDiff && strlen(findPattern) > 0) {
                char c = findPattern[strlen(findPattern) - 1];
                switch (c) {
                    case '.':
                        findPattern[strlen(findPattern) - 1] = ',';
                        break;
                    case ',':
                        findPattern[strlen(findPattern) - 1] = '!';
                        break;
                    case '!':
                        findPattern[strlen(findPattern) - 1] = '?';
                        break;
                    case '?':
                        findPattern[strlen(findPattern) - 1] = '0';
                        break;
                    case '0':
                        findPattern[strlen(findPattern) - 1] = '.';
                        break;
                }
            } else if (strlen(findPattern) < 20) {
                findPattern[strlen(findPattern)] = '0';
                findPattern[strlen(findPattern) + 1] = '\0';
            }
            SDL_FillRect(screen, &(SDL_Rect) {240, SCREEN_HEIGHT / 2 - 44 + SPACING + 1, 390, SIZE + 2 * SPACING - 2},
                         SDL_MapRGB(screen->format, 255, 255, 255));
            sdl_print(findPattern, 240, SCREEN_HEIGHT / 2 - 44, screen);
            SDL_Flip(screen);
            break;
        case _2:
            if (diff <= maxDiff && strlen(findPattern) > 0) {
                char c = findPattern[strlen(findPattern) - 1];
                switch (c) {
                    case '2':
                        findPattern[strlen(findPattern) - 1] = 'A';
                        break;
                    case 'C':
                        findPattern[strlen(findPattern) - 1] = '2';
                        break;
                    default:
                        findPattern[strlen(findPattern) - 1]++;
                        break;
                }
            } else if (strlen(findPattern) < 20) {
                findPattern[strlen(findPattern)] = 'A';
                findPattern[strlen(findPattern) + 1] = '\0';
            }
            SDL_FillRect(screen, &(SDL_Rect) {240, SCREEN_HEIGHT / 2 - 44 + SPACING + 1, 390, SIZE + 2 * SPACING - 2},
                         SDL_MapRGB(screen->format, 255, 255, 255));
            sdl_print(findPattern, 240, SCREEN_HEIGHT / 2 - 44, screen);
            SDL_Flip(screen);
            break;
        case _3:
            if (diff <= maxDiff && strlen(findPattern) > 0) {
                char c = findPattern[strlen(findPattern) - 1];
                switch (c) {
                    case '3':
                        findPattern[strlen(findPattern) - 1] = 'D';
                        break;
                    case 'F':
                        findPattern[strlen(findPattern) - 1] = '3';
                        break;
                    default:
                        findPattern[strlen(findPattern) - 1]++;
                        break;
                }
            } else if (strlen(findPattern) < 20) {
                findPattern[strlen(findPattern)] = 'D';
                findPattern[strlen(findPattern) + 1] = '\0';
            }
            SDL_FillRect(screen, &(SDL_Rect) {240, SCREEN_HEIGHT / 2 - 44 + SPACING + 1, 390, SIZE + 2 * SPACING - 2},
                         SDL_MapRGB(screen->format, 255, 255, 255));
            sdl_print(findPattern, 240, SCREEN_HEIGHT / 2 - 44, screen);
            SDL_Flip(screen);
            break;
        case _4:
            if (diff <= maxDiff && strlen(findPattern) > 0) {
                char c = findPattern[strlen(findPattern) - 1];
                switch (c) {
                    case '4':
                        findPattern[strlen(findPattern) - 1] = 'G';
                        break;
                    case 'I':
                        findPattern[strlen(findPattern) - 1] = '4';
                        break;
                    default:
                        findPattern[strlen(findPattern) - 1]++;
                        break;
                }
            } else if (strlen(findPattern) < 20) {
                findPattern[strlen(findPattern)] = 'G';
                findPattern[strlen(findPattern) + 1] = '\0';
            }
            SDL_FillRect(screen, &(SDL_Rect) {240, SCREEN_HEIGHT / 2 - 44 + SPACING + 1, 390, SIZE + 2 * SPACING - 2},
                         SDL_MapRGB(screen->format, 255, 255, 255));
            sdl_print(findPattern, 240, SCREEN_HEIGHT / 2 - 44, screen);
            SDL_Flip(screen);
            break;
        case _5:
            if (diff <= maxDiff && strlen(findPattern) > 0) {
                char c = findPattern[strlen(findPattern) - 1];
                switch (c) {
                    case '5':
                        findPattern[strlen(findPattern) - 1] = 'J';
                        break;
                    case 'L':
                        findPattern[strlen(findPattern) - 1] = '5';
                        break;
                    default:
                        findPattern[strlen(findPattern) - 1]++;
                        break;
                }
            } else if (strlen(findPattern) < 20) {
                findPattern[strlen(findPattern)] = 'J';
                findPattern[strlen(findPattern) + 1] = '\0';
            }
            SDL_FillRect(screen, &(SDL_Rect) {240, SCREEN_HEIGHT / 2 - 44 + SPACING + 1, 390, SIZE + 2 * SPACING - 2},
                         SDL_MapRGB(screen->format, 255, 255, 255));
            sdl_print(findPattern, 240, SCREEN_HEIGHT / 2 - 44, screen);
            SDL_Flip(screen);
            break;
        case _6:
            if (diff <= maxDiff && strlen(findPattern) > 0) {
                char c = findPattern[strlen(findPattern) - 1];
                switch (c) {
                    case '6':
                        findPattern[strlen(findPattern) - 1] = 'M';
                        break;
                    case 'O':
                        findPattern[strlen(findPattern) - 1] = '6';
                        break;
                    default:
                        findPattern[strlen(findPattern) - 1]++;
                        break;
                }
            } else if (strlen(findPattern) < 20) {
                findPattern[strlen(findPattern)] = 'M';
                findPattern[strlen(findPattern) + 1] = '\0';
            }
            SDL_FillRect(screen, &(SDL_Rect) {240, SCREEN_HEIGHT / 2 - 44 + SPACING + 1, 390, SIZE + 2 * SPACING - 2},
                         SDL_MapRGB(screen->format, 255, 255, 255));
            sdl_print(findPattern, 240, SCREEN_HEIGHT / 2 - 44, screen);
            SDL_Flip(screen);
            break;
        case _7:
            if (diff <= maxDiff && strlen(findPattern) > 0) {
                char c = findPattern[strlen(findPattern) - 1];
                switch (c) {
                    case '7':
                        findPattern[strlen(findPattern) - 1] = 'P';
                        break;
                    case 'S':
                        findPattern[strlen(findPattern) - 1] = '7';
                        break;
                    default:
                        findPattern[strlen(findPattern) - 1]++;
                        break;
                }
            } else if (strlen(findPattern) < 20) {
                findPattern[strlen(findPattern)] = 'P';
                findPattern[strlen(findPattern) + 1] = '\0';
            }
            SDL_FillRect(screen, &(SDL_Rect) {240, SCREEN_HEIGHT / 2 - 44 + SPACING + 1, 390, SIZE + 2 * SPACING - 2},
                         SDL_MapRGB(screen->format, 255, 255, 255));
            sdl_print(findPattern, 240, SCREEN_HEIGHT / 2 - 44, screen);
            SDL_Flip(screen);
            break;
        case _8:
            if (diff <= maxDiff && strlen(findPattern) > 0) {
                char c = findPattern[strlen(findPattern) - 1];
                switch (c) {
                    case '8':
                        findPattern[strlen(findPattern) - 1] = 'T';
                        break;
                    case 'V':
                        findPattern[strlen(findPattern) - 1] = '8';
                        break;
                    default:
                        findPattern[strlen(findPattern) - 1]++;
                        break;
                }
            } else if (strlen(findPattern) < 20) {
                findPattern[strlen(findPattern)] = 'T';
                findPattern[strlen(findPattern) + 1] = '\0';
            }
            SDL_FillRect(screen, &(SDL_Rect) {240, SCREEN_HEIGHT / 2 - 44 + SPACING + 1, 390, SIZE + 2 * SPACING - 2},
                         SDL_MapRGB(screen->format, 255, 255, 255));
            sdl_print(findPattern, 240, SCREEN_HEIGHT / 2 - 44, screen);
            SDL_Flip(screen);
            break;
        case _9:
            if (diff <= maxDiff && strlen(findPattern) > 0) {
                char c = findPattern[strlen(findPattern) - 1];
                switch (c) {
                    case '9':
                        findPattern[strlen(findPattern) - 1] = 'W';
                        break;
                    case 'Z':
                        findPattern[strlen(findPattern) - 1] = '9';
                        break;
                    default:
                        findPattern[strlen(findPattern) - 1]++;
                        break;
                }
            } else if (strlen(findPattern) < 20) {
                findPattern[strlen(findPattern)] = 'W';
                findPattern[strlen(findPattern) + 1] = '\0';
            }
            SDL_FillRect(screen, &(SDL_Rect) {240, SCREEN_HEIGHT / 2 - 44 + SPACING + 1, 390, SIZE + 2 * SPACING - 2},
                         SDL_MapRGB(screen->format, 255, 255, 255));
            sdl_print(findPattern, 240, SCREEN_HEIGHT / 2 - 44, screen);
            SDL_Flip(screen);
            break;
        case _0:
            if (diff <= maxDiff && strlen(findPattern) > 0) {
                char c = findPattern[strlen(findPattern) - 1];
                switch (c) {
                    case '0':
                        findPattern[strlen(findPattern) - 1] = ' ';
                        break;
                    case ' ':
                        findPattern[strlen(findPattern) - 1] = '0';
                        break;
                }
            } else if (strlen(findPattern) < 20) {
                findPattern[strlen(findPattern)] = '0';
                findPattern[strlen(findPattern) + 1] = '\0';
            }
            SDL_FillRect(screen, &(SDL_Rect) {240, SCREEN_HEIGHT / 2 - 44 + SPACING + 1, 390, SIZE + 2 * SPACING - 2},
                         SDL_MapRGB(screen->format, 255, 255, 255));
            sdl_print(findPattern, 240, SCREEN_HEIGHT / 2 - 44, screen);
            SDL_Flip(screen);
            break;
        default:
            break;
    }
    maxDiff = 1.25;
}

bool GUI_events(SDL_Surface *screen, Torrents *torrents, char *dir, CURL *myHandle) {
    findPattern = calloc(21, sizeof(char));
    redrawScreen(screen);
    selectedButton(VYHLADAT, screen);
    bool loop = true, exit = false;
    while (loop) {
#if !defined(__x86_64__)
        remote_key_event_sender();
#endif
        SDL_Event ev;
        if (SDL_PollEvent(&ev)) {
            if (ev.type == SDL_KEYDOWN) {
                if (exit && ev.key.keysym.sym == SDLK_F10 && ev.key.keysym.sym == SDLK_ESCAPE &&
                    ev.key.keysym.sym == SDLK_BACKSPACE)
                    exit = false;
                switch (ev.key.keysym.sym) {
                    case SDLK_RETURN:
                        if (activeScreen == Find || activeScreen == TYPING) {
                            buttonLogic(Selected_Button, torrents, screen, myHandle);
                        } else if (activeScreen == Selection && Selected_Torrent != NULL) {
                            downloadTorrent(myHandle, Selected_Torrent, dir);
                            SDL_BlitSurface(GUI_img[MESS], NULL, screen,
                                            &(SDL_Rect) {SCREEN_WIDTH / 2 - GUI_img[MESS]->w / 2,
                                                         SCREEN_HEIGHT / 2 - GUI_img[MESS]->h / 2, 0, 0});
                            sdl_print("Torrent stiahnutý.", SCREEN_WIDTH / 2 - 125, SCREEN_HEIGHT / 2 - 15, screen);
                            SDL_Flip(screen);
                            SDL_Delay(1000);
                            SDL_FillRect(screen, &(SDL_Rect) {0, 4, SCREEN_WIDTH / 3 + 2, (SIZE + 2 * SPACING) + 4},
                                         SDL_MapRGB(screen->format, 220, 220, 220));
                            SDL_FillRect(screen, &(SDL_Rect) {0, 6, SCREEN_WIDTH / 3, (SIZE + 2 * SPACING)},
                                         SDL_MapRGB(screen->format, 245, 248, 246));
                            sdl_print("Výsledky.", strlen("Výsledky") * SIZE / 2, 4, screen);
                            GUI_printTorrents(screen);
                        }
                        break;
                    case SDLK_UP:
                        if (activeScreen == Selection) {
                            if (Selected_Torrent != NULL && Selected_Torrent->prev != NULL) {
                                Selected_Torrent = Selected_Torrent->prev;
                                if (top == Selected_Torrent) {
                                    if (top->prev != NULL) {
                                        top = top->prev;
                                        bottom = bottom->prev;
                                    }
                                }
                                GUI_printTorrents(screen);
                            }
                        } else if (activeScreen == Find) {
                            selectedButton(VYHLADAT, screen);
                        } else if (activeScreen == TYPING && Selected_Button >= _4) {
                            Selected_Button -= 3;
                            redrawKeys(screen);
                            SDL_Flip(screen);
                        }
                        maxDiff = -1;
                        break;
                    case SDLK_DOWN:
                        if (activeScreen == Selection) {
                            if (Selected_Torrent != NULL && Selected_Torrent->next != NULL) {
                                Selected_Torrent = Selected_Torrent->next;
                                if (Selected_Torrent->next == NULL) {
                                    if (nextPageTorrent(findPattern, myHandle)) {
                                        getTorrents(torrents);
                                        GUI_printTorrents(screen);
                                    }
                                }
                                if (bottom == Selected_Torrent) {
                                    if (bottom->next != NULL) {
                                        bottom = bottom->next;
                                        top = top->next;
                                    }
                                }
                                GUI_printTorrents(screen);
                            }
                        } else if (activeScreen == Find) {
                            selectedButton(FILMY, screen);
                        } else if (activeScreen == TYPING && Selected_Button <= _9) {
                            Selected_Button += 3;
                            redrawKeys(screen);
                            SDL_Flip(screen);
                        }
                        maxDiff = -1;
                        break;
                    case SDLK_LEFT:
                        if (activeScreen == Find && Selected_Button != FILMY && Selected_Button != VYHLADAT) {
                            selectedButton(Selected_Button - 1, screen);
                        } else if (activeScreen == TYPING && Selected_Button > _1) {
                            Selected_Button--;
                            redrawKeys(screen);
                            SDL_Flip(screen);
                        }
                        maxDiff = -1;
                        break;
                    case SDLK_RIGHT:
                        if (activeScreen == Find && Selected_Button != SERIALY && Selected_Button != VYHLADAT) {
                            selectedButton(Selected_Button + 1, screen);
                        } else if (activeScreen == TYPING && Selected_Button <= _OK) {
                            Selected_Button++;
                            redrawKeys(screen);
                            SDL_Flip(screen);
                        }
                        maxDiff = -1;
                        break;
                    case SDLK_BACKSPACE:
                    case SDLK_ESCAPE:
                    case SDLK_F10:
                        if (exit)
                            loop = false;
                        else {
                            if (activeScreen == Find)
                                exit = true;
                            if (activeScreen == TYPING) {
                                activeScreen = Find;
                                redrawScreen(screen);
                                selectedButton(VYHLADAT, screen);
                            }
                            if (activeScreen == Selection) {
                                activeScreen = Find;
                                redrawScreen(screen);
                                selectedButton(VYHLADAT, screen);
                            }
                            break;
                        }
                        break;
                    default:
                        if (activeScreen == Find && Selected_Button == TEXT && strlen(findPattern) < 20) {
                            findPattern[strlen(findPattern) + 1] = '\0';
                            findPattern[strlen(findPattern)] = ev.key.keysym.unicode;
                            SDL_FillRect(screen, &(SDL_Rect) {240, SCREEN_HEIGHT / 2 - 44 + SPACING + 1, 390,
                                                              SIZE + 2 * SPACING - 2},
                                         SDL_MapRGB(screen->format, 255, 255, 255));
                            sdl_print(findPattern, 240, SCREEN_HEIGHT / 2 - 44, screen);
                            SDL_Flip(screen);
                        }
                        break;
                }
            }
        }
        SDL_Delay(1);
    }
    free(findPattern);
    return false;
}
