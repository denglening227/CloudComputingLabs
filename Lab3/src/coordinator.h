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

#define NUM_PARTS 3
#define MAX_PARTS_NUM 10

using namespace std;

typedef int portnum;
typedef int fd;

struct sockaddr_in crd_addr, par_addr[MAX_PARTS_NUM];
map<portnum, fd> part_portnum_set;
map<fd, portnum> part_fd_set;
vector<float> commit_success;
map<int, float> timestamp_count;
