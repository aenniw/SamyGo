#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define MAX_BUF 1024
#define MAX_DRIVES 10
#define FIFO "/mtd_ram/mountD_"
#define USB_LOG "/dtv/usb/log"
#define USB_LOG_TMP "/dtv/usb/log_tmp"

int writeM(char *mess);

void *daemonM();

void *mounterM();

void mountDrive(char *mnt, char *name);

void uMountDrive(char *mnt);

bool isMounted(char *name);

struct DriveNode_s {
    char *mnt;
    char *name;
    struct DriveNode_s *next;
};

typedef struct DriveNode_s DriveNode;

typedef struct {
    DriveNode *drives;
    bool drivesLock;
    unsigned int size, mSize;
} Drives;

DriveNode *addNode(Drives *d, char *mnt, char *name) {
    if (d->size < d->mSize) {
        DriveNode *tmp = (DriveNode *) malloc(sizeof(DriveNode));
        tmp->mnt = (char *) malloc((strlen(mnt) + 1) * sizeof(char));
        strcpy(tmp->mnt, mnt);
        tmp->name = (char *) malloc((strlen(name) + 1) * sizeof(char));
        strcpy(tmp->name, name);
        if (d->drives == NULL)
            tmp->next = NULL;
        else
            tmp->next = d->drives;
        d->drives = tmp;
        d->size++;
        return tmp;
    }
    return NULL;
}

void removeNode(Drives *d, char *mnt) {
    DriveNode *tmp = d->drives;
    DriveNode *prv = NULL;
    while (tmp != NULL) {
        if (strcmp(tmp->mnt, mnt) == 0) {
            if (isMounted(tmp->name))
                uMountDrive(tmp->mnt);
            if (prv != NULL)
                prv->next = tmp->next;
            else
                d->drives = tmp->next;
            free(tmp->mnt);
            free(tmp->name);
            free(tmp);
            break;
        }
        prv = tmp;
        tmp = tmp->next;
    }
}

Drives drives;
bool daemonRun = true;

int main(int argc, char *argv[]) {
    if (argc == 2 && strcmp(argv[1], "-d") == 0) {
        drives.size = 0;
        drives.drivesLock = false;
        drives.mSize = MAX_DRIVES;

        pthread_t pipeReaderT, mounterT;
        pthread_create(&pipeReaderT, NULL, daemonM, NULL);
        pthread_create(&mounterT, NULL, mounterM, NULL);
        pthread_join(pipeReaderT, NULL);
        pthread_join(mounterT, NULL);
        fprintf(stdout, "Daemon Stoped.\n");

        DriveNode *tmp = drives.drives;
        while (tmp != NULL) {
            if (isMounted(tmp->name))
                uMountDrive(tmp->mnt);
            DriveNode *hlp = tmp;
            tmp = tmp->next;
            free(hlp);
        }
    } else if (argc >= 3 && argc <= 4) {
        if (argc == 4 && strcmp(argv[1], "-mount") == 0) {
            char *buff = (char *) malloc(sizeof(char) * (strlen(argv[1]) + strlen(argv[2]) + strlen(argv[3]) + 3));
            strcat(buff, argv[1]);
            strcat(buff, " ");
            strcat(buff, argv[2]);
            strcat(buff, " ");
            strcat(buff, argv[3]);
            writeM(buff);
            free(buff);
        } else if (strcmp(argv[1], "-umount") == 0) {
            char *buff = (char *) malloc(sizeof(char) * (strlen(argv[1]) + strlen(argv[2]) + 2));
            strcat(buff, argv[1]);
            strcat(buff, " ");
            strcat(buff, argv[2]);
            writeM(buff);
            free(buff);
        }

    } else if (argc == 2 && strcmp(argv[1], "kill") == 0) {
        writeM(argv[1]);
    } else {
        fprintf(stdout, "USAGE::\n   -d  for Deamon\n   -mount where name\n   -umount where\n");
    }
    return 0;
}

int writeM(char *mess) {
    int fd;
    char *myfifo = FIFO;
    /* create the FIFO (named pipe) */
    fd = open(myfifo, O_WRONLY);
    if (write(fd, mess, strlen(mess)) < 0) {
        close(fd);
        return -1;
    }
    close(fd);
    return 0;
}


void *mounterM() {
    while (daemonRun) {
        while (drives.drivesLock)
            sleep(1);
        drives.drivesLock = true;
        DriveNode *tmp = drives.drives;
        while (tmp != NULL) {
            if (!isMounted(tmp->name))
                mountDrive(tmp->mnt, tmp->name);
            tmp = tmp->next;
        }
        drives.drivesLock = false;
        sleep(2);
    }
    return 0;
}

void mountDrive(char *mnt, char *name) {
    char buf[MAX_BUF];
    memset(buf, '\0', MAX_BUF);
    strcat(buf, "[");
    strcat(buf, mnt);
    strcat(buf, "]");
    strcat(buf, "\nVendor : VirtualDrive\nProduct : ");
    strcat(buf, name);
    strcat(buf, "\nSerial : Q80VQFLF\nDevpath : 4\nLun : 0\nMountDir : /dtv/usb/");
    strcat(buf, mnt);
    strcat(buf, "\nFileSystem : vfat\n\n");
    FILE *log = fopen(USB_LOG, "a");
    fprintf(log, "%s", buf);
    fprintf(stdout, "%s", buf);
    fclose(log);
}

void uMountDrive(char *mnt) {
    FILE *log = fopen(USB_LOG, "r");
    if (log == NULL)
        return;
    FILE *tmp = fopen(USB_LOG_TMP, "w");
    unsigned int lineL = 100, deleted = 0;
    char line[lineL];
    while (fgets(line, lineL, log) != NULL) {
        if (strstr(line, mnt) != NULL)
            deleted++;
        else if (deleted == 0 || deleted == 9) {
            fprintf(tmp, "%s", line);
        } else
            deleted++;
    }
    fclose(log);
    fclose(tmp);
    remove(USB_LOG);
    rename(USB_LOG_TMP, USB_LOG);
}

bool isMounted(char *name) {
    FILE *log = fopen(USB_LOG, "r");
    if (log == NULL)
        return false;
    unsigned int lineL = 100;
    char line[lineL];
    while (fgets(line, lineL, log) != NULL) {
        if (strstr(line, name) != NULL)
            return true;
    }
    fclose(log);
    return false;
}

void *daemonM() {
    int fd;
    char *myfifo = FIFO;
    char buf[MAX_BUF], cmd[10], mnt[MAX_BUF], name[MAX_BUF];
    int readM = 0;
    mkfifo(myfifo, 0666);
    /* open, read, and display the message from the FIFO */
    fd = open(myfifo, O_RDWR);
    while (daemonRun) {
        if ((readM = read(fd, buf, MAX_BUF)) > 0) {
            buf[readM] = '\0';
            fprintf(stdout, "Received: %s\n", buf);
            if (strcmp(buf, "kill") == 0) {
                daemonRun = false;
                return 0;
            }
            if (drives.size < drives.mSize) {
                while (drives.drivesLock)
                    sleep(1);
                drives.drivesLock = true;
                unsigned int count = 0, split = 0;
                while (buf[count] != ' ') {
                    cmd[count] = buf[count];
                    count++;
                }
                cmd[count] = '\0';
                if (strcmp(cmd, "-umount") == 0) {
                    count++;
                    while (buf[count] != '\0' && buf[count] != '\n' && buf[count] != ' ') {
                        mnt[split] = buf[count];
                        count++;
                        split++;
                    }
                    mnt[split] = '\0';
                    removeNode(&drives, mnt);
                    drives.drivesLock = false;
                } else if (strcmp(cmd, "-mount") == 0) {
                    count++;
                    while (buf[count] != ' ') {
                        mnt[split] = buf[count];
                        count++;
                        split++;
                    }
                    mnt[split] = '\0';
                    split = 0;
                    count++;
                    while (buf[count] != '\0' && buf[count] != '\n' && buf[count] != ' ') {
                        name[split] = buf[count];
                        count++;
                        split++;
                    }
                    name[split] = '\0';
                    addNode(&drives, mnt, name);
                    drives.drivesLock = false;
                }
            }
        }
        sleep(2);
    }
    close(fd);
    return 0;
}
