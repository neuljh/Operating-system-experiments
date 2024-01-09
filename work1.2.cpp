#include<iostream>
#include<string>
#include<cstdlib>
#include<ctime>
#include<windows.h>
#include<stdlib.h>
#include<stdio.h>
#define random(a,b) (rand()%(b-a+1)+a)
#define TOTAL (10000000)
using namespace std;

int nAccount1=0;//初始账户1
int nAccount2=0;//初始账户2
int nLoop=0;//账户1和账户2之间转账的次数
int nTemp1,nTemp2,nRandom;//分别代表了：nTemp1模拟存储账户1的寄存器；nTemp2模拟存储账户2的寄存器
//nRandom代表随机转账的金额
bool flag[2];//peterson算法标志位
int turn;//peterson算法标志位

void change(int num){
    int temp=1-num;//计算另外一个线程的ID
    turn=temp;//赋值给turn,表示允许另外一个线程进入临界区
    flag[num]=true;//将本线程标志位设置为true,表示本线程想要进入临界区
    while(flag[temp]&&turn==temp){

    };//判断若另外一个线程想要进入临界区并且也允许其进入临界区，则本线程等待
}//对于线程ID为num的线程，进行标志位的初始化

DWORD WINAPI Thread_Func_peterson0(HANDLE Thread){
    change(0);//对ID为0的线程进行数据初始化
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
        flag[0]=false;//执行完成一次，设置标志位为false
        change(0);//对ID为0的线程进行数据修改
    }while((nAccount1+nAccount2)==0&&nLoop<(TOTAL));
    flag[0]=false;//执行完成，设置标志位为false
    change(0);//对ID为0的线程进行数据修改
    flag[0]=false;//设置标志位为false
    cout<<"The number of cycles (thread 0) is "<<nLoop<<endl;
    return 0;
}

DWORD WINAPI Thread_Func_peterson1(HANDLE Thread){
    change(1);//对ID为1的线程进行数据初始化
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
        flag[1]=false;//执行完成一次，设置标志位为false
        change(1);//对ID为1的线程进行数据修改
    }while((nAccount1+nAccount2)==0&&nLoop<(TOTAL));
    flag[1]=false;//执行完成，设置标志位为false
    change(1);//对ID为1的线程进行数据修改
    flag[1]=false;//设置标志位为false
    cout<<"The number of cycles (thread 1) is "<<nLoop<<endl;
    return 0;
}


void solution(){
    HANDLE Thread[2];//创建两个线程对象
    DWORD begin,end;//定义两个系统时间
    begin=GetTickCount();//获取线程运行开始前的系统时间
    if((Thread[0]=CreateThread(NULL,0,Thread_Func_peterson0,NULL,0,NULL))==NULL){
        cout<<"Thread: Thread_Func_peterson0-0 failed to create!"<<endl;
        exit(0);
    }//实例化线程对象0,若实例化失败，则直接退出程序
    if((Thread[1]=CreateThread(NULL,0,Thread_Func_peterson1,NULL,0,NULL))==NULL){
        cout<<"Thread: Thread_Func_peterson1-1 failed to create!"<<endl;
        exit(0);
    }//实例化线程对象1,若实例化失败，则直接退出程序
    WaitForMultipleObjects(2,Thread,true,INFINITE);//主线程等待所有子线程的通知
    end=GetTickCount();//获取线程运行结束的系统时间
    cout<<"The total running time of the thread is "<<end-begin<<endl;//打印时间差
    CloseHandle(Thread);//关闭子线程
}



int main(){
    solution();
    system("pause");
}


