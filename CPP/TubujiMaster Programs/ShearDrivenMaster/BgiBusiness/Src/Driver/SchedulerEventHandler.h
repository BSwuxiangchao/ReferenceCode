#pragma once

#include <QObject>
#include "DriveEventHandler.h"

class SchedulerDriver;

class SchedulerEventHandler  : public DriveEventHandler
{
	Q_OBJECT

public:
	SchedulerEventHandler();
	~SchedulerEventHandler();

public:
	virtual bool HandleEvent(BgiMpsEvent* pEvent);

private:
	void StartRun();
	void SetEndCycle();

private:
	BgiMpsEvent* m_curEvent;
	SchedulerDriver* m_SchedulerDriver;
};
