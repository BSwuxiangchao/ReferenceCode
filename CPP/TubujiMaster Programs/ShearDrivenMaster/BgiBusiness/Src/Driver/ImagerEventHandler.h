#pragma once

#include <QObject>
#include "DriveEventHandler.h"

class ImagerDriver;

class ImagerEventHandler  : public DriveEventHandler
{
	Q_OBJECT

public:
	ImagerEventHandler();
	~ImagerEventHandler();

public:
	virtual bool HandleEvent(BgiMpsEvent* pEvent);

private:
	void SwitchVacuum();
	void ImagerConfigSet();
	void ReleaseMem();

private:
	BgiMpsEvent* m_curEvent;
	ImagerDriver* m_ImagerDriver;
};
