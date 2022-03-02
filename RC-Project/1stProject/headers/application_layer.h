#ifndef _APPLICATION_LAYER_H
#define _APPLICATION_LAYER_H

#include <string.h>
#include <stdio.h>
#include <math.h>
#include <sys/stat.h>
#include "./link_layer.h"

#define PACKET_LENGTH 100

struct applicationLayer {
  int fileDescriptor;
  int status;
};

/**
 * @brief Provides the application layer for the reader
 * 
 * @param fd - serial port file descriptor
 * @return int - 0 on success, -1 otherwise
 */
int appRead(int fd);

/**
 * @brief Provides the application layer for the transmitter
 * 
 * @param fd - serial port file descriptor
 * @param name - file path, representing the file to transmit
 * @return int - 0 on success, -1 otherwise
 */
int appWrite(int fd, char * name);

int main(int argc, char** argv);

#endif /*_APPLICATION_LAYER_H*/
