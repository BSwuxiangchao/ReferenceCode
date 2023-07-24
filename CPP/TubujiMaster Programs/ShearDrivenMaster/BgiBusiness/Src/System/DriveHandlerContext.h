/*
* @Abrief 设备处理器的集合
* @note 设备处理器的实例化
* @version 1.0
* @author wuxiangchao 2023-02-20
*/

#ifndef _DRIVE_HANDLER_CONTEXT_H
#define _DRIVE_HANDLER_CONTEXT_H

#include "BgiLock.h"
#include <qobject.h>
#include "BgiMpsEvent.h"
#include <qthread.h>
#include <qvector.h>
#include <qsharedpointer.h>
#include "BusinessExternDef.h"

class RobotParaHandler;
class EpsonRobotEventHandler;
class CoaterEventHandler;
class ImagerEventHandler;
class SchedulerEventHandler;
class ScannerEventHandler;

class BGIBUSINE_CLASS_EXPORT DriveHandlerContext
{
public:
	DriveHandlerContext();
	~DriveHandlerContext();

public:
	bool HandleEvent(BgiMpsEvent* pEvent);

	RobotParaHandler* m_pRobotParaHandler;
	EpsonRobotEventHandler* m_pEpsonRobotEventHandler;

	CoaterEventHandler* m_pCoaterEventHandler;
	ImagerEventHandler* m_pImagerEventHandler;

	SchedulerEventHandler* m_pSchedulerEventHandler;

	ScannerEventHandler* m_pScannerEventHandler;
};

#endif
