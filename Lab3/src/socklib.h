#include <bitset>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <ctype.h>
// External libraries
#include <signal.h>
#include <iostream>
#include <iterator>

#define ERROR_SEND_FAIL -1
#define ERROR_RECEIVE_FAIL -1
#define ERROR_CLIENT_CLOSE -2

int send_msg(int sockfd, char *message, int length);
int recv_msg(int sockfd, char *message, int length);
