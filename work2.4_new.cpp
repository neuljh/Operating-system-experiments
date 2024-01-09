
#include <Windows.h>
#include <iostream>
#include <cstdio>
#include <stdlib.h>
#include <time.h>
#define random(a,b) (rand()%(b-a+1)+a)
using namespace std;

const unsigned int PERSON=5;//定义哲学家人数
const unsigned int MEALS=3;//定义每个哲学家总共需要吃三顿饭吃饱
HANDLE chopstick_semaphore[PERSON];//定义筷子的互斥信号量
HANDLE Thread[PERSON];//每个哲学家定义一个子线程
HANDLE mutex;//子线程的互斥信号量
unsigned int nums = 0;//访问的人数序号

void eating(int id)
{//传进来为id的哲学家
    cout<<"%%%%%%%%%%%%%%%% Eat start %%%%%%%%%%%%%%%%%%5"<<endl;
    cout<<"Philosopher start to eat......"<<endl;
    for(int i=1;i<=PERSON;i++){
        cout<<"Philosopher  "<<i;
        if(i==id){
            cout<<"<---- is eating";
        }else{
            cout<<"<---- is thinking";
        }
        cout<<endl;
    }//判断哪些哲学家在吃饭，哪些在思考
    int num = random(1,100);//吃饭吃一段随机的时间
    Sleep(num);
    cout<<"Philosopher "<<id<<" eat for "<<num<<" seconds"<<endl;
    cout<<"%%%%%%%%%%%%%% Eat end %%%%%%%%%%%%%%%%%%%5"<<endl;
}

DWORD WINAPI Philosopher(LPVOID param)
{
    nums++;//循环哲学家的id
    int id = nums;
    int left_chopstick = id - 1;//左筷子的编号
    int rignt_chopstick = id % PERSON;//右筷子的编号
    int times = 0;//哲学家就餐的次数
    int ret1, ret2;//信号量的返回值
    while (true)
    {
        Sleep(100);
        if (times >= MEALS){
            break;
        }//如果哲学家已经吃了足够的饭，则直接退出
        if (id % 2 == 0){//如果哲学家编号为偶数
            ret1 = WaitForSingleObject(chopstick_semaphore[left_chopstick], 0);//取走左边筷子
            if (ret1 == WAIT_OBJECT_0){//如果能取走
                ret2 = WaitForSingleObject(chopstick_semaphore[rignt_chopstick], 0);//再取右边的筷子
                if (ret2 == WAIT_OBJECT_0){//如果能取走
                    WaitForSingleObject(mutex, INFINITE);
                    cout<<"Philosopher "<<id<<" gets two chopsticks and starts his "<<times+1<<"th meal"<<endl;
                    ReleaseMutex(mutex);
                    times++;
                    WaitForSingleObject(mutex, INFINITE);
                    eating(id);
                    ReleaseMutex(mutex);
                    WaitForSingleObject(mutex, INFINITE);
                    cout<<"Philosopher "<<id<<" has finished his/her meal"<<endl;
                    ReleaseMutex(mutex);
                    ReleaseSemaphore(chopstick_semaphore[rignt_chopstick], 1, NULL);//放回左边的筷子
                }//开始就餐
                ReleaseSemaphore(chopstick_semaphore[left_chopstick], 1, NULL);//放回右边的筷子
                }
        }
        else//如果哲学家编号为奇数，下面代码含义同上
        {
        ret1 = WaitForSingleObject(chopstick_semaphore[rignt_chopstick], 0);
        if (ret1 == WAIT_OBJECT_0)
        {
            ret2 = WaitForSingleObject(chopstick_semaphore[left_chopstick], 0);
            if (ret2 == WAIT_OBJECT_0)
            {
            WaitForSingleObject(mutex, INFINITE);
            cout<<"Philosopher "<<id<<" gets two chopsticks and starts his "<<times+1<<"th meal"<<endl;
            ReleaseMutex(mutex);
            times++;
            WaitForSingleObject(mutex, INFINITE);
            eating(id);
            ReleaseMutex(mutex);
            WaitForSingleObject(mutex, INFINITE);
            cout<<"Philosopher "<<id<<" has finished his/her meal"<<endl;
            ReleaseMutex(mutex);
            ReleaseSemaphore(chopstick_semaphore[left_chopstick], 1, NULL);
            }
            ReleaseSemaphore(chopstick_semaphore[rignt_chopstick], 1, NULL);
        }
        }
        WaitForSingleObject(mutex, INFINITE);
        ReleaseMutex(mutex);
    }
    cout<<"******** Philosopher "<<id<<" is full and leave now **********"<<endl;
    return 0;
}

int solution(){
    srand((unsigned)time(NULL));
    mutex = CreateMutex(NULL, false, NULL);
    for (int i = 0; i < PERSON; ++i){
        chopstick_semaphore[i] = CreateSemaphore(NULL, 1, 1, NULL);
    }
    for (int i = 0; i < PERSON; ++i){
        Thread[i] = CreateThread(NULL, 0, Philosopher, NULL, 0, NULL);
    }
    Sleep(5000);
    for (int i = 0; i < PERSON; ++i){
        CloseHandle(Thread[i]);
        CloseHandle(chopstick_semaphore[i]);
    }
    CloseHandle(mutex);
    Sleep(1500);
}

int main()
{
    solution();
    system("pause");
    return 0;
}