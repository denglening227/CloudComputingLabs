/*
 * @Author: your name
 * @Date: 2020-04-27 17:37:01
 * @LastEditTime: 2020-04-27 18:12:10
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: home/denglening/Github/CloudComputingLabs/Lab2/src/httpserver.cpp
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define IP_Address "127.0.0.1"
#define portNumber 1234

#define BUF_SIZE 100
#define FILE_NAME "test"

int main()
{
    // Check the file out
    char* filename = FILE_NAME;
    FILE *testFile = fopen(filename, "r");
    if (testFile == NULL) 
    {
        printf("Error <100> | Open file failed. Run this program one more time.");
        exit(0);
    }

    // Create socket
    int serv_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    // Tie socket up with IP & Port
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr)); // Fill up with '0'
    serv_addr.sin_family = AF_INET;    // Use IPv4 address
    serv_addr.sin_addr.s_addr = inet_addr(IP_Address); // IP address
    serv_addr.sin_port = htons(portNumber); //Port
    bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    // Listening & Waiting User send something
    listen(serv_sock, 20);

    // Accept client's request
    struct sockaddr_in clnt_addr;
    socklen_t clnt_addr_size = sizeof(clnt_addr);
    int clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);

    // Send data till it ends
    char buffer[BUF_SIZE] = {0};
    int nCount;
    while((nCount = fread(buffer, 1, BUF_SIZE, testFile))>0)
    {
        // Send Data to Client
        send(clnt_sock, buffer, nCount, 0);
    }
    

    // Close socket
    fclose(testFile);
    close(serv_sock);
    close(clnt_sock);

    return 0;
}