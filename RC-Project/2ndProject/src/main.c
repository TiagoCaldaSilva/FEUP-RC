#include "../headers/utils.h"
#include "../headers/download.h"

int main(int argc, char * argv[]) {
    char * user = NULL, * password = NULL, * hostName = NULL, * urlPath;

    if ((argc != 2) || (strncmp(argv[1], "ftp://", 6) != 0)) {
        printError("Invalid arguments.");
        exit(-1);
    }

    int length = strlen(argv[1]) - 6;
    if(length < 3) {
        printError("Invalid arguments.");
        exit(-1);
    }
    
    char * newBuf = (char *)malloc(length);
    strncpy(newBuf, argv[1] + 6, length);

    char * currentString = NULL;
    if (getSubString(&user, newBuf, ":") < 0) {
        printError("Invalid arguments.");
        exit(-1);
    }

    // Anonim
    if (strlen(user) != length) {
        if (getSubString(&password, currentString, "@") < 0) {
            printError("Invalid arguments.");
            exit(-1);
        }

        if(!validCredentials(user, password)) {
            printError("Invalid credentials.");
            exit(-1);
        }
    } else {
        free(user);
        user = (char *)malloc(10);
        password = (char *)malloc(10);
        strncpy(user, "anonymous", 10);
        strncpy(password, "anonymous", 10);
        currentString = (char *)malloc(length);
        strncpy(currentString, newBuf, length);
    }

    if (getSubString(&hostName, currentString, "/") < 0) {
        printError("Invalid arguments");
        exit(-1);
    }

    if (getSubString(&urlPath, NULL, " ") < 0) {
        urlPath = (char *)malloc(1);
        urlPath = "/";
    }

    struct hostent * host;
    if ((host = getIp(hostName)) != NULL) {
        printf("Host name  : %s\n", host->h_name);
        printf("IP Address : %s\n", inet_ntoa(*((struct in_addr *) host->h_addr)));

        download(inet_ntoa(*((struct in_addr *) host->h_addr)), user, password, urlPath);
    }

    free(user);
    free(password);
    free(hostName);
    free(urlPath);
}
