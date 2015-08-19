#include <dirent.h>
#include <unistd.h>
#include <stdbool.h>
#include <SDL/SDL.h>
#include "cztorrent.h"
#include "gui.h"

//MAIN
Uint8 *kbhits;

#if !defined(__x86_64__)
int ex_key_code = 0;
int ex_key_mod  = 0;
void send_key_event(int, int);
void send_key_event(int key_code, int key_mod)
{
    SDL_Event uevent;
    int hitval;
    if (key_code == 0) return;
    if (key_code > 0)
    {
        uevent.key.type = SDL_KEYDOWN;
        uevent.key.state = SDL_PRESSED;
        hitval = 1;
    }
    else
    {
        uevent.key.type = SDL_KEYUP;
        uevent.key.state = SDL_RELEASED;
        key_code = -key_code;
        hitval = 0;
    }
    uevent.key.keysym.sym = key_code;
    uevent.key.keysym.mod = key_mod;
    uevent.key.keysym.unicode = 0;
    uevent.key.keysym.scancode = 0;
    SDL_PushEvent(&uevent);
    if (kbhits)
    {
        kbhits[key_code] = hitval;
        if (key_mod) kbhits[key_mod] = hitval;
    }
}

#define AUTOREPEAT_DELAY 10
#define AUTOREPEAT_LOOPS 20
int remote_key_keyboard(int code);
int repeat_counter = 0;
int last_key_code = 0;
void remote_key_event_sender(void)
{
    if (ex_key_code < 0)
    {
        send_key_event(ex_key_code, ex_key_mod);
        ex_key_code = 0;
        ex_key_mod  = 0;
        repeat_counter = 0;
    }
    else if (ex_key_code > 0)
    {
        if (last_key_code == ex_key_code) {
            SDL_Delay(AUTOREPEAT_DELAY);
            repeat_counter++;
        } else repeat_counter=0;
        if ((repeat_counter == 0) || (repeat_counter > AUTOREPEAT_LOOPS)) {
            send_key_event(ex_key_code, ex_key_mod);
        }
        last_key_code = ex_key_code;
    }
}
#endif


int InitSDL(SDL_Surface **screen) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
        return 1;
    SDL_EnableUNICODE(true);
    SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);
    if (!(*screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, 0)))
        return 1;
    SDL_FillRect(*screen, &(SDL_Rect) {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT}, SDL_MapRGB((*screen)->format, 45, 56, 74));
    SDL_Flip(*screen);
    return 0;
}

// add ip port
bool loadConfig(char **name, char **pass, char **downD) {
    FILE *file = fopen("./conf.txt", "r");
    if (file == NULL)
        return true;
    char line[50];
    char *point = NULL;
    while (fgets(line, 50, file) != NULL) {
        if ((point = strstr(line, "user='"))) {
            point += 6;
            *name = malloc(sizeof(char) * (strlen(point) + 1));
            strcpy(*name, point);
            (*name)[strlen(*name) - 2] = '\0';
        } else if ((point = strstr(line, "user="))) {
            point += 5;
            *name = malloc(sizeof(char) * (strlen(point) + 1));
            strcpy(*name, point);
            (*name)[strlen(*name) - 1] = '\0';
        } else if ((point = strstr(line, "pass='"))) {
            point += 6;
            *pass = malloc(sizeof(char) * (strlen(point) + 1));
            strcpy(*pass, point);
            (*pass)[strlen(*pass) - 2] = '\0';
        } else if ((point = strstr(line, "pass="))) {
            point += 5;
            *pass = malloc(sizeof(char) * (strlen(point) + 1));
            strcpy(*pass, point);
            (*pass)[strlen(*pass) - 1] = '\0';
        } else if ((point = strstr(line, "dir='"))) {
            point += 5;
            *downD = malloc(sizeof(char) * (strlen(point) + 1));
            strcpy(*downD, point);
            (*downD)[strlen(*downD) - 2] = '\0';
        } else if ((point = strstr(line, "dir="))) {
            point += 4;
            *downD = malloc(sizeof(char) * (strlen(point) + 1));
            strcpy(*downD, point);
            (*downD)[strlen(*downD) - 1] = '\0';
        }
    }
    fclose(file);
    return *name == NULL && *pass == NULL && *downD == NULL;
}

int _Game_Main(const char *path, const char *udn __attribute__ ((unused))) {
    char old_path[256];
    if (getcwd(old_path, sizeof(old_path)) == NULL) strcpy(old_path, "/mtd_exe/");
    chdir(path);
    setenv("HOME", path, 1);
    char libDir[strlen(path) + 7];
    libDir[0] = '\0';
    strcat(libDir, path);
    strcat(libDir, "/LIB/:");
    if (strstr(getenv("LD_LIBRARY_PATH"), libDir) == NULL) {
        char libs[strlen(libDir) + strlen(getenv("LD_LIBRARY_PATH")) + 1];
        libs[0] = '\0';
        strcat(libs, libDir);
        strcat(libs, getenv("LD_LIBRARY_PATH"));
        setenv("LD_LIBRARY_PATH", libs, 1);
    }
    SDL_Surface *screen = NULL;
    Torrents torrents = (Torrents) {0, NULL, NULL};
    if (InitSDL(&screen)) {
        fprintf(stdout, "%s", SDL_GetError());
        return 1;
    }
    GUI_init();
    char *name = NULL, *pass = NULL, *dir = NULL;
    if (loadConfig(&name, &pass, &dir)) {
        fprintf(stdout, "Missing configurations file.\n");
        GUI_drawMessageBox("Missing configurations file.", screen);
        SDL_Delay(1500);
        GUI_clean();
        SDL_FreeSurface(screen);
        SDL_Quit();
        chdir(old_path);
        setenv("HOME", old_path, 1);
        free(name);
        free(pass);
        free(dir);
        return 1;
    }
    CURL *myHandle = initCurl();
    if (myHandle == NULL) {
        fprintf(stdout, "Server unavailable\n");
        GUI_drawMessageBox("Server unavailable", screen);
        SDL_Delay(1500);
        GUI_clean();
        SDL_FreeSurface(screen);
        SDL_Quit();
        chdir(old_path);
        setenv("HOME", old_path, 1);
        free(name);
        free(pass);
        free(dir);
        return 1;
    }
    logIn(name, pass, myHandle);
    free(name);
    free(pass);
    GUI_events(screen, &torrents, dir, myHandle);
    freeTorrents(&torrents);
    curl_easy_cleanup(myHandle);
    GUI_clean();
    SDL_FreeSurface(screen);
    SDL_Quit();
    free(dir);
    chdir(old_path);
    setenv("HOME", old_path, 1);
    return 0;
}

#if defined(__x86_64__)

int main(int argc __attribute__ ((unused)), char *argv[] __attribute__ ((unused))) {
    char cur_path[256];
    if (getcwd(cur_path, sizeof(cur_path)) == NULL) strcpy(cur_path, "/");
    _Game_Main(cur_path, "");
    return 0;
}

#endif
