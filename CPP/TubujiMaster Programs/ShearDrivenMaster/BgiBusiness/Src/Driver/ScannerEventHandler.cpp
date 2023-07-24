#include "ScannerEventHandler.h"
#include "ScannerDriver.h"
#include "ErrorCodeTranslate.h"
#include "MasterDevices.h"

ScannerEventHandler::ScannerEventHandler()
{
	m_pScannerDriver = MasterDevices::Instance()->GetScannerDriver();
}

ScannerEventHandler::~ScannerEventHandler()
{
}

bool ScannerEventHandler::HandleEvent(BgiMpsEvent* pEvent)
{
	m_curEvent = pEvent;
	QString event_action = pEvent->Action();
	
	if (!m_pScannerDriver->GetConnectStatus() )
	{
		ErrorCodeTranslate::Instance()->PromptInfo("扫码异常", "扫码器网络未连接，请检查网络状态！", gc_colorYellow);
		return false;
	}
	if (event_action == "StartReadCode")
	{
		m_pScannerDriver->StartReadCode();
	}
	return true;
}