//
// Created by aen on 19.8.2015.
//

#ifndef CZTORRENTDOWNLOADER_COMMONS_H
#define CZTORRENTDOWNLOADER_COMMONS_H
#define SCREEN_WIDTH    (1920/2)
#define SCREEN_HEIGHT    (1080/2)
#define SCREEN_BPP    32
#define SPACING 3
#define SIZE 28

struct Torrent_s {
    char *url;
    char *name;
    char *size;
    char *seeds;
    char *peers;
    char *active;
    struct Torrent_s *next;
    struct Torrent_s *prev;
};

typedef struct Torrent_s Torrent;
typedef struct {
    int size;
    Torrent *begin;
    Torrent *end;
} Torrents;
#endif //CZTORRENTDOWNLOADER_COMMONS_H
