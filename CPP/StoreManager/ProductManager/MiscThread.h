#pragma once

// 杂项线程，处理一些琐事
class MiscThread : public nbase::FrameworkThread
{
public:
	/**
	* 构造函数
	* @param[in] thread_id 线程id
	* @param[in] name 线程名字
	*/
	MiscThread(enum  ThreadId thread_id, const char *name);
	~MiscThread(void);

private:
	/**
	* 虚函数，初始化杂项线程
	* @return void	无返回值
	*/
	virtual void Init() override;

	/**
	* 虚函数，杂项线程退出时，做一些清理工作
	* @return void	无返回值
	*/
	virtual void Cleanup() override;

private:
	enum ThreadId m_threadId;
};

