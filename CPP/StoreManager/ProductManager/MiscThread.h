#pragma once

// �����̣߳�����һЩ����
class MiscThread : public nbase::FrameworkThread
{
public:
	/**
	* ���캯��
	* @param[in] thread_id �߳�id
	* @param[in] name �߳�����
	*/
	MiscThread(enum  ThreadId thread_id, const char *name);
	~MiscThread(void);

private:
	/**
	* �麯������ʼ�������߳�
	* @return void	�޷���ֵ
	*/
	virtual void Init() override;

	/**
	* �麯���������߳��˳�ʱ����һЩ������
	* @return void	�޷���ֵ
	*/
	virtual void Cleanup() override;

private:
	enum ThreadId m_threadId;
};

