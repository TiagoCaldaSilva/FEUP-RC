#ifndef _RECEIVER_H
#define _RECEIVER_H

#include <unistd.h>
#include <signal.h>
#include "./utils.h"

/**
 * @brief Receives control frame specified in the controlField parameter and sends a response
 * 
 * @param fd - link layer identifier
 * @param controlField - control frame specifier to receive
 * @param response - control frame specifier to send
 * @return int - link layer identifier in case of success, -1 in case of error
 */
int receiveControl(int fd, unsigned char controlField, unsigned char response);

/**
 * @brief Simulates the state of the current frame beeing read
 * 
 * @param byteReceived - received byte from serial port
 * @param currentState - current state of frame 
 * @param prevWasFlag - flag indicating if previous byte read was a flag
 */
void stateMachineReceiver(char byteReceived, enum state * currentState, int * prevWasFlag);

/**
 * @brief Reads information from serial port until end of frame
 * 
 * @param fd - link layer identifier
 * @param buffer - character buffer with read information
 * @return int - buffer size
 */
int receiveData(int fd, unsigned char * buffer);

#endif /*_RECEIVER_H*/
