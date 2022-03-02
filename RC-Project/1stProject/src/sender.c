#include "../headers/sender.h"

extern struct linkLayer ll;
int counter;

void sigAlarmHandler(){
	printf("alarme #%d\n", counter);
	ll.alarmFlag = 1;
	counter++;
}

int sendControl(int fd, unsigned char controlField, unsigned char response) {
  
  if(sendMessage(fd, A_SND, controlField) < 0) return -1;
  
  if(receiveMessage(fd, A_RCVR, response) < 0) return -1;
  
  return fd;
}

void stateMachineSender(unsigned char byteReceived, enum state * currentState, unsigned char * controlField) {
  switch (*currentState) {
      case START:
      if(byteReceived == FLAG) *currentState = FLAG_RCV;
      break;
    case FLAG_RCV:
      if(byteReceived == A_RCVR) *currentState = A_RCV;
      else if (byteReceived != FLAG) *currentState = START;
      break;
    case A_RCV:
      if(byteReceived == FLAG) *currentState = FLAG_RCV;
      else if(byteReceived == (RR(1)) || byteReceived == (RR(0)) || byteReceived == (REJ(0)) || byteReceived == (REJ(1))) {
        *controlField = byteReceived;
        *currentState = C_RCV;
      }
      else *currentState = START;
      break;
    case C_RCV:
      if(byteReceived == FLAG) *currentState = FLAG_RCV;
      else if(byteReceived == (BCC(A_RCVR, *controlField))) *currentState = BCC_OK;
      else *currentState = START;
      break;
    case BCC_OK:
      if(byteReceived == FLAG) *currentState = STOP;
      else *currentState = START;
      break;
    default:
      break;
  }
}

int sendData(int fd, struct frame * frame) {
  enum state current_state = START;
  char buf[255];
  int resR = 0, resW = 0;
  ll.alarmFlag = 0;
  counter = 0;
  unsigned char controlField;

  resW = write(fd, frame->buffer, frame->length);
  /* Activates 3s alarm */
  alarm(3);

  do {
    if(ll.alarmFlag) {
      /* Activates 3s alarm */
      alarm(3);
      ll.alarmFlag = 0;
      resW = write(fd, frame->buffer, frame->length);
    }
    
    resR = read(fd, buf, 1);
    if (!resR) continue;
    stateMachineSender(buf[0], &current_state, &controlField);
  } while(current_state != STOP && counter <= ll.timeOutMax);

  alarm(0);

  int nextSequenceNumber = (ll.sequenceNumber) ? 0:1;
  if(current_state == STOP) {
    if( controlField == (REJ(ll.sequenceNumber)) ) {
      return sendData(fd, frame);
    }
    if( controlField == (RR(nextSequenceNumber)) ) {
      ll.sequenceNumber = nextSequenceNumber;
      return resW;
    }

  }
  return -1;
}
