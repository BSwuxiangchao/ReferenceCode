/*
* @Abrief 调试事件处理器
* @note 液路阀、转运设备等需要工程师调试的设备事件处理器
* @version 1.0
* @author mosongjin 2021-10-21
*/

#ifndef _DRIVE_EVENT_HANDLER_H
#define _DRIVE_EVENT_HANDLER_H

#include "BgiLock.h"
#include <qobject.h>
#include "BgiMpsEvent.h"

class DriveEventHandler : public QObject
{
	Q_OBJECT
public:
	DriveEventHandler();
	virtual ~DriveEventHandler();

public:
	virtual bool HandleEvent(BgiMpsEvent* pEvent) = 0;

};

#endif
