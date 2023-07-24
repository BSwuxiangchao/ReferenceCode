/*
* @Abrief 设备动作的执行细节
* @note 以太网通讯的具体执行动作
* @version 1.0
* @author mosongjin 2021-11-19
*/

#ifndef _BGI_SOCKET_COMM_DRIVER_H
#define _BGI_SOCKET_COMM_DRIVER_H

#include "BgiLock.h"
#include <qobject.h>
#include "ConstDef.h"



class BgiTcpSocket;

class BgiSocketCommDriver : public QObject
{
	Q_OBJECT
public:
	BgiSocketCommDriver();
	virtual ~BgiSocketCommDriver();

protected:
	//等待指令接收后的应答
	//status:应答条件，只有当status==1时，接收到正确的应答
	//total：等待计数
	//ms:等待计数的间隔(ms)
	void WaitCommandReplay(int status, int total, int ms);

	void SetTcpSocket(BgiTcpSocket* port);
	void ConnectSocketSlot();
	void DisConnectSocketSlot();

	virtual void ExplainBuff(QByteArray buf) = 0;

private slots:
	void GetSocketBuff(QByteArray); //串口传输出来的状态信息

protected:
	BgiLock m_BgiLock;

private:
	BgiTcpSocket* m_pSocket;

};

#endif
