#include <Windows.h>
#include <iostream>
#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <io.h>
#include <string.h>
using namespace std;

int CountReader = 0; //用于记录当前的读者数量
int CountWriter = 0; //用于记录当前的写者数量
HANDLE readercount; //读者计数信号量
HANDLE writercount; //写者计数信号量
HANDLE rw; //用于保证读者和写者互斥地访问文件
HANDLE wait; //用于保证写优先（读者等待）
CRITICAL_SECTION FILES; //用于保护文件的临界区变量
unsigned const int Thread_NUM=10;//自定义的进程总数量
unsigned const int SLEEP_TIME=200;//模拟的运行时间

typedef struct PCB {
	string name; //进程名
	int requireTime; //请求时间
	int runningTime; //运行时间
}PCB;

//读者优先
//代码起始点A.....
//读者进程
DWORD WINAPI ReaderFisrt_reader(LPVOID lpParameter) {
	PCB* p = (PCB*)lpParameter;
	string name = p->name;
	Sleep((p->requireTime) * SLEEP_TIME); //模拟等待时间

	WaitForSingleObject(readercount, INFINITE);//P操作，互斥信号量申请（上锁)，申请进入读者计数器临界区
	CountReader++; // 读者计数器+1
	if (CountReader == 1) { //当第一个读进程读取时
		EnterCriticalSection(&FILES); //申请进入关于文件的临界区
		WaitForSingleObject(rw, INFINITE);//P操作，读进程读取文件，拒绝写进程进入
	}
	ReleaseMutex(readercount);//V操作，离开读者计数器临界区，互斥信号量释放（开锁）

	cout << name.c_str() << " is reading ..." << endl;
	Sleep((p->runningTime) * SLEEP_TIME); //模拟运行时间

	WaitForSingleObject(readercount, INFINITE);	//P操作，互斥信号量申请（上锁)，申请进入读者计数器临界区
	CountReader--; // 读者计数器-1
	if (CountReader == 0) { //当最后一个读进程读取完成时
		LeaveCriticalSection(&FILES);//离开关于文件的临界区
		ReleaseMutex(rw);//V操作，允许写进程进入，互斥信号量释放（开锁）
	}
	ReleaseMutex(readercount);//V操作，离开读者计数器临界区，互斥信号量释放（开锁）
	return 0;
}

//写者进程
DWORD WINAPI ReaderFisrt_writer(LPVOID lpParameter) {
	PCB* p = (PCB*)lpParameter;
	string name = p->name;
	Sleep((p->requireTime) * SLEEP_TIME); //模拟等待时间

	EnterCriticalSection(&FILES); //申请进入关于文件的临界区
	WaitForSingleObject(rw, INFINITE);
	cout << name.c_str() << " is writing ..." << endl;
	LeaveCriticalSection(&FILES); //离开关于文件的临界区
	ReleaseMutex(rw);	//V操作，允许写进程进入，互斥信号量释放（开锁）
	return 0;
}
//代码中止点A.....


//写者优先
//代码起始点B.....
//读者进程
DWORD WINAPI WriterFisrt_reader(LPVOID lpParameter) {
	PCB* p = (PCB*)lpParameter;
	string name = p->name;
	Sleep((p->requireTime) * SLEEP_TIME); //模拟等待时间

	WaitForSingleObject(rw, INFINITE); //P操作，读写互斥访问
	WaitForSingleObject(wait, INFINITE); //P操作，在没有写进程请求时申请进入临界区
	WaitForSingleObject(readercount, INFINITE); //P操作，进入读者计数器临界区
	CountReader++; // 读者计数器+1
	if (CountReader == 1) { //当第一个读进程读取时
		EnterCriticalSection(&FILES); //申请进入关于文件的临界区
	}
	ReleaseMutex(readercount);//V操作，离开读者计数器临界区
	ReleaseMutex(wait); //V操作
	ReleaseMutex(rw); //V操作

	cout << name.c_str() << " is writing ..." << endl;
	Sleep((p->runningTime) * SLEEP_TIME); //模拟运行时间

	WaitForSingleObject(readercount, INFINITE); //P操作，进入读者计数器临界区
	CountReader--; // 读者计数器-1
	if (CountReader == 0) { //当最后一个读进程读取完成时
		LeaveCriticalSection(&FILES);//离开关于文件的临界区
	}
	ReleaseMutex(readercount);//V操作，离开读者计数器临界区
	return 0;
}

//写者进程
DWORD WINAPI WriterFisrt_writer(LPVOID lpParameter) {
	PCB* p = (PCB*)lpParameter;
	string name = p->name;
	Sleep((p->requireTime) * SLEEP_TIME); //模拟等待时间

	WaitForSingleObject(writercount, INFINITE); //P操作，进入写者计数器临界区
	if (CountWriter == 0) { //如果为第一个写者
		WaitForSingleObject(wait, INFINITE); //P操作，开始执行写优先操作
	}
	CountWriter++;
	ReleaseMutex(writercount); //V操作，离开写者计数器临界区

	EnterCriticalSection(&FILES); //P操作，申请进入关于文件的临界区
	cout << name.c_str() << " is writing ..." << endl;
	Sleep((p->runningTime) * SLEEP_TIME); //模拟运行时间
	//V操作，允许写进程进入，互斥信号量释放（开锁）
	LeaveCriticalSection(&FILES); //离开关于文件的临界区

	//P操作，进入写者计数器临界区
	WaitForSingleObject(writercount, INFINITE);
	CountWriter--;
	if (CountWriter == 0) { //如果为最后一个写者
		ReleaseMutex(wait); //V操作，写操作完成
	}
	ReleaseMutex(writercount); //V操作，离开写者计数器临界区
	return 0;
}
//代码终止点B.....

void solution(int sign) {
	int i = 0;
	HANDLE Thread[Thread_NUM];
	PCB pro[Thread_NUM] = { {"r1",0,15},                          
					{"r2",1, 15},                         
					{"w1",3,3},
					{"r3",4, 2},
					{"w2",5,6},
					{"w3",6,10},
					{"r4",7,8},
					{"r5",9,2},
					{"w4",10,18},
					{"w5",12,2}
	};//初始化若干读写进程

	InitializeCriticalSection(&FILES); //初始化临界区变量
	readercount = CreateMutex(NULL, FALSE, NULL); //创建读者计数器信号量
	writercount = CreateMutex(NULL, FALSE, NULL); //创建写者计数器信号量
	rw = CreateMutex(NULL, FALSE, NULL); //创建互斥信号量，用于读者和写者互斥访问
	wait = CreateMutex(NULL, FALSE, NULL); //创建互斥信号量，用于确保写者优先

	cout << "Process application order:";
	for (i = 0; i < Thread_NUM; i++) {
		cout << pro[i].name << " ";
	}
	cout << endl;

	if (sign == 1) {
		cout << "Start reader priority process operation:" << endl;
		for (i = 0; i < Thread_NUM; i++) { //根据线程名称创建不同的线程
			if (pro[i].name[0] == 'r')  //名称的首字母是'r'则创建读者线程
				Thread[i] = CreateThread(NULL, 0, ReaderFisrt_reader, &pro[i].name, 0, NULL);
			else {                          //名称的首字母是'w'则创建写者线程		
				Thread[i] = CreateThread(NULL, 0, ReaderFisrt_writer, &pro[i].name, 0, NULL);
			}
			WaitForMultipleObjects(Thread_NUM, Thread, TRUE, -1); //等待所有线程结束
			
		}
	}
	else if (sign == 2) {
		cout << "Start a writer-first process operation:" << endl;
		for (i = 0; i < Thread_NUM; i++) { //根据线程名称创建不同的线程
			if (pro[i].name[0] == 'r')  //名称的首字母是'r'则创建读者线程
				Thread[i] = CreateThread(NULL, 0, WriterFisrt_reader, &pro[i].name, 0, NULL);
			else {                          //名称的首字母是'w'则创建写者线程		
				Thread[i] = CreateThread(NULL, 0, WriterFisrt_writer, &pro[i].name, 0, NULL);
			}
			WaitForMultipleObjects(Thread_NUM, Thread, TRUE, -1); //等待所有线程结束
		}
	}
    //CloseHandle(Thread);//关闭线程句柄
}

int main() {
	cout << "Select the algorithm to use: (1: Reader first / 2: Writer first)";//
	int sign;
	cin >> sign;
	solution(sign);
    cout<<"The end......"<<endl;
	system("pause");
}

