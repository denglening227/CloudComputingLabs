#include <stdio.h>
#include <iostream>
#include <fstream>
using namespace std;



int main(int argc, char* argv[])
{
    // 判断参数
    static struct option long_options[] =
    {
        {"ip", required_argument, NULL, '1'},
        {"port", required_argument, NULL, '1'},
        {"number-thread", required_argument, NULL,'1'},
        {NULL,  0,   NULL, 0},
    };
    //判断是否缺少参数
	if(argc < 5)
	{
		printf("<Usage> Insert Arguments IP and Port.\n");
        printf("<Exmaple> ./httpserver --ip 127.0.0.1 --port 8888 --number-thread 2\n");
        printf("<Attention> IP and port are required. Threads number:2-8.\n");
		return 1;
	}

	//获取输入的IP、端口和线程数
	char* IP_Addr = argv[2];
	int portNumber = atoi(argv[4]);

    return 0;
}
