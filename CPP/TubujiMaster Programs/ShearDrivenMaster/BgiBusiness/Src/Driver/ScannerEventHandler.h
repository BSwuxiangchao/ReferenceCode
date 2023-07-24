#pragma once
/*
* @Abrief MCR扫码器处理器
* @note 处理扫码器扫码事件
* @version 1.0
* @author wuxiangchao 2023-5-29
*/

#include <qobject.h>
#include "DriveEventHandler.h"

class ScannerDriver;
class ScannerEventHandler : public DriveEventHandler
{
	Q_OBJECT
public:
	ScannerEventHandler();
	virtual ~ScannerEventHandler();

public:
	virtual bool HandleEvent(BgiMpsEvent* pEvent);

private:

	BgiMpsEvent* m_curEvent;


	ScannerDriver* m_pScannerDriver;

};

