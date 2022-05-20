#include"stdafx.h"
#include"MyThread.h"

std::mutex MyThread::m_mut;

MyThread::MyThread()
{

}
MyThread::~MyThread()
{

}
std::thread* MyThread::CreateThreadS()
{
	std::thread* th = new std::thread(ThreadProc, this);
	return th;
}
void MyThread::Join(std::thread* th)
{
	if (th->joinable())
		th->join();
	delete th;
	th = nullptr;
}
void MyThread::Detach(std::thread* th)
{
	th->detach();
}
void MyThread::Lock() 
{
	m_mut.lock();
}
void MyThread::Unique_Lock()
{
	std::unique_lock<std::mutex>Lock(m_mut);
}
void MyThread::UnLock()
{
	m_mut.unlock();
}
void MyThread::SleepMillSec(int ttm)
{
	std::this_thread::sleep_for(std::chrono::milliseconds(ttm));
}
std::thread::id MyThread::GetId(std::thread* th)
{
	return th->get_id();
}
void MyThread::ThreadProc(void* p)
{
	MyThread* pth = static_cast<MyThread*>(p);
	pth->Routine();
}
void MyThread::Routine()
{

}
