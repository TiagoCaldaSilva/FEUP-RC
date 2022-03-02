#ifndef _DOWNLOAD_H_
#define _DOWNLOAD_H_

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "../headers/msgHandler.h"

#define SERVER_PORT 21

/**
 * @brief Connect to host and retrieve socket fd
 * 
 * @param server_address - server ip address
 * @param port - connection port
 * @return int - socket file descriptor
 */
int connectToHost(char* server_address, int port);

/**
 * @brief Get the File Name object
 * 
 * @param path 
 * @param fileName 
 */
void getFileName(char* path, char** fileName);

/**
 * @brief 
 * 
 * @param fd 
 * @param fileName 
 * @return int 
 */
int readFile(int fd, char* fileName);

/**
 * @brief 
 * 
 * @param server_address 
 * @param user 
 * @param password 
 * @param path 
 * @return int 
 */
int download(char* server_address, char* user, char* password, char* path);

#endif //_DOWNLOAD_H_
