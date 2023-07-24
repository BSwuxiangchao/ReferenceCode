#pragma once

#include <QObject>
#include "DriveEventHandler.h"

class CoaterDriver;

class CoaterEventHandler  : public DriveEventHandler
{
	Q_OBJECT

public:
	CoaterEventHandler();
	~CoaterEventHandler();

public:
	virtual bool HandleEvent(BgiMpsEvent* pEvent);

private:
	void ReadCmdData();
	void HandlePause();
	void SwitchVacuum();

private:
	BgiMpsEvent* m_curEvent;
	CoaterDriver* m_CoaterDriver;
};
