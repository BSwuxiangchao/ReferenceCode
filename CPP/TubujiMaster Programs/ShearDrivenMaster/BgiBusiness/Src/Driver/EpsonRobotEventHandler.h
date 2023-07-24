/*
* @Abrief EPson机械手处理器
* @note EPson机械手处理器
* @version 1.0
* @author wuxiangchao 2021-02-27
*/

#ifndef _EPSON_ROBOT_EVENT_HANDLER_H
#define _EPSON_ROBOT_EVENT_HANDLER_H

#include "BgiLock.h"
#include <qobject.h>
#include "BgiMpsEvent.h"
#include <qthread.h>
#include <qvector.h>
#include "DriveEventHandler.h"
#include <qsharedpointer.h>
#include "BgiProcId.h"
class EpsonRobotDriver;
class EpsonRobotEventHandler : public DriveEventHandler
{
	Q_OBJECT
public:
	EpsonRobotEventHandler();
	virtual ~EpsonRobotEventHandler();

public:
	virtual bool HandleEvent(BgiMpsEvent* pEvent);

private:
	void SetPower();
	void SetPTPSpeed();
	void SetPTPAccel();
	void SetCPSpeed();
	void SetCPAccel();
	void SetRSpeed();
	void SetRAccel();
	void TransferPos();
	void OptChips();
	void MotorDoCommand();
	void MotorGoVar();
	void MotorStep();
	void OptRobotSocket();
	void TurnOnOutIO();
	void TurnOffOutIO();

private:
	
	BgiMpsEvent* m_curEvent;


	EpsonRobotDriver* m_EpsonRobotDriver;
	bool m_bOptRobotSocket;
};

#endif
