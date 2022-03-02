#include "../headers/download.h"

int connectToHost(char* server_address, int port) {
    int sockfd;
    struct sockaddr_in server_addr;

    /*server address handling*/
    bzero((char*)&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(server_address);    /*32 bit Internet address network byte ordered*/
    server_addr.sin_port = htons(port);                  /*server TCP port must be network byte ordered */

    /*open a TCP socket*/
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket()");
        return -1;
    }
    /*connect to the server*/
    if (connect(sockfd,
        (struct sockaddr*)&server_addr,
        sizeof(server_addr)) < 0) {
        perror("connect()");
        return -1;
    }

    return sockfd;
}

void getFileName(char* path, char** fileName) {
    char delim = '/';
    char* token = strtok(path, &delim);
    if (token == NULL) {
        *fileName = (char*)malloc(13);
        strncpy(*fileName, "fileReceived", 13);
        return;
    }

    while (token != NULL) {
        *fileName = (char*)malloc(strlen(token));
        strncpy(*fileName, token, strlen(token));
        token = strtok(NULL, &delim);
    }
}

int readFile(int fd, char* fileName) {
    //Create file
    int file_fd;
    if ((file_fd = open(fileName, O_WRONLY | O_CREAT, 0444)) < 0) {
        perror("Open file");
        return -1;
    }

    //Read information
    char buf[1];
    int res;
    printf("Receiving file...\n");

    while (1) {
        res = read(fd, &buf[0], 1);
        if (res == 0) break;
        if (res < 0) {
            close(file_fd);
            return -1;
        }
        res = write(file_fd, &buf[0], 1);
        if (res < 0) {
            close(file_fd);
            perror("Couldn't write to file");
        }
    }
    printf("File received\n");

    //Close file
    close(file_fd);
    return 0;
}

int download(char* server_address, char* user, char* password, char* path) {
    int sockfd;

    if ((sockfd = connectToHost(server_address, SERVER_PORT)) == -1) {
        return -1;
    }

    /*receive welcome msg*/
    int ret = receiveResponse(sockfd);
    if (ret != 220) {
        close(sockfd);
        printf("Bad server");
        return -1;
    }

    /*send username*/
    int length = 6 + strlen(user);
    char* message = (char*)malloc(length);
    snprintf(message, length, "user %s", user);
    message[length - 1] = '\n';
    printf("%s", message);

    if (!writeMsg(sockfd, message, length, 331)) {
        free(message);
        close(sockfd);
        perror("Login - user");
        return -1;
    }

    /*send password*/
    length = 6 + strlen(password);
    message = (char*)realloc(message, length);
    snprintf(message, length, "pass %s", password);
    message[length - 1] = '\n';
    printf("%s", message);

    if (!writeMsg(sockfd, message, length, 230)) {
        free(message);
        close(sockfd);
        perror("Login - password");
        return -1;
    }

    /*send pasv*/
    message = (char*)realloc(message, 5);
    snprintf(message, 5, "pasv");
    message[4] = '\n';
    printf("%s", message);
    char* response = (char*)malloc(255);
    if (!writeMsgToGetRes(sockfd, message, 5, 227, response)) {
        free(message);
        close(sockfd);
        perror("Set passive mode");
        return -1;
    }

    int ip1, ip2, ip3, ip4, port1, port2;
    sscanf(response, "Entering Passive Mode (%d,%d,%d,%d,%d,%d).", &ip1, &ip2, &ip3, &ip4, &port1, &port2);
    free(response);

    int port = port1 * 256 + port2;
    int sockfd2;

    if ((sockfd2 = connectToHost(server_address, port)) == -1) {
        return -1;
    }

    /*send retrieve*/
    length = 6 + strlen(path);
    message = (char*)realloc(message, length);
    snprintf(message, length, "retr %s", path);
    message[length - 1] = '\n';
    printf("%s", message);

    if (!writeMsg(sockfd, message, length, 150)) {
        free(message);
        close(sockfd);
        perror("Retrive");
        return -1;
    }

    /*receive file info*/
    char* fileName;
    getFileName(path, &fileName);
    readFile(sockfd2, fileName);
    free(fileName);

    /*receive response*/
    ret = receiveResponse(sockfd);

    free(message);
    close(sockfd);

    if (ret != 226) {
        perror("Transfer incomplete\n");
        return -1;
    }
    printf("transfer complete\n");
    return 0;
}
