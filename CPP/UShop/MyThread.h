//copyright 2021 dragon@oct

//线程类
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
	static void SleepMillSec(int ttm);                //休眠，单位：毫秒
	static std::thread::id GetId(std::thread* th);    //获取线程ID
	virtual void Routine();                    //线程工作函数，具体功能一般由子类重写
protected:
	static void ThreadProc(void* p);
private:
	static std::mutex m_mut;           //线程锁
};