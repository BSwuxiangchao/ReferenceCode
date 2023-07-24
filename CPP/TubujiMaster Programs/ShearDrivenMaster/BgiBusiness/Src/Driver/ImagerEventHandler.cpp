#include "ImagerEventHandler.h"
#include "MasterDevices.h"
#include "ImagerDriver.h"

ImagerEventHandler::ImagerEventHandler()
{
	m_ImagerDriver = MasterDevices::Instance()->GetImagerDriver();
}

ImagerEventHandler::~ImagerEventHandler()
{}

bool ImagerEventHandler::HandleEvent(BgiMpsEvent* pEvent)
{
	try
	{
		m_curEvent = pEvent;

		QString action = pEvent->Action();

		if (action == "GetConnStatus")
		{
			m_ImagerDriver->GetConnStatus();
		}
		else if (action == "ImagerConfigGet")
		{
			m_ImagerDriver->ImagerConfigGet();
		}
		else if (action == "ImagerConfigSet")
		{
			ImagerConfigSet();
		}
		else if (action == "GoLoadingPos")
		{
			m_ImagerDriver->GoLoadingPos();
		}
		else if (action == "GoHome")
		{
			m_ImagerDriver->GoHome();
		}
		else if (action == "SwitchVacuum")
		{
			SwitchVacuum();
		}
		else if (action == "Reset")
		{
			m_ImagerDriver->Reset();
		}
		else if (action == "GetBasecallMems")
		{
			m_ImagerDriver->GetBasecallMems();
		}
		else if (action == "ReleaseMem")
		{
			ReleaseMem();
		}
		else if (action == "GetVersions")
		{
			m_ImagerDriver->GetVersions();
		}
		else if (action == "GetDiskSpace")
		{
			m_ImagerDriver->GetDiskSpace();
		}

		return true;
	}
	catch (...)
	{
		return false;
	}
}

void ImagerEventHandler::SwitchVacuum()
{
	const QVariantHash& param_hash = m_curEvent->Params();
	QVariant para1 = param_hash["Param1"];
	bool bOpen = para1.toBool();

	m_ImagerDriver->SwitchVacuum(bOpen);
}

void ImagerEventHandler::ImagerConfigSet()
{
	const QVariantHash& param_hash = m_curEvent->Params();
	QVariant para1 = param_hash["Param1"];
	ImagerParaType::ImagerConfigPara data = para1.value<ImagerParaType::ImagerConfigPara>();

	m_ImagerDriver->ImagerConfigSet(data);
}

void ImagerEventHandler::ReleaseMem()
{
	const QVariantHash& param_hash = m_curEvent->Params();
	QVariant para1 = param_hash["Param1"];
	QString slideId = para1.toString();

	m_ImagerDriver->ReleaseRun(slideId);
}