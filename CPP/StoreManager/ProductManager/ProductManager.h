#pragma once

#include "resource.h"

class MainThread : public nbase::FrameworkThread
{
public:
	MainThread():
		nbase::FrameworkThread("MainThread")
	{

	}

private:
	virtual void Init() override;
	virtual void Cleanup() override;

	void BeforeMessageLoop();

	void AfterMesssageLoop();
	void InitHttpService();
private:
	std::unique_ptr<PaymentThread> m_pPaymentThread;
	std::unique_ptr<MiscThread> m_pMiscThread;
};