#include<iostream>
#include<string>
#include<cstdlib>
#include<ctime>
#include<windows.h>
#include<stdlib.h>
#include<stdio.h>
#define random(a,b) (rand()%(b-a+1)+a)
using namespace std;

int nAccount1=0;//初始账户1
int nAccount2=0;//初始账户2
int nLoop=0;//账户1和账户2之间转账的次数
int nTemp1,nTemp2,nRandom;//分别代表了：nTemp1模拟存储账户1的寄存器；nTemp2模拟存储账户2的寄存器
//nRandom代表随机转账的金额
DWORD WINAPI Thread_Func_Not_synced(HANDLE Thread){
    do{
        nRandom=rand();
        nTemp1=nAccount1;
        nTemp2=nAccount2;
        nAccount1=nTemp1+nRandom;
        nAccount2=nTemp2-nRandom;
        nLoop++;
    }while((nAccount1+nAccount2)==0);//转账线程的主体逻辑结构
    cout<<"The number of cycles is "<<nLoop<<endl;//记录转账的次数，即循环次数
    return 0;
}//银行账户转账同步问题的抽象及未采取同步控制情况下的编程实现

void solution(){
    HANDLE Thread[2];//创建两个线程对象
    if((Thread[0]=CreateThread(NULL,0,Thread_Func_Not_synced,NULL,0,NULL))==NULL){
        cout<<"Thread: Thread_Func_Not_synced-0 failed to create!"<<endl;
        exit(0);
    }//实例化线程对象0,若实例化失败，则直接退出程序
    if((Thread[1]=CreateThread(NULL,0,Thread_Func_Not_synced,NULL,0,NULL))==NULL){
        cout<<"Thread: Thread_Func_Not_synced-1 failed to create!"<<endl;
        exit(0);
    }//实例化线程对象1,若实例化失败，则直接退出程序
    WaitForMultipleObjects(2,Thread,true,INFINITE);//实现主线程对两个银行账户转账的等待
    CloseHandle(Thread);//关闭线程句柄
}

int main(){
    solution();
    system("pause");
}












// class lab1_1{
//     private:
//     public:
//         static int nAccount1;
//         static int nAccount2;
//         static int nLoop;
//         static int nTemp1,nTemp2,nRandom;

//         // DWORD WINAPI ThreadFunc(HANDLE Thread){
//         //     do{
//         //         nRandom=random(1,100);
//         //         nTemp1=nAccount1;
//         //         nTemp2=nAccount2;
//         //         nAccount1=nTemp1+nRandom;
//         //         nAccount2=nTemp2-nRandom;
//         //         cout<<"Transfer out "<<nRandom<<" amount from account:nAccount1!"<<endl;
//         //         cout<<"Transfer in "<<nRandom<<" amount from account:nAccount2!"<<endl;
//         //         cout<<"Account: nAccount1 has "<<nAccount1<<" now !"<<endl;
//         //         cout<<"Account: nAccount2 has "<<nAccount2<<" now !"<<endl;
//         //         nLoop++;
//         //     }while((nAccount1+nAccount2)==0);
//         //     cout<<"The number of cycles is "<<nLoop<<endl;
//         // }

//         DWORD static WINAPI ThreadFunc(LPVOID lpParameter){
//             int *pid=(int*)lpParameter;
//             do{
//                 nRandom=random(1,100);
//                 nTemp1=nAccount1;
//                 nTemp2=nAccount2;
//                 nAccount1=nTemp1+nRandom;
//                 nAccount2=nTemp2-nRandom;
//                 cout<<"Transfer out "<<nRandom<<" amount from account:nAccount1!"<<endl;
//                 cout<<"Transfer in "<<nRandom<<" amount from account:nAccount2!"<<endl;
//                 cout<<"Account: nAccount1 has "<<nAccount1<<" now !"<<endl;
//                 cout<<"Account: nAccount2 has "<<nAccount2<<" now !"<<endl;
//                 nLoop++;
//             }while((nAccount1+nAccount2)==0);
//             cout<<"The number of cycles is "<<nLoop<<endl;
//         }

        

//         void solution(){
//             HANDLE Thread[2];
//             Thread[0]=CreateThread(NULL,0,ThreadFunc,NULL,0,NULL);
//         }

// };