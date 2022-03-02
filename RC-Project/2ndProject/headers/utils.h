#ifndef _UTILS_H_
#define _UTILS_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int getSubString(char ** to, char * from, char * delim);

void printError(char * str);

int validCredentials(char * user, char * password);

struct hostent* getIp(char * hostName);

#endif //_UTILS_H_