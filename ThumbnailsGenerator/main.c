#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <curl/curl.h>

unsigned long fsize(char *name) {
    FILE *fp = fopen(name, "r");
    fseek(fp, 0L, SEEK_END);
    unsigned long sz = ftell(fp);
    fclose(fp);
    return sz;
}

void listDir(char *path);

CURL *initCurl();

void downloadFile(CURL *myHandle, char *url, char *fullName);

int main() {
    //char *source="/media/data/downloads/";
    //listDir(source);

    int socket_desc, client_sock, c, dataSize;
    struct sockaddr_in server, client;

    //Create socket
    if ((socket_desc = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        fprintf(stderr, "Could not create socket\n");
        return -1;
    } else fprintf(stdout, "Socket created\n");
    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(9874);
    //Bind
    if (bind(socket_desc, (struct sockaddr *) &server, sizeof(server)) < 0) {
        //print the error message
        fprintf(stderr, "bind failed. Error\n");
        return 1;
    } else fprintf(stdout, "Bind done.\n");
    //Listen
    listen(socket_desc, 3);
    //Accept and incoming connection
    c = sizeof(struct sockaddr_in);
    unsigned int running = 1;
    while (running) {
        fprintf(stdout, "Waiting for incoming connections...\n");

        //accept connection from an incoming client
        client_sock = accept(socket_desc, (struct sockaddr *) &client, (socklen_t *) &c);
        if (client_sock < 0) {
            fprintf(stderr, "accept failed\n");
            break;
        }
        fprintf(stdout, "Connection accepted\n");
        //Receive a message from client
        while (recv(client_sock, &dataSize, sizeof(int), 0) > 0) {
            char *data = calloc(1 + dataSize, sizeof(char));
            if (recv(client_sock, data, dataSize, 0) > 0)
                fprintf(stdout, "%s\n", data);
            else fprintf(stdout, "ERROR recieving data.\n");
            free(data);
            sleep(1);
        }
        fprintf(stdout, "Client disconeceted.\n");
    }
    fprintf(stdout, "Stoping server !\n");
    close(client_sock);
    return 0;
}

size_t write_data(__attribute__((unused)) void *buffer, size_t size, size_t nmemb,
                  __attribute__((unused)) void *userp) {
    return size * nmemb;
}

CURL *initCurl() {
    curl_global_init(CURL_GLOBAL_ALL);
    CURL *myHandle = curl_easy_init();

    // Set up a couple inaitial paramaters that we will not need to mofiy later.
    curl_easy_setopt(myHandle, CURLOPT_USERAGENT, "Mozilla/5.0");
    curl_easy_setopt(myHandle, CURLOPT_AUTOREFERER, 1);
    curl_easy_setopt(myHandle, CURLOPT_WRITEFUNCTION, write_data);
    return myHandle;
}

void downloadFile(CURL *myHandle, char *url, char *fullName) {
    FILE *file = fopen(fullName, "w");

    curl_easy_setopt(myHandle, CURLOPT_WRITEFUNCTION, NULL);
    curl_easy_setopt(myHandle, CURLOPT_WRITEDATA, file);
    curl_easy_setopt(myHandle, CURLOPT_URL, url);
    curl_easy_perform(myHandle);

    fclose(file);
}

void listDir(char *path) {
    DIR *dir = NULL;
    dir = opendir(path);
    struct dirent *ent;
    while ((ent = readdir(dir)) != NULL) {
        if (strcmp(ent->d_name, ".") != 0 && strcmp(ent->d_name, "..") != 0) {
            char *fullName = malloc(sizeof(char) * (2 + strlen(path) + strlen(ent->d_name)));
            fullName[0] = '\0';
            strcat(fullName, path);
            if (path[strlen(path) - 1] != '/')
                strcat(fullName, "/");
            strcat(fullName, ent->d_name);
            DIR *subDir = opendir(fullName);
            if (subDir != 0) {
                listDir(fullName);
            } else {
                fprintf(stdout, "%s\n", fullName);
            }
            free(fullName);
        }
    }
    closedir(dir);
}
