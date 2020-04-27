/*
 * @Author: your name
 * @Date: 2020-04-02 05:16:05
 * @LastEditTime: 2020-04-02 07:07:26
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /undefined/home/denglening/LabCodes/ComputerNetwork/client.cpp
 */
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <iostream>
using namespace std;

#define IP_Address "127.0.0.1"
#define portNumber 1234

#define BUF_SIZE 100

int main()
{
    // Create a file
    char filename[100] = {0};  
    printf("Input filename to save: ");
    cin.getline(filename, BUF_SIZE);
    FILE *fp = fopen(filename, "wb");
    if(fp == NULL)
    {
        printf("Cannot open file, Run this program.\n");
        exit(0);
    }


    // Create a socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    // Send Request to some Server
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(IP_Address);
    serv_addr.sin_port = htons(portNumber);
    connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

    char bufRecv[BUF_SIZE];
    int nCount;
    while((nCount = recv(sock, bufRecv, BUF_SIZE, 0))>0)
    {
        fwrite(bufRecv, nCount, 1, fp);
    }
    puts("File Transfer Success!\n");
    
    // Close everything
    fclose(fp);
    close(sock);
    return 0;
}