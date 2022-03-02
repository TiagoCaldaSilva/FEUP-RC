#ifndef _UTILS_H
#define _UTILS_H

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>

#include <stdlib.h>
#include <unistd.h>

#define BAUDRATE B38400
#define MODEMDEVICE "/dev/ttyS1"
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FLAG       0x7E
#define ESC        0x7D
#define NEWVAL(x)  x ^ 0x20
#define SETUP      0x03
#define DISC       0x0B
#define UAKN       0x07
#define A_SND      0x03
#define BCC(x, y)  x ^ y
#define SEQUENCENUMBER(x) x << 6
#define A_RCVR     0x01
#define RR(x)      x<<7 | 0x05
#define REJ(x)     x<<7 | 0x01
#define TRANSMITTER 1
#define RECEIVER    0
#define DATA_PKT    0x01
#define START_PKT   0x02
#define END_PKT     0x03
#define FILE_SIZE   0
#define FILE_NAME   1

enum state {START, FLAG_RCV, A_RCV, C_RCV, BCC_OK, STOP};

struct linkLayer {
  unsigned int sequenceNumber;
  unsigned int timeOutMax;
  unsigned int alarmFlag;
};

struct frame
{
  unsigned char * buffer;
  int length;
};

/**
 * @brief Simulates the state of the current response frame beeing read
 * 
 * @param byteReceived - received byte from serial port
 * @param currentState - current state of frame 
 * @param addressField - address field specifier which is expected to be read from received response
 * @param controlField - control field specifier which is expected to be read from received response
 */
void stateMachineSU(char byteReceived, enum state * currentState, unsigned char addressField, unsigned char controlField);

/**
 * @brief Sends control frame with given parameters
 * 
 * @param fd - link layer identifier
 * @param adressField - address field specifier 
 * @param controlField - control field specifier
 * @return int - bytes send, -1 in case of error
 */
int sendMessage(int fd, unsigned char adressField, unsigned char controlField);

/**
 * @brief Receives control frame with given parameters
 * 
 * @param fd - link layer identifier
 * @param adressField - address field specifier
 * @param controlField - control field specifier
 * @return int - positive in case of success, -1 otherwise
 */
int receiveMessage(int fd, unsigned char adressField, unsigned char controlField);

#endif /*_UTILS_H*/