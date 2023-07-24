#include "DriveHandlerContext.h"
#include "BgiLogger.h"
#include <qserialport.h>
#include "BgiSerialPort.h"
#include "RobotParaHandler.h"
#include "EpsonRobotEventHandler.h"
#include "CoaterEventHandler.h"
#include "ImagerEventHandler.h"
#include "SchedulerEventHandler.h"
#include "ScannerEventHandler.h"

DriveHandlerContext::DriveHandlerContext()
{
	//MpsDevices::Instance()->CreateDriver();
	//机械手参数处理器
	m_pRobotParaHandler = new RobotParaHandler();
	//机械手事件处理器
	m_pEpsonRobotEventHandler = new EpsonRobotEventHandler();
	//m_pRobotParaHandler->SetEpsonRobotEventHandler(m_pEpsonRobotEventHandler);

	//涂布机事件处理器
	m_pCoaterEventHandler = new CoaterEventHandler();
	//光机事件处理器
	m_pImagerEventHandler = new ImagerEventHandler();

	//调度事件处理
	m_pSchedulerEventHandler = new SchedulerEventHandler();

	//扫码器事件处理
	m_pScannerEventHandler = new ScannerEventHandler();
}

DriveHandlerContext::~DriveHandlerContext()
{
	if (m_pRobotParaHandler != NULL)
	{
		delete m_pRobotParaHandler;
		m_pRobotParaHandler = NULL;
	} 
	if (m_pCoaterEventHandler != NULL)
	{
		delete m_pCoaterEventHandler;
		m_pCoaterEventHandler = NULL;
	}
	if (m_pImagerEventHandler != NULL)
	{
		delete m_pImagerEventHandler;
		m_pImagerEventHandler = NULL;
	}
	if (m_pSchedulerEventHandler != NULL)
	{
		delete m_pSchedulerEventHandler;
		m_pSchedulerEventHandler = NULL;
	}
	if (m_pScannerEventHandler != NULL)
	{
		delete m_pScannerEventHandler;
		m_pScannerEventHandler = NULL;
	}
}


bool DriveHandlerContext::HandleEvent(BgiMpsEvent* pEvent)
{
	bool bHandled = false;

	const QVariantHash& param_hash = pEvent->Params();
	QString event_name = pEvent->Key();
	if (event_name == "Driver_Event_Epson_Robot")
	{
		m_pEpsonRobotEventHandler->HandleEvent(pEvent);

		bHandled = true;
	}
	else if (event_name == "Driver_Event_Coater")
	{
		m_pCoaterEventHandler->HandleEvent(pEvent);

		bHandled = true;
	}
	else if (event_name == "Driver_Event_Imager")
	{
		m_pImagerEventHandler->HandleEvent(pEvent);

		bHandled = true;
	}
	else if (event_name == "Driver_Event_Scheduler")
	{
		m_pSchedulerEventHandler->HandleEvent(pEvent);

		bHandled = true;
	}
	else if (event_name == "Driver_Event_Scanner")
	{
		m_pScannerEventHandler->HandleEvent(pEvent);
		bHandled = true;
	}
	return true;
}
