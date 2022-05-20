#pragma once

/** @class PaymentThread
* @brief 商品支付线程
*/
class PaymentThread : public nbase::FrameworkThread
{
public:
	/**
	* 构造函数
	* @param[in] thread_id 线程id
	* @param[in] name 线程名字
	*/
	PaymentThread(enum  ThreadId thread_id, const char *name);
	~PaymentThread(void);

private:
	/**
	* 虚函数，初始化商品支付线程
	* @return void	无返回值
	*/
	virtual void Init() override;

	/**
	* 虚函数，商品支付线程退出时，做一些清理工作
	* @return void	无返回值
	*/
	virtual void Cleanup() override;

private:
	enum ThreadId m_threadId;
};