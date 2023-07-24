#include "BgiMasterApp.h"
#include "BgiMpsEvent.h"
#include "DriveHandlerContext.h"

BgiMasterApp::BgiMasterApp()
{
	//m_pProgContext = new ProgContext();
	m_pDriveHandlerContext = new DriveHandlerContext();

	//主线程接收Engine事件
	m_pThread = new QThread();
	this->moveToThread(m_pThread);
	m_pThread->start(QThread::NormalPriority);
}


BgiMasterApp::~BgiMasterApp()
{
	m_pThread->wait(1);
	m_pThread->quit();

	//if (m_pProgContext != NULL)
	//{
	//	delete m_pProgContext;
	//	m_pProgContext = NULL;
	//}

	if (m_pDriveHandlerContext != NULL)
	{
		delete m_pDriveHandlerContext;
		m_pDriveHandlerContext = NULL;
	}
}

BgiMasterApp* BgiMasterApp::Instance()
{
	static BgiMasterApp instance;
	return &instance;
}

void BgiMasterApp::InstallEventEventFilter(QObject* /*obj*/)
{
	//obj->installEventFilter(this);
}

bool BgiMasterApp::event(QEvent* pEvent)
{
	//obj事件发送的对象
	//事件类型为自定义类型
	if (pEvent->type() == BGI_MPS_EVENT_TYPE)
	{
		BgiMpsEvent* e = dynamic_cast<BgiMpsEvent*>(pEvent);
		m_pDriveHandlerContext->HandleEvent(e);

		return true;
	}

	return QObject::event(pEvent);
}

bool BgiMasterApp::eventFilter(QObject* obj, QEvent* pEvent)
{
	//obj事件发送的对象
	//事件类型为自定义类型
	if (pEvent->type() == BGI_MPS_EVENT_TYPE)
	{
		BgiMpsEvent* e = dynamic_cast<BgiMpsEvent*>(pEvent);
		m_pDriveHandlerContext->HandleEvent(e);

		return true;
	}

	return QObject::eventFilter(obj, pEvent);
}