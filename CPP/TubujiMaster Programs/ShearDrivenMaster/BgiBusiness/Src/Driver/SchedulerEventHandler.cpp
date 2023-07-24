#include "SchedulerEventHandler.h"
#include "MasterDevices.h"
#include "SchedulerDriver.h"

SchedulerEventHandler::SchedulerEventHandler()
{
	m_SchedulerDriver = MasterDevices::Instance()->GetSchedulerDriver();
}

SchedulerEventHandler::~SchedulerEventHandler()
{}

bool SchedulerEventHandler::HandleEvent(BgiMpsEvent* pEvent)
{
	try
	{
		m_curEvent = pEvent;

		QString action = pEvent->Action();

		if (action == "StartRun")
		{
			StartRun();
		}
		else if (action == "PauseRun")
		{
			m_SchedulerDriver->PauseRun();
		}
		else if (action == "StopRun")
		{
			m_SchedulerDriver->StopRun();
		}
		else if (action == "SetEndCycle")
		{
			SetEndCycle();
		}

		return true;
	}
	catch (...)
	{
		return false;
	}
}

void SchedulerEventHandler::StartRun()
{
	const QVariantHash& param_hash = m_curEvent->Params();
	QVariant para1 = param_hash["Param1"];
	QString barcode = para1.toString();

	m_SchedulerDriver->StartRun(barcode);
}

void SchedulerEventHandler::SetEndCycle()
{
	const QVariantHash& param_hash = m_curEvent->Params();
	QVariant para1 = param_hash["Param1"];
	int cycle = para1.toInt();

	m_SchedulerDriver->SetEndCycle(cycle);
}
