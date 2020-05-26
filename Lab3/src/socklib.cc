#include "socklib.h"
int send_msg(int sockfd, char *message, int length) {
	int index = 0, nbytes;
	int left = length;
	while (left > 0) {
		nbytes = send(sockfd, &message[index], left, 0);
		if (nbytes == -1) {
			return ERROR_SEND_FAIL;
		} else if (nbytes == 0) {
			return ERROR_CLIENT_CLOSE;
		}else {
			left -= nbytes;
			index += nbytes;
		}
	}
	return 0;
}

int recv_msg(int sockfd, char *message, int length) {
	int index = 0, nbytes;
	int left = length;
	while (left > 0) {
		nbytes = recv(sockfd, &message[index], left, 0);
		if (nbytes == -1) {
			return ERROR_SEND_FAIL;
		} else if (nbytes == 0) {
			return ERROR_CLIENT_CLOSE;
		}else {
			left -= nbytes;
			index += nbytes;
		}
	}
	return 0;
}
