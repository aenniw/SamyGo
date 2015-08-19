//
// Created by aen on 19.8.2015.
//

#include <string.h>
#include <stdlib.h>
#include "cztorrent.h"

bool catOptions[Serialy + 1];
bool hasNextPage = false;
int page = 1;


char *translateOptions() {
    int i = 0;
    char *out = malloc(sizeof(char) * 43);
    out[0] = '\0';
    for (i = Filmy_Dokumenty; i <= Serialy; i++) {
        if (catOptions[i]) {
            switch (i) {
                case Filmy_Dokumenty:
                    strcat(out, "c33=1&");
                    break;
                case Filmy:
                    strcat(out, "c1=1&");
                    break;
                case Mluvne_slovo:
                    strcat(out, "c32=1&");
                    break;
                case Hudba:
                    strcat(out, "c2=1&");
                    break;
                case Filmy_Anime:
                    strcat(out, "c35=1&");
                    break;
                case Filmy_Kreslene:
                    strcat(out, "c5=1&");
                    break;
                case Serialy:
                    strcat(out, "c25=1&");
                    break;
            }
        }
    }
    if (strlen(out) == 0) {
        free(out);
        return NULL;
    }
    return out;
}

void addTorrent(Torrents *list, Torrent torrent) {
    Torrent *t = malloc(sizeof(Torrent));
    t->url = torrent.url;
    t->name = torrent.name;
    t->size = torrent.size;
    t->seeds = torrent.seeds;
    t->peers = torrent.peers;
    t->active = torrent.active;
    if (list->size == 0) {
        list->begin = t;
        list->end = t;
        t->next = NULL;
        t->prev = NULL;
    } else {
        t->next = NULL;
        t->prev = list->end;
        list->end->next = t;
        list->end = t;
    }
    list->size++;
}

void freeTorrents(Torrents *list) {
    Torrent *tmp = NULL, *t = list->begin;
    while (t != NULL) {
        free(t->active);
        free(t->name);
        free(t->peers);
        free(t->seeds);
        free(t->size);
        free(t->url);
        tmp = t;
        t = t->next;
        free(tmp);
    }
    list->begin = NULL;
    list->end = NULL;
    list->size = 0;
}

void printTorrents(Torrents *list) {
    Torrent *t = list->begin;
    while (t != NULL) {
        fprintf(stdout, "%s\n", t->name);
        fprintf(stdout, "%s\n", t->url);
        fprintf(stdout, "%s\t", t->size);
        fprintf(stdout, "%s|", t->seeds);
        fprintf(stdout, "%s|", t->peers);
        fprintf(stdout, "%s\n\n", t->active);
        t = t->next;
    }
    fflush(stdout);
}

size_t write_data(__attribute__((unused)) void *buffer, size_t size, size_t nmemb,
                  __attribute__((unused)) void *userp) {
    return size * nmemb;
}

CURL *initCurl() {
    int i = 0;
    for (i = Filmy_Dokumenty; i <= Serialy; i++)
        catOptions[i] = false;
    curl_global_init(CURL_GLOBAL_ALL);
    CURL *myHandle = curl_easy_init();

    // Set up a couple inaitial paramaters that we will not need to mofiy later.
    curl_easy_setopt(myHandle, CURLOPT_USERAGENT, "Mozilla/5.0");
    curl_easy_setopt(myHandle, CURLOPT_AUTOREFERER, 1);
    curl_easy_setopt(myHandle, CURLOPT_COOKIEJAR, COOKIE);
    curl_easy_setopt(myHandle, CURLOPT_COOKIEFILE, COOKIE);
    curl_easy_setopt(myHandle, CURLOPT_WRITEFUNCTION, write_data);
    return myHandle;
}

char *readLine() {
    FILE *file = fopen(TMPHTML, "r");
    if (file == NULL) {
        printf("Error: file pointer is null.");
        return NULL;
    }
    int maximumLineLength = 128;
    char *lineBuffer = (char *) malloc(sizeof(char) * maximumLineLength);
    if (lineBuffer == NULL) {
        printf("Error allocating memory for line buffer.");
        fclose(file);
        return NULL;
    }
    do {
        char ch = fgetc(file);
        int count = 0;

        while (ch != '\n') {
            if (ch == EOF) {
                free(lineBuffer);
                fclose(file);
                return NULL;
            }
            if (count == maximumLineLength) {
                maximumLineLength += 128;
                lineBuffer = realloc(lineBuffer, maximumLineLength);
                if (lineBuffer == NULL) {
                    printf("Error reallocating space for line buffer.");
                    fclose(file);
                    return NULL;
                }
            }
            lineBuffer[count] = ch;
            count++;

            ch = fgetc(file);
        }
        lineBuffer[count] = '\0';
    } while (!strstr(lineBuffer, "<table id='torrenty' cellpadding='0' cellspacing='0'>"));
    fclose(file);
    return lineBuffer;
}

void logIn(char *user, char *pass, CURL *myHandle) {
    // Visit the login page once to obtain a PHPSESSID cookie
    curl_easy_setopt(myHandle, CURLOPT_URL, URL);
    curl_easy_perform(myHandle);

    // Now, can actually login. First we forge the HTTP referer field, or HTS will deny the login
    curl_easy_setopt(myHandle, CURLOPT_FOLLOWLOCATION, 1);
    curl_easy_setopt(myHandle, CURLOPT_REFERER, URL);
    // Next we tell LibCurl what HTTP POST data to submit
    char data[(1 + strlen(user) + strlen(pass) +
               strlen("username=&password=&persistent_login=1&submit=P%C5%99ihl%C3%A1sit"))];
    data[0] = '\0';
    strcat(data, "username=");
    strcat(data, user);
    strcat(data, "&password=");
    strcat(data, pass);
    strcat(data, "&persistent_login=1&submit=P%C5%99ihl%C3%A1sit");
    curl_easy_setopt(myHandle, CURLOPT_POSTFIELDS, data);
    curl_easy_perform(myHandle);
}

void downloadTorrent(CURL *myHandle, Torrent *torrent, char *downloadDir) {
    int length = strlen(downloadDir) + strlen(torrent->name) + strlen(".torrent") + 1;
    char *fPath = malloc(sizeof(char) * length);
    strcpy(fPath, downloadDir);
    int count = 0, start = strlen(downloadDir);
    while (torrent->name[count] != '\0') {
        if (torrent->name[count] == '/' || torrent->name[count] == '\\')
            fPath[start + count] = '|';
        else
            fPath[start + count] = torrent->name[count];
        count++;
    }
    fPath[start + count] = '\0';
    strcat(fPath, ".torrent");
    FILE *file = fopen(fPath, "w");
    fprintf(stdout, "\n%s\n", fPath);

    curl_easy_setopt(myHandle, CURLOPT_WRITEFUNCTION, NULL);
    curl_easy_setopt(myHandle, CURLOPT_WRITEDATA, file);
    curl_easy_setopt(myHandle, CURLOPT_URL, torrent->url);
    curl_easy_perform(myHandle);

    fclose(file);
    free(fPath);
}

void findTorrent(char *name, CURL *myHandle) {
    hasNextPage = false;
    page = 1;
    unsigned i, len = strlen(name);
    char *pName = malloc(sizeof(char) * (len + 1));
    for (i = 0; i < len; i++)
        if (name[i] == ' ')
            pName[i] = '+';
        else
            pName[i] = name[i];
    pName[len] = '\0';
    char *lookUrl = malloc(sizeof(char) * (45 + 43 + len));
    lookUrl[0] = '\0';
    strcat(lookUrl, "http://tracker.cztorrent.net/torrents?");
    char *c = NULL;
    if ((c = translateOptions()) != NULL) {
        strcat(lookUrl, c);
        free(c);
    }
    strcat(lookUrl, "s=");
    strcat(lookUrl, pName);
    strcat(lookUrl, "&t=0");
    FILE *file = fopen(TMPHTML, "w");
    curl_easy_setopt(myHandle, CURLOPT_WRITEFUNCTION, NULL);
    curl_easy_setopt(myHandle, CURLOPT_WRITEDATA, file);
    curl_easy_setopt(myHandle, CURLOPT_URL, lookUrl);
    curl_easy_perform(myHandle);
    fclose(file);
    free(lookUrl);
    free(pName);
}

bool nextPageTorrent(char *name, CURL *myHandle) {
    if (!hasNextPage) return false;
    page++;
    unsigned i, len = strlen(name);
    char *pName = malloc(sizeof(char) * (len + 1));
    for (i = 0; i < len; i++)
        if (name[i] == ' ')
            pName[i] = '+';
        else
            pName[i] = name[i];
    pName[len] = '\0';
    char *lookUrl = malloc(sizeof(char) * (45 + 53 + len));
    lookUrl[0] = '\0';
    strcat(lookUrl, "http://tracker.cztorrent.net/torrents?");
    char *c = NULL;
    if ((c = translateOptions()) != NULL) {
        strcat(lookUrl, c);
        free(c);
    }
    strcat(lookUrl, "s=");
    strcat(lookUrl, pName);
    strcat(lookUrl, "&t=0&p=");
    char str[15];
    sprintf(str, "%d", page);
    strcat(lookUrl, str);
    FILE *file = fopen(TMPHTML, "w");
    curl_easy_setopt(myHandle, CURLOPT_WRITEFUNCTION, NULL);
    curl_easy_setopt(myHandle, CURLOPT_WRITEDATA, file);
    curl_easy_setopt(myHandle, CURLOPT_URL, lookUrl);
    curl_easy_perform(myHandle);
    fclose(file);
    free(lookUrl);
    free(pName);
    return true;
}

void setOptions(enum Category c) {
    catOptions[c] = !catOptions[c];
}

void getTorrents(Torrents *t) {
    char *line = NULL;
    if ((line = readLine()) != NULL) {
        int offset = 0;
        char *p = line;
        char pageStr[255];
        pageStr[0] = '\0';
        sprintf(pageStr, "<a title='%d. stránka'", page + 1);
        char *np = strstr(p, "<p class='pager'>");
        if (np != NULL) if (strstr(np, pageStr))
            hasNextPage = true;
        else
            hasNextPage = false;
        else
            hasNextPage = false;
        while ((p = strstr(p, "<td class='download'>   <a href="))) {
            char *url = NULL, *name = NULL, *size = NULL, *seeds = NULL, *peers = NULL, *active = NULL;
            bool canAdd = false;
            //down
            p += 33;
            offset = p;
            while (*p != '\'')
                p++;
            offset = p - offset;
            url = malloc(sizeof(char) * (1 + offset + strlen(DURL)));
            strcpy(url, DURL);
            strncpy(url + strlen(DURL), (p - offset), offset);
            url[offset + strlen(DURL)] = '\0';
            //canAdd
            p = strstr(p, "class='") + 7;
            int i = 0;
            while (*p != '\'') {
                p++;
                i++;
            }
            canAdd = (strlen("download") == i);
            //name
            if ((p = strstr(p, "<td class='detaily'>"))) {
                p += 20;
                while (*p != '>')
                    p++;
                p++;
                offset = p;
                while (*p != '<')
                    p++;
                offset = p - offset;
                name = malloc(sizeof(char) * (offset + 1));
                strncpy(name, (p - offset), offset);
                name[offset] = '\0';
            }
            //size
            if ((p = strstr(p, "<span>Velikost:"))) {
                p += 16;
                offset = p;
                while (*p != '|')
                    p++;
                offset = p - offset;
                size = malloc(sizeof(char) * (offset + 1));
                strncpy(size, (p - offset), offset);
                size[offset] = '\0';
            }
            //seeds
            if ((p = strstr(p, "<span class='peers_"))) {
                p += 19;
                while (*p != '>')
                    p++;
                p++;
                offset = p;
                while (*p != '<')
                    p++;
                offset = p - offset;
                seeds = malloc(sizeof(char) * (offset + 1));
                strncpy(seeds, (p - offset), offset);
                seeds[offset] = '\0';
            }
            //peers
            if ((p = strstr(p, "<span class='peers_"))) {
                p += 19;
                while (*p != '>')
                    p++;
                p++;
                offset = p;
                while (*p != '<')
                    p++;
                offset = p - offset;
                peers = malloc(sizeof(char) * (offset + 1));
                strncpy(peers, (p - offset), offset);
                peers[offset] = '\0';
            }
            //active
            if ((p = strstr(p, "<td class='active'>"))) {
                p += 19;
                while (*p != '>')
                    p++;
                p++;
                offset = p;
                while (*p != '<')
                    p++;
                offset = p - offset;
                active = malloc(sizeof(char) * (offset + 1));
                strncpy(active, (p - offset), offset);
                active[offset] = '\0';
            }
            if (canAdd)
                addTorrent(t, (Torrent) {url, name, size, seeds, peers, active, NULL, NULL});
            else {
                free(url);
                free(name);
                free(size);
                free(seeds);
                free(peers);
                free(active);
            }
        }
    }
    free(line);
}
