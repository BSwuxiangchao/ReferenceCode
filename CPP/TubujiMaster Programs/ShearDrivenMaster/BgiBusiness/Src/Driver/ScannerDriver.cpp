#include "ScannerDriver.h"
#include "BgiTcpSocket.h"
#include "RobotOutputParaUtil.h"

ScannerDriver::ScannerDriver(BgiTcpSocket* socket)
	:m_bRobotConnect(false)
	, m_pSocket(socket)
{
	SetTcpSocket(socket);

	ConnectSocketSlot();
	m_pTimer = new QTimer(this);
	connect(m_pTimer, SIGNAL(timeout()), this, SLOT(SlotSendBeat()));
	m_pTimer->start(1000);
}

ScannerDriver::~ScannerDriver()
{
	DisConnectSocketSlot();
}

// 返回扫码器网络连接状态
bool ScannerDriver::GetConnectStatus()
{
	return m_bRobotConnect;
}

void ScannerDriver::SlotConnectStatus(bool bSuccess)
{
	m_bRobotConnect = bSuccess;
}

void ScannerDriver::SlotSendBeat()
{
	if (m_bRobotConnect)
		return;
	QByteArray cmd("670D");
	SendCommand(cmd);
}

//串口传输出来的状态信息
void ScannerDriver::ExplainBuff(QByteArray buf)
{
	QString strBuf = buf;
	RobotOutputParaUtil::SetScannerInfo(strBuf);
}

void ScannerDriver::SendCommand(QByteArray buf)
{
	m_pSocket->Write(QByteArray::fromHex(buf));
}

bool ScannerDriver::StartReadCode()
{
	QByteArray cmd("670D");
	SendCommand(cmd);
	return true;
}