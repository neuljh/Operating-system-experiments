#include<iostream>
#include<string>
#include<cstdlib>
#include<ctime>
#include<windows.h>
#include<stdlib.h>
#include<stdio.h>
#define random(a,b) (rand()%(b-a+1)+a)
#define TOTAL (100000)
using namespace std;

int nAccount1=0;//初始账户1
int nAccount2=0;//初始账户2
int nTemp1,nTemp2,nRandom;//分别代表了：nTemp1模拟存储账户1的寄存器；nTemp2模拟存储账户2的寄存器
//nRandom代表随机转账的金额
HANDLE mutex;//windows互斥信号量


DWORD WINAPI Thread_Func_windows(HANDLE Thread){
    int nLoop=0;//账户1和账户2之间转账的次数
    WaitForSingleObject(mutex,INFINITE);//对互斥信号量的申请，上锁操作
    do{
        nRandom=rand();
        nTemp1=nAccount1;
        nTemp2=nAccount2;
        nAccount1=nTemp1+nRandom;
        nAccount2=nTemp2-nRandom;//进行转账
        if(nAccount1+nAccount2!=0){
            break;
        }//若此时两个账户资金之和不为0，跳出循环
        nLoop++;//若仍然满足循环条件，循环次数+1
        ReleaseMutex(mutex);//对互斥信号量的释放，解锁操作
        WaitForSingleObject(mutex,INFINITE);//对互斥信号量的申请，上锁操作
    }while((nAccount1+nAccount2)==0&&nLoop<(TOTAL));
    ReleaseMutex(mutex);//对互斥信号量的释放，解锁操作
    WaitForSingleObject(mutex,INFINITE);//对互斥信号量的申请，上锁操作
    cout<<"The number of cycles is "<<nLoop<<endl;//打印循环次数
    ReleaseMutex(mutex);//对互斥信号量的释放，解锁操作
    return 0;
}



void solution(){
    HANDLE Thread[2];//创建两个线程对象
    DWORD begin,end;//定义两个系统时间
    begin=GetTickCount();//获取线程运行开始前的系统时间
    mutex=CreateMutex(NULL,false,NULL);//创建互斥信号量
    if((Thread[0]=CreateThread(NULL,0,Thread_Func_windows,NULL,0,NULL))==NULL){
        cout<<"Thread: Thread_Func_windows-0 failed to create!"<<endl;
        exit(0);
    }//实例化线程对象0,若实例化失败，则直接退出程序
    if((Thread[1]=CreateThread(NULL,0,Thread_Func_windows,NULL,0,NULL))==NULL){
        cout<<"Thread: Thread_Func_windows-1 failed to create!"<<endl;
        exit(0);
    }//实例化线程对象1,若实例化失败，则直接退出程序
    WaitForMultipleObjects(2,Thread,true,INFINITE);//主线程等待两个子线程的响应
    end=GetTickCount();//获取线程运行结束的系统时间
    cout<<"The total running time of the thread is "<<end-begin<<endl;//打印时间差
    CloseHandle(Thread);//关闭子线程
    CloseHandle(mutex);//关闭互斥信号量
}



int main(){
    solution();
    system("pause");
}


