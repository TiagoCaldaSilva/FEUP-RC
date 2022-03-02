#ifndef _SENDER_H
#define _SENDER_H

#include <unistd.h>
#include <signal.h>
#include "./utils.h"

/**
 * @brief Sender alarm handler
 * 
 */
void sigAlarmHandler();

/**
 * @brief Sends control frame specified in the controlField parameter and receives a response
 * 
 * @param fd - link layer identifier
 * @param controlField - control frame specifier to send
 * @param response - control frame specifier to receive
 * @return int - link layer identifier in case of success, -1 in case of error
 */
int sendControl(int fd, unsigned char controlField, unsigned char response);

/**
 * @brief Simulates the state of the current response frame beeing read
 * 
 * @param byteReceived - received byte from serial port
 * @param currentState - current state of frame 
 * @param controlField - control field specifier which will be read from received response
 */
void stateMachineSender(unsigned char byteReceived, enum state * currentState, unsigned char * controlField);

/**
 * @brief Sends information to serial port and waits for receiver response (confirmation or rejection)
 * 
 * @param fd - link layer identifier
 * @param frame - frame to write to the serial port
 * @return int - number of bytes sents, -1 in case of error
 */
int sendData(int fd, struct frame * frame);

#endif /*_SENDER_H*/
