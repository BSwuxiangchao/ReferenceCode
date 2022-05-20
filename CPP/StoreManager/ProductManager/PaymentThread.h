#pragma once

/** @class PaymentThread
* @brief ��Ʒ֧���߳�
*/
class PaymentThread : public nbase::FrameworkThread
{
public:
	/**
	* ���캯��
	* @param[in] thread_id �߳�id
	* @param[in] name �߳�����
	*/
	PaymentThread(enum  ThreadId thread_id, const char *name);
	~PaymentThread(void);

private:
	/**
	* �麯������ʼ����Ʒ֧���߳�
	* @return void	�޷���ֵ
	*/
	virtual void Init() override;

	/**
	* �麯������Ʒ֧���߳��˳�ʱ����һЩ������
	* @return void	�޷���ֵ
	*/
	virtual void Cleanup() override;

private:
	enum ThreadId m_threadId;
};