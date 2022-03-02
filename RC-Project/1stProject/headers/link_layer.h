#ifndef _LINK_LAYER_H
#define _LINK_LAYER_H

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include "./sender.h"
#include "./receiver.h"

/**
 * @brief Set up the serial port settings and link the transmitter and receiver through control frames
 * 
 * @param path - serial port specifier
 * @param status - transmitter or receiver flag 
 * @return int - link layer identifier or -1 in case of error
 */
int llopen(char* path, int status);

/**
 * @brief Reset the serial port settings and close the PCs connection
 * 
 * @param fd - link layer identifier
 * @param status - transmitter or receiver flag 
 * @return int - positive number in case of success, -1 otherwise
 */
int llclose(int fd, int status);

/**
 * @brief Send a frame protected by byte stuffing with information
 * 
 * @param fd - link layer identifier
 * @param buffer - character buffer to transmit
 * @param length - character buffer size
 * @return int - number of bytes sent, -1 in case of error
 */
int llwrite(int fd, unsigned char * buffer, int length);

/**
 * @brief Receive a frame protected by byte stuffing and process its content
 * 
 * @param fd - link layer identifier
 * @param buffer - read character buffer
 * @return int - number of bytes received, -1 in case of error
 */
int llread(int fd, unsigned char * buffer);

/**
 * @brief Applies the byte stuffing mechanism to the given frame buffer
 * 
 * @param toStuff - frame to stuff
 * @return struct frame* - frame already stuffed
 */
struct frame * stuffing(struct frame * toStuff);

/**
 * @brief Removes the byte stuffing mechanism of the given frame buffer
 * 
 * @param toDestuff - frame to destuff
 * @return struct frame* - frame already destuffed
 */
struct frame * destuffing(struct frame * toDestuff);

#endif /*_LINK_LAYER_H*/
