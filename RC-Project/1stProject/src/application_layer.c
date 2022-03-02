#include "../headers/application_layer.h"

int appRead(int fd) {
  
  unsigned char buffer[255];
  int res, current_index = 0;
  off_t file_length_start = 0, current_position = 0, data_length = 0, file_length_end = 0;
  unsigned int name_size = 0;

  /* Receive start packet */
  if((res = llread(fd, buffer)) < 0) {
    perror("llread");
    exit(-1);
  }

  if(buffer[current_index++] != START_PKT) {
    perror("Start packet");
    exit(-1);
  }
  
  /* FILE LENGTH */
  if(buffer[current_index++] != FILE_SIZE) {
    perror("Start- Wrong type");
    exit(-1);
  }
  for(size_t i = 0, index = current_index++; i < buffer[index]; i++) {
    file_length_start = (file_length_start * 256) + buffer[current_index++];
  }

  /* FILE NAME */
  if(buffer[current_index++] != FILE_NAME) {
    perror("Start- Wrong type");
    exit(-1);
  }

  name_size = buffer[current_index];
  char * name = (char *)malloc(name_size);
  for(size_t i = 0, index = current_index++; i < buffer[index]; i++) {
    name[i] = buffer[current_index++];
  }

  int file_fd;
    

  if((file_fd = open(name, O_WRONLY | O_CREAT, 0444)) < 0) {
    free(name);
    perror("Open file");
    exit(-1);
  }

  /* Packet processing */
  while(current_position != file_length_start) {
    if((res = llread(fd, buffer)) < 0) {

      free(name);
      close(file_fd);
      perror("llread");
      exit(-1);
    }

    if(!res) continue;

    current_index = 0;
    if(buffer[current_index++] != DATA_PKT) {

      free(name);
      close(file_fd);
      perror("File not full received");
      exit(-1);
    }

    current_index++;

    data_length = buffer[current_index] * 256 + buffer[current_index+1];

    current_index+=2;
    char * data = (char *)malloc(data_length);
    for(size_t i = 0; i < data_length; i++) {
      data[i] = buffer[current_index++];
    }

    current_position += data_length;

    if(write(file_fd, data, data_length) < 0) {
      free(data);
      free(name);
      close(file_fd);
      perror("Write to file");
      exit(-1);
    }

    free(data);
  }

  close(file_fd);

  /* Receive end packet */
  current_index = 0;
  if((res = llread(fd, buffer)) < 0) {
    free(name);
    perror("llread");
    exit(-1);
  }

  if(buffer[current_index++] != END_PKT) {
    free(name);
    perror("End packet");
    exit(-1);
  }
  
  /* FILE LENGTH */
  if(buffer[current_index++] != FILE_SIZE) {
    free(name);
    perror("End- Wrong type");
    exit(-1);
  }
  
  for(size_t i = 0, index = current_index++; i < buffer[index]; i++) {
    file_length_end = (file_length_end * 256) + buffer[current_index++];
  }

  if(file_length_end != file_length_start) {
    free(name);
    perror("End - Wrong length");
    exit(-1);
  }

  /* FILE NAME */
  if(buffer[current_index++] != FILE_NAME) {
    free(name);
    perror("End - Wrong type");
    exit(-1);
  }

  if(buffer[current_index++] != name_size) {
    free(name);
    perror("End - Wrong name size");
    exit(-1);
  }

  for(size_t i = 0; i < name_size; i++) {
    if(name[i] != buffer[current_index++]) {
      free(name);
      perror("End - Wrong name");
      exit(-1);
    }
  }

  free(name);
  return 0;
}

int appWrite(int fd, char * name) {
  struct stat fileInfo;
  unsigned int current_index = 0;
  int resW=0, resR = 0, sequence_number = 0;

  if (stat(name, &fileInfo) == -1) {
      perror("stat");
      exit(EXIT_FAILURE);
  }
  off_t length = fileInfo.st_size;
  unsigned char control_packet[255];
  control_packet[current_index++] = START_PKT;
  control_packet[current_index++] = FILE_SIZE;
  control_packet[current_index] = ceil(log2(length) / 8.0);
  unsigned char *length_buf = (unsigned char *)malloc(control_packet[current_index]);

  for(size_t i = control_packet[current_index++]; i > 0; i--) {
      length_buf[i - 1] = length >> (8*(2-i));
  }

  for(size_t i = 0; i < control_packet[2]; i++) {
    control_packet[current_index++] = length_buf[i];
  }

  control_packet[current_index++] = FILE_NAME;
  control_packet[current_index++] = strlen(name);
  for(size_t i = 0; i < strlen(name); i++) {
    control_packet[current_index++] = name[i];
  }

  /* Send start packet */
  llwrite(fd, control_packet, current_index);

  /* Open transmission file */
  int file_fd;
  if((file_fd = open(name, O_RDONLY)) < 0) {
    free(length_buf);
    perror("Open file");
    exit(-1);
  }

  unsigned char data[PACKET_LENGTH];
  unsigned char *data_packet = (unsigned char *)malloc(PACKET_LENGTH);
  off_t current_position = 0;

  /* Send data packets */
  while (current_position != length) {

    if((resR = read(file_fd, data, PACKET_LENGTH)) < 0) {
      free(data_packet);
      free(length_buf);
      exit(-1);
    }

    data_packet = (unsigned char *)realloc(data_packet, resR+4);
    data_packet[0]= DATA_PKT;
    data_packet[1]= sequence_number%255;
    data_packet[2]= (resR/256);
    data_packet[3]= resR%256;
    for (size_t i = 0; i < resR; i++)
    {
      data_packet[4+i] = data[i];
    }
    if((resW = llwrite(fd, data_packet, resR + 4)) < 0) {
      free(data_packet);
      free(length_buf);
      exit(-1);
    }

    current_position += resR;
    sequence_number++;
  }

  control_packet[0] = END_PKT;

  /* Send end packet */
  llwrite(fd, control_packet, current_index);
  free(data_packet);
  free(length_buf);

  return 0;
}

int main(int argc, char** argv) {
  struct applicationLayer appLayer;
  int res = 0;
  
  if((argc != 3 && argc != 4) || 
     (argc == 3 && atoi(argv[argc - 1]) != RECEIVER) || 
     (argc == 4 && atoi(argv[argc - 1]) != TRANSMITTER) ||
     ((strcmp("/dev/ttyS0", argv[1])!=0) && (strcmp("/dev/ttyS1", argv[1])!=0))) {
    printf("Usage:\tnserial SerialPort\nex:\n\t./app /dev/ttySX fileName 1 [TRANSMITTER]\n\t./app /dev/ttySX 0 [RECEIVER]\n");
    exit(1);
  }

  /* Receive serial port and status specification */
  appLayer.status = atoi(argv[argc - 1]);
  appLayer.fileDescriptor = llopen(argv[1], appLayer.status);

  switch (appLayer.status) {
    case RECEIVER:
      res = appRead(appLayer.fileDescriptor);
      llclose(appLayer.fileDescriptor, RECEIVER);
      break;
    case TRANSMITTER:
      res = appWrite(appLayer.fileDescriptor, argv[2]);
      llclose(appLayer.fileDescriptor, TRANSMITTER);
      break;
  }
  
  return res;
}
