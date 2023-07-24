#include "CoaterEventHandler.h"
#include "MasterDevices.h"
#include "CoaterDriver.h"

CoaterEventHandler::CoaterEventHandler()
{
	m_CoaterDriver = MasterDevices::Instance()->GetCoaterDriver();
}

CoaterEventHandler::~CoaterEventHandler()
{}

bool CoaterEventHandler::HandleEvent(BgiMpsEvent* pEvent)
{
	m_curEvent = pEvent;

	QString action = pEvent->Action();
	
	if (action == "GetConnStatus")
	{
		m_CoaterDriver->GetConnStatus();
	}
	else if (action == "Startup")
	{
		m_CoaterDriver->Startup();
	}
	else if (action == "Pause")
	{
		HandlePause();
	}
	else if (action == "Reset")
	{
		m_CoaterDriver->Reset();
	}
	else if (action == "Stop")
	{
		m_CoaterDriver->Stop();
	}
	else if (action == "Halt")
	{
		m_CoaterDriver->Halt();
	}
	else if (action == "SwitchVacuum")
	{
		SwitchVacuum();
	}
	else if (action == "AlarmReset")
	{
		m_CoaterDriver->AlarmReset();
	}
	else if (action == "ReadCmdData")
	{
		ReadCmdData();
	}

	return true;
}

void CoaterEventHandler::ReadCmdData()
{
	const QVariantHash& param_hash = m_curEvent->Params();
	QVariant para1 = param_hash["Param1"];
	bool bRead = para1.toBool();

	m_CoaterDriver->ReadCmdData(bRead);
}

void CoaterEventHandler::HandlePause()
{
	const QVariantHash& param_hash = m_curEvent->Params();
	QVariant para1 = param_hash["Param1"];
	bool bPause = para1.toBool();

	m_CoaterDriver->Pause(bPause);
}

void CoaterEventHandler::SwitchVacuum()
{
	const QVariantHash& param_hash = m_curEvent->Params();
	QVariant para1 = param_hash["Param1"];
	bool bOpen = para1.toBool();

	m_CoaterDriver->SwitchVacuum(bOpen);
}
