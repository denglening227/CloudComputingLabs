#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <time.h>

#define INPUT_JOB_NUM 10
#define INPUT_FOLDER_NAME "testInput"

#define THE_HEAVY_JOB_ID 1
int THE_HEAVY_WEIGHT=1;

#define CHUNK_SIZE 4096
#define TOTAL_CHUNK_NUM 10240

//动态分配任务
typedef unsigned char BYTE;

char inJob[INPUT_JOB_NUM][256];
int nextJobToBeDone=0;
//任务队列锁
pthread_mutex_t jobQueueMutex=PTHREAD_MUTEX_INITIALIZER;

//计算时间差
double time_diff(struct timeval x , struct timeval y)
{
  double x_ms , y_ms , diff;
  x_ms = (double)x.tv_sec*1000000 + (double)x.tv_usec;
  y_ms = (double)y.tv_sec*1000000 + (double)y.tv_usec;
  diff = (double)y_ms - (double)x_ms;
  if(diff<0)
  {
    fprintf(stderr, "ERROR! time_diff<0\n");
    exit(1);
  }
  return diff;
}

//创建任务（创建一个文本文件）
long int generateJobs()
{
  char command[1000];
  sprintf(command,"rm -rf %s",INPUT_FOLDER_NAME);
  int status=system(command);
  mkdir(INPUT_FOLDER_NAME, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
  BYTE writeBuf[CHUNK_SIZE];
  int writeSize=0;
  for(int i=0;i<CHUNK_SIZE;i++)
    writeBuf[i]=1;//All byte set to be 1
  long int totalBytes=0;
  for(int i=0;i<INPUT_JOB_NUM;i++)
  {
    sprintf(inJob[i], "%s/testInput%04d.txt",INPUT_FOLDER_NAME,i);
    FILE *fp;
    if((fp = fopen(inJob[i],"w"))==NULL)
    {
      perror("fopen ERROR!");
      exit(1);
    }
    int heavyScale=1;
    if(i==THE_HEAVY_JOB_ID)
      heavyScale=THE_HEAVY_WEIGHT;
    for(int j=0;j<TOTAL_CHUNK_NUM*heavyScale;j++)
    {
      while(1)//Write until this CHUNK_SIZE has all been written
      {
        writeSize=writeSize+fwrite(writeBuf, 1, CHUNK_SIZE-writeSize, fp);
        totalBytes=totalBytes+writeSize;
        if(writeSize<0) {
          perror("write ERROR!");
          exit(1);
        }
        else if(writeSize==CHUNK_SIZE) {
          //This CHUNK_SIZE done, go to the next CHUNK_SIZE
          writeSize=0;
          break;
        }
      }
    }
    fclose(fp);   
  }
  return totalBytes;
}

//
typedef struct {
  long int result;
} ThreadParas;

//获取任务
int recvAJob()
{
  int currentJobID=0;
  //进入临界区  获取任务队列锁
  pthread_mutex_lock(&jobQueueMutex);
  //如果下一个应该完成的任务号 大于输入任务总数  说明所有任务都执行完了
  if(nextJobToBeDone>=INPUT_JOB_NUM) 
  {
  //释放锁  返回-1表示执行任务完毕
    pthread_mutex_unlock(&jobQueueMutex);
    return -1;
  }
  //迭代任务号
  currentJobID=nextJobToBeDone;
  nextJobToBeDone++;
  //释放锁
  pthread_mutex_unlock(&jobQueueMutex);
  //返回任务号
  return currentJobID;
}

//计算任务的具体函数 通过任务号读取任务
void processAJob(int jobID, long int *sum)
{
  BYTE readBuf[CHUNK_SIZE]={0};
  int readSize=0;
  //IO从文件 根据任务号读取任务
  FILE *fp;
  if((fp = fopen(inJob[jobID],"r"))==NULL)
  {
    perror("fopen ERROR!");
    exit(1);
  }
  while(1)//Read until EOF
  {
    readSize=fread(readBuf, 1, CHUNK_SIZE, fp);
    if(readSize<0){
      perror("read ERROR!");
      exit(1);
    }
    else if(readSize==0){ //EOF
      break;
    }
    
    //累加的过程 把该部分任务和加到传入的long int *sum中
    for(int j=0;j<readSize;j++)
      *sum=*sum+readBuf[j];
    memset(readBuf,0,sizeof(BYTE)*readSize);
  }
  fclose(fp);
}

//线程计算总和的函数（包括获取动态任务、执行任务）
void* calcSum(void* args) {
  ThreadParas* para = (ThreadParas*) args;
  long int sum=0;
  int currentJobID=0;
  int *whichJobIHaveDone=malloc(INPUT_JOB_NUM*sizeof(int));//Remember which job I have done
  long int numOfJobsIHaveDone=0;//Remember how many jobs I have done
  while(1)
  {
    currentJobID=recvAJob();
    if(currentJobID==-1)//All job done!
      break;

    whichJobIHaveDone[numOfJobsIHaveDone]=currentJobID;
    numOfJobsIHaveDone++;

    processAJob(currentJobID,&sum);
  }
  
  //打印本线程执行的任务
  pthread_t tid = pthread_self();       
  printf("[%ld] thread (sum of inJobs["
    , pthread_self());
  for(int i=0;i<numOfJobsIHaveDone;i++) 
  {
    if(i>0)
      printf(",");
    printf("%d",whichJobIHaveDone[i]);
  }
  printf("]): \t %ld\n", sum);
  para->result=sum;
}

int main(int argc, char *argv[])
{
  int numOfWorkerThread=1;
  if(argc>=2)
    numOfWorkerThread=atoi(argv[1]);
  if(numOfWorkerThread>INPUT_JOB_NUM)
    numOfWorkerThread=INPUT_JOB_NUM;
  if(argc>=3)
    THE_HEAVY_WEIGHT=atoi(argv[2]);

  struct timeval tvGenStart,tvEnd;
  struct timeval tvMainStartCacl,tvMainEndCacl;
  struct timeval tvWorkerStartCacl,tvWorkerEndCacl;

//生成任务
  printf("Generating input jobs ...\n");
  gettimeofday(&tvGenStart,NULL);
  long int totalBytes=generateJobs();
  gettimeofday(&tvEnd,NULL);
  printf("Generating input jobs done. Spend %.5lf s to finish. Total test input data size is %lf MBs\n",time_diff(tvGenStart,tvEnd)/1E6,(double)totalBytes/1E6);
//主线程计算总和
  printf("Main thread start doing jobs ...\n");
  gettimeofday(&tvMainStartCacl,NULL);
  ThreadParas thParaMain;
  calcSum(&thParaMain);
  gettimeofday(&tvMainEndCacl,NULL);
  printf("Main thread finish jobs. Spend %.5lf s to finish.\n",time_diff(tvMainStartCacl,tvMainEndCacl)/1E6);


//多线程计算总和
  nextJobToBeDone=0;
  printf("Worker threads start doing jobs ...\n");
  gettimeofday(&tvWorkerStartCacl,NULL);
  pthread_t th[numOfWorkerThread];
  ThreadParas thPara[numOfWorkerThread];
  //创建并开启若干个计算线程
  for(int i=0;i<numOfWorkerThread;i++)
  {
  //每一个计算线程的参数是thPara数组中的一个数字  那么等所有计算线程都执行完毕 
  //thPara数组中所以有数字的和就是最终计算结果
    if(pthread_create(&th[i], NULL, calcSum, &thPara[i])!=0)
    {
      perror("pthread_create failed");
      exit(1);
    }
  }
  //等待所有的计算线程返回
  for(int i=0;i<numOfWorkerThread;i++)
    pthread_join(th[i], NULL);
  //停止计时
  gettimeofday(&tvWorkerEndCacl,NULL);
  printf("Worker threads finish jobs. Spend %.5lf s to finish.\n",time_diff(tvWorkerStartCacl,tvWorkerEndCacl)/1E6);

//计算结果总和
  long int workerSum=0;
  for(int i=0;i<numOfWorkerThread;i++)
    workerSum=workerSum+thPara[i].result;
  printf("Sum of all %d threads: \t\t %ld\n", numOfWorkerThread, workerSum);

  //In real project, do free the memory and destroy mutexes and semaphores
  exit(0);
}
