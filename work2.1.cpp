#include<iostream>
#include<string>
#include<cstdlib>
#include<ctime>
#include<windows.h>
#include<stdlib.h>
#include<stdio.h>
using namespace std;

const unsigned int BUFFER=5;//缓冲区长度
unsigned int PRODUCT_ID=0;//生产者ID
unsigned int CONSUME_ID=0;//消费者ID
unsigned int in;//产品进缓冲区时的缓冲区产品个数
unsigned int out;//产品出缓冲区时的缓冲区产品个数
int g_BUFFER[BUFFER];//缓冲区为循环队列
bool g_CONTINUE=true;//控制程序结束
HANDLE g_HMUTEX;//线程间的互斥对象
HANDLE g_HFULL_Semaphore;//缓冲区初始化为空，满则生产者等待
HANDLE g_HEMPTY_Semaphore;//空闲缓冲区，空则消费者等待

const unsigned int PRODUCE_COUNT=2;//生产者数量
const unsigned int CONSUMER_COUNT=1;//消费者数量
const unsigned int THREADS_COUNT=PRODUCE_COUNT+CONSUMER_COUNT;//线程的数据总量

void produce(){
    cout<<"********** Produce start **************"<<endl;
    cout<<"create product: "<<++PRODUCT_ID<<endl;
    g_BUFFER[in]=PRODUCT_ID;
    in=(in+1)%BUFFER;
    cout<<"put new product in buffer......"<<endl;
    for(int i=0;i<BUFFER;i++){
        cout<<i<<" :"<<g_BUFFER[i];
        if(i==in){
            cout<<"<---- produce";
        }
        if(i==out){
            cout<<"<---- consume";
        }
        cout<<endl;
    }
    cout<<"********** Produce end **************"<<endl;
}//生产的主逻辑函数

void consume(){
    cout<<"********** Consume start **************"<<endl;
    cout<<"get product from the buffer......"<<endl;
    CONSUME_ID=g_BUFFER[out];
    out=(out+1)%BUFFER;
    for(int i=0;i<BUFFER;i++){
        cout<<i<<" :"<<g_BUFFER[i];
        if(i==in){
            cout<<"<---- produce";
        }
        if(i==out){
            cout<<"<---- consume";
        }
        cout<<endl;
    }
    cout<<"consume product:"<<CONSUME_ID<<endl;
    cout<<"********** Consume end **************"<<endl;
}//消费的主逻辑函数

DWORD WINAPI producer(HANDLE Thread){
    while(g_CONTINUE){
        WaitForSingleObject(g_HFULL_Semaphore,INFINITE);
        WaitForSingleObject(g_HMUTEX,INFINITE);//
        produce();
        Sleep(1000);
        ReleaseMutex(g_HMUTEX);
        ReleaseSemaphore(g_HEMPTY_Semaphore,1,NULL);
    }
    return 0;
}//生产的子线程函数

DWORD WINAPI consumer(HANDLE Thread){
    while(g_CONTINUE){
        WaitForSingleObject(g_HEMPTY_Semaphore,INFINITE);
        WaitForSingleObject(g_HMUTEX,INFINITE);
        consume();
        Sleep(1000);
        ReleaseMutex(g_HMUTEX);
        ReleaseSemaphore(g_HFULL_Semaphore,1,NULL);
    }
    return 0;
}//消费的子线程函数

int solution(){
    g_HMUTEX=CreateMutex(NULL,false,NULL);//线程间的互斥对象,互斥锁的创建
    g_HFULL_Semaphore=CreateSemaphore(NULL,BUFFER-1,BUFFER-1,NULL);//生产者信号量
    g_HEMPTY_Semaphore=CreateSemaphore(NULL,0,BUFFER-1,NULL);//消费者信号量

    HANDLE h_Threads[THREADS_COUNT];//创建若干线程对象
    DWORD PRODUCER_ID[PRODUCE_COUNT];//生产者线程的标识
    DWORD CONSUMER_ID[CONSUMER_COUNT];//消费者线程的标识
    for(int i=0;i<PRODUCE_COUNT;i++){
        if(( h_Threads[i]=CreateThread(NULL,0,producer,NULL,0,&PRODUCER_ID[i]))==NULL){
            cout<<"Thread: Thread_Func_producer- "<<i<<" failed to create!"<<endl;
            exit(0);
        }
    }
    for(int i=0;i<CONSUMER_COUNT;i++){
        if((h_Threads[PRODUCE_COUNT+i]=CreateThread(NULL,0,consumer,NULL,0,&CONSUMER_ID[i]))==NULL){
            cout<<"Thread: Thread_Func_consumer- "<<i<<" failed to create!"<<endl;
            exit(0);
        }
    }
    while(g_CONTINUE){
        if(getchar()){
            g_CONTINUE=false;
        }
    }
    CloseHandle(h_Threads);
    CloseHandle(g_HFULL_Semaphore);
    CloseHandle(g_HEMPTY_Semaphore);
    CloseHandle(g_HMUTEX);//关闭信号量，锁和句柄
    return 0;
}

int main(){
    solution();
    system("pause");
}


