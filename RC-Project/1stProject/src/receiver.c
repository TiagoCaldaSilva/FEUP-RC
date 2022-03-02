#include "../headers/receiver.h"

int receiveControl(int fd, unsigned char controlField, unsigned char response) {

  if(receiveMessage(fd, A_SND, controlField) < 0) return -1;

  if(sendMessage(fd, A_RCVR, response) < 0) return -1;
  
  return fd;
}

void stateMachineReceiver(char byteReceived, enum state * currentState, int * prevWasFlag) {
  switch (*currentState) {
    case START: {
      if(byteReceived == FLAG) {
       *currentState = FLAG_RCV;
       *prevWasFlag = 1;
       }
      break;
    }
    case FLAG_RCV: {
      if (byteReceived == FLAG) *currentState = (*prevWasFlag) ? FLAG_RCV:STOP;
      else *prevWasFlag = 0;
      break;
    }
    default: 
      break;
  }
}

int receiveData(int fd, unsigned char * buffer) {
  enum state current_state = START;
  char buf[255];
  int res = 0, current_index = 0;
  int prevWasFlag = 0;

  do {
    if((res = read(fd, buf, 1)) < 0)  {
      return -1;
    }

    if (!res) continue;

    stateMachineReceiver(buf[0], &current_state, &prevWasFlag);
    if(prevWasFlag && current_state == FLAG_RCV) current_index = 0;
    if(current_state != START) buffer[current_index++] = buf[0];
  } while(current_state != STOP);

  return current_index;
}
