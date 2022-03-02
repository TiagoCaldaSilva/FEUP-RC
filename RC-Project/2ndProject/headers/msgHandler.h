#ifndef _MSGHANDLER_H_
#define _MSGHANDLER_H_

#include <unistd.h>
#include <string.h>
#include <stdlib.h>

int receiveResponse(int fd);
int writeMsg(int sockfd, char* message, size_t length, int code);
int receiveMsg(int fd, char* msg);
int writeMsgToGetRes(int sockfd, char* message, size_t length, int code, char* response);

#endif //_MSGHANDLER_H_