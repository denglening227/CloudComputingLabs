#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <string>
#include <string.h>
#include <sys/types.h>
#include <sys/select.h>
#include <set>
#include <sstream>
#include <iostream>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <map>
#include <ctime>
#include <vector>
#include <sys/time.h>

#define NUM_CLIENTS 1

using namespace std;

typedef string hostname;
typedef int fd;

struct sockaddr_in serv_addr, cli_addr;
map<hostname, fd> client_hostname_set;
map<fd, hostname> client_fd_set;
vector<float> commit_success;
map<int, float> timestamp_count;
