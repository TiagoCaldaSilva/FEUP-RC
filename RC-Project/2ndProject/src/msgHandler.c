#include "../headers/msgHandler.h"

int receiveResponse(int fd) {
    char buf[256];
    int res;
    int i = 0;
    char code[4];
    int end = 1;

    while (end) {
        for (int j = 0; j < 4; j++) {
            res = read(fd, &code[j], 1);
        }
        end = code[3] == '-';
        i = 0;
        do {
            res = read(fd, &buf[i++], 1);
            if (res < 0) return 0;
        } while (buf[i - 1] != '\n');
    }

    return atoi(code);
}

int writeMsg(int sockfd, char* message, size_t length, int code) {
    /* send message */
    size_t bytes = write(sockfd, message, length);
    if (bytes < 1) {
        return -1;
    }

    /*receive response*/
    return receiveResponse(sockfd) == code;
}

int receiveMsg(int fd, char* msg) {
    char buf[256];
    int res;
    int i = 0;
    char code[4];
    int end = 1;

    while (end) {
        for (int j = 0; j < 4; j++) {
            res = read(fd, &code[j], 1);
        }
        end = code[3] == '-';
        i = 0;
        memset(buf, 0, 255);
        do {
            res = read(fd, &buf[i++], 1);
            if (res < 0) return 0;
        } while (buf[i - 1] != '\n');
    }

    strncpy(msg, buf, 255);
    return atoi(code);
}

int writeMsgToGetRes(int sockfd, char* message, size_t length, int code, char* response) {
    /* send message */
    size_t bytes = write(sockfd, message, length);
    if (bytes < 1) {
        return -1;
    }

    /*receive response*/
    return receiveMsg(sockfd, response) == code;
}
