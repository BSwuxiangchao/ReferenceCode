#pragma once
/*
* @Abrief MCR扫码器
* @note 操作扫码器扫码
* @version 1.0
* @author wuxiangchao 2023-5-29
*/
#include <QObject>
#include <QTimer>
#include "BgiSocketCommDriver.h"
#include "BgiThread.h"
#include "RobotParaType.h"
#include "BusinessExternDef.h"
class BgiTcpSocket;

class BGIBUSINE_CLASS_EXPORT ScannerDriver : public BgiSocketCommDriver
{
	Q_OBJECT
public:
	ScannerDriver(BgiTcpSocket* socket);
	~ScannerDriver();

	// 返回扫码器网络连接状态
	bool GetConnectStatus();

	// 发送扫码触发命令开始扫码
	bool StartReadCode();
public slots:
	void SlotConnectStatus(bool bSuccess);
	void SlotSendBeat();
private:
	void ExplainBuff(QByteArray); //串口传输出来的状态信息
	void SendCommand(QByteArray buf);

private:
	bool m_bRobotConnect;
	BgiTcpSocket* m_pSocket;
	QTimer* m_pTimer;

};

