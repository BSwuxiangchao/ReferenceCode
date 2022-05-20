//copyright 2021 dragon@oct

//�߳���
/* $Header:PlcServer/MyThread.h     5/27/2021 16:39                      */
/**************************************************************************
***                                                                     ***
*                                                                         *
*                    Project Name: PlcServer                              *
*                                                                         *
*                       File Name: MyThread.h                             *
*                                                                         *
*                      Start Date: 5/27/2021                              *
*                                                                         *
*                       Last Date: 5/27/2021                              *
*-------------------------------------------------------------------------*
* Functions:                                                              *
*-------------------------------------------------------------------------*
**************************************************************************/

#pragma once
#include<thread>
#include<mutex>
#include<chrono>
#include<future>
#include<iostream>

class MyThread
{
public:
	MyThread();
	virtual ~MyThread();
	std::thread* CreateThreadS();
	static void Join(std::thread* th);
	static void Detach(std::thread* th);
	static void Lock();
	static void UnLock();
	static void Unique_Lock();
	static void SleepMillSec(int ttm);                //���ߣ���λ������
	static std::thread::id GetId(std::thread* th);    //��ȡ�߳�ID
	virtual void Routine();                    //�̹߳������������幦��һ����������д
protected:
	static void ThreadProc(void* p);
private:
	static std::mutex m_mut;           //�߳���
};