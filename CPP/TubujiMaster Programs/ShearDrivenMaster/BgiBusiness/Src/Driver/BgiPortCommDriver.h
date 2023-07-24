/*
* @Abrief 设备动作的执行细节
* @note 设备动作的执行细节基类，包括轴方向移动，复位，取管，液面探测，吸液，滴液，弃管，孵育，清洗等
* @version 1.0
* @author mosongjin 2021-09-02
*/

#ifndef _BGI_PORT_COMM_DRIVER_H
#define _BGI_PORT_COMM_DRIVER_H

#include "BgiLock.h"
#include <qobject.h>
#include "ConstDef.h"

class BgiSerialPort;

class BgiPortCommDriver : public QObject
{
	Q_OBJECT
public:
	BgiPortCommDriver();
	virtual ~BgiPortCommDriver();	

protected:
	//等待指令接收后的应答
	//status:应答条件，只有当status==1时，接收到正确的应答
	//total：等待计数
	//ms:等待计数的间隔(ms)
	void WaitCommandReplay(int status, int total, int ms);

	void SetSerialPort(BgiSerialPort* port);
	void ConnectPortSlot();
	void DisConnectPortSlot();

	virtual void ExplainBuff(QByteArray buf) = 0;

private slots:
	void GetSerialPortBuff(QByteArray); //串口传输出来的状态信息

protected:
	BgiLock m_BgiLock;

private:
	BgiSerialPort* m_pPort;

};

#endif
