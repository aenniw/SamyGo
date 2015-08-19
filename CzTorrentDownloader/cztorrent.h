//
// Created by aen on 19.8.2015.
//

#ifndef CZTORRENTDOWNLOADER_CZTORRENT_H
#define CZTORRENTDOWNLOADER_CZTORRENT_H
#define URL "http://tracker.cztorrent.net/login"
#define DURL "http://tracker.cztorrent.net"
#define TMPHTML "./curlTmp.html"
#define COOKIE "./cookie.txt"

#include <curl/curl.h>
#include <stdbool.h>
#include "commons.h"

enum Category {
    Filmy_Dokumenty = 0, Filmy, Mluvne_slovo, Hudba, Filmy_Anime, Filmy_Kreslene, Serialy
};



CURL *initCurl();

char *translateOptions();

void addTorrent(Torrents *list, Torrent torrent);

void freeTorrents(Torrents *list);

void printTorrents(Torrents *list);

char *readLine();

void logIn(char *user, char *pass, CURL *myHandle);

void downloadTorrent(CURL *myHandle, Torrent *torrent, char *downloadDir);

void findTorrent(char *name, CURL *myHandle);

bool nextPageTorrent(char *name, CURL *myHandle);

void setOptions(enum Category c);

void getTorrents(Torrents *t);

#endif //CZTORRENTDOWNLOADER_CZTORRENT_H
