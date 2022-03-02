#include "../headers/link_layer.h"

struct termios oldtio;
int flag, conta;
struct linkLayer ll = {0, 3, 0};

int llopen(char* path, int status) {

  int fd = open(path, O_RDWR | O_NOCTTY );
  
  struct termios newtio;

  (void) signal(SIGALRM, sigAlarmHandler);

  if ( tcgetattr(fd,&oldtio) == -1) { /* save current port settings */
    perror("tcgetattr");
    exit(-1);
  }

  bzero(&newtio, sizeof(newtio));
  newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
  newtio.c_iflag = IGNPAR;
  newtio.c_oflag = 0;

  /* set input mode (non-canonical, no echo,...) */
  newtio.c_lflag = 0;
    
  newtio.c_cc[VTIME]    = 5;   /* inter-character timer unused */
  newtio.c_cc[VMIN]     = 0;   /* blocking read until 1 chars received */

  tcflush(fd, TCIOFLUSH);

  if ( tcsetattr(fd,TCSANOW,&newtio) == -1) {
    perror("tcsetattr");
    exit(-1);
  }

  printf("New termios structure set\n");

  if(status) return sendControl(fd, SETUP, UAKN);
  
  return receiveControl(fd, SETUP, UAKN);
}

int llclose(int fd, int status) {
  int res = 0;
  switch (status) {
    case TRANSMITTER:
      if((res = sendControl(fd, DISC, DISC)) < 0) return -1;
      if((res = sendMessage(fd, A_SND, UAKN)) < 0) return -1;
      sleep(1);
      break;
    case RECEIVER:
      if((res = receiveControl(fd, DISC, DISC)) < 0) return -1;
      if((res = receiveMessage(fd, A_SND, UAKN)) < 0) return -1;
      break;
  }

  if ( tcsetattr(fd, TCSANOW, &oldtio) == -1) {
    perror("tcsetattr");
    exit(-1);
  }
  close(fd);
  return res;
}

int llwrite(int fd, unsigned char * buffer, int length) {
  unsigned char * newBuffer = (unsigned char *) malloc(length + 6);
  unsigned int newLength = length + 6;

  newBuffer[0] = FLAG;
  newBuffer[1] = A_SND;
  newBuffer[2] = SEQUENCENUMBER(ll.sequenceNumber);
  newBuffer[3] = BCC(A_SND, newBuffer[2]);

  unsigned int bcc2 = 0x0;
  for(size_t i = 0; i < length; i++) {
    newBuffer[4 + i] = buffer[i];
    bcc2 ^= buffer[i];
  }
  newBuffer[newLength - 2] = bcc2;
  newBuffer[newLength - 1] = FLAG;
  struct frame * toStuff = (struct frame *) malloc(sizeof(struct frame));
  toStuff->buffer = newBuffer;
  toStuff->length = newLength;

  struct frame * frame = stuffing(toStuff);
  free(toStuff->buffer);
  free(toStuff);

  int res = sendData(fd, frame);

  free(frame->buffer);
  free(frame);

  return res;
  
}

int llread(int fd, unsigned char * buffer) {
  int res;
  unsigned char buf[255];
    
  if((res = receiveData(fd, buf)) < 0) return -1;

  struct frame * toDestuff = (struct frame *) malloc(sizeof(struct frame));
  toDestuff->buffer = buf;
  toDestuff->length = res;

  struct frame * frame = destuffing(toDestuff);
  free(toDestuff);

  /* Wrong header - ignore */
  if (frame->buffer[0] != FLAG || frame->buffer[1] != A_SND ||
      frame->buffer[3] != (BCC(A_SND, SEQUENCENUMBER(ll.sequenceNumber))) ) {
    
    free(frame->buffer);
    free(frame);
    return 0;
  }

  unsigned char prevSequenceNumber = (ll.sequenceNumber) ? 0:1;
  /* Duplicate frame - ignore */
  if(frame->buffer[2] == SEQUENCENUMBER(prevSequenceNumber)) {
    sendMessage(fd, A_RCVR, RR(ll.sequenceNumber));
    free(frame->buffer);
    free(frame);
    return 0;
  }
  /* Wrong header - ignore */
  if( frame->buffer[2] != SEQUENCENUMBER(ll.sequenceNumber)) {
    free(frame->buffer);
    free(frame);
    return 0;
  }

  unsigned char bcc2 = 0x00;
  for(size_t i = 4; i < (frame->length - 2); i++) {
    bcc2 ^= frame->buffer[i];
    buffer[i - 4] = frame->buffer[i];
  }

  /* Wrong BCC2 */
  if(frame->buffer[frame->length - 2] != bcc2) {
    sendMessage(fd, A_RCV, REJ(ll.sequenceNumber));    
    free(frame->buffer);
    free(frame);
    return 0;
  }

  free(frame);

  /* Correct frame */
  ll.sequenceNumber = prevSequenceNumber;
  sendMessage(fd, A_RCVR, RR(ll.sequenceNumber));

  return res - 6;
}

struct frame * stuffing(struct frame * toStuff) {
  struct frame * newFrame = (struct frame *) malloc(sizeof(struct frame));
  newFrame->buffer = (unsigned char *)malloc(255);

  unsigned int newIndex = 0;
  newFrame->buffer[newIndex++] = FLAG;
  for(size_t i = 1; i < toStuff->length - 1; i++) {
    if (toStuff->buffer[i] == FLAG) {
      newFrame->buffer[newIndex++] = ESC;
      newFrame->buffer[newIndex++] = (NEWVAL(FLAG));
      continue;
    }
    if(toStuff->buffer[i] == ESC) {
      newFrame->buffer[newIndex++] = ESC;
      newFrame->buffer[newIndex++] = (NEWVAL(ESC));
      continue;
    }
    newFrame->buffer[newIndex++] = toStuff->buffer[i];
  }
  newFrame->buffer[newIndex++] = FLAG;
  newFrame->length = newIndex;
  return newFrame;
}

struct frame * destuffing(struct frame * toDestuff) {
  struct frame * newFrame = (struct frame *) malloc(sizeof(struct frame));
  newFrame->buffer = (unsigned char *)malloc(255);

  unsigned int newIndex = 1;
  newFrame->buffer[0] = FLAG;
  for(size_t i = 1; i < toDestuff->length - 1; i++) {
    if (toDestuff->buffer[i] == ESC) {
      i++;
      if(toDestuff->buffer[i] == (NEWVAL(FLAG)) ) {
        newFrame->buffer[newIndex++] = FLAG;
        continue;
      }
      newFrame->buffer[newIndex++] = ESC;
      continue;
    }
    newFrame->buffer[newIndex++] = toDestuff->buffer[i];
  }
  newFrame->buffer[newIndex++] = FLAG;
  newFrame->length = newIndex;

  return newFrame;
}
