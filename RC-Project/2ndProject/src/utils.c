#include "../headers/utils.h"


int getSubString(char ** to, char * from, char * delim) {
    char * token = strtok(from, delim);
    if (token == NULL) return -1;
    *to = (char *)malloc(strlen(token));
    strncpy(*to, token, strlen(token));
    return 0;
}

void printError(char * str) {
    printf("%s: Try: ./download ftp://[<user>:<password>@]<host>/<url-path>\n", str);
}

int validCredentials(char * user, char * password) {
    if((strcmp(user, "rcom")==0) && (strcmp(password, "rcom")==0)) return 1;
    return 0;
}

struct hostent* getIp(char * hostName) {
    struct hostent *host;

    if ((host = gethostbyname(hostName)) == NULL) {
        herror("gethostbyname()");
        return NULL;
    }

    return host;
}
