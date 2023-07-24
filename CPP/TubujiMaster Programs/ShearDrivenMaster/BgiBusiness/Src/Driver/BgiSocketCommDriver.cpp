#include "BgiSocketCommDriver.h"
#include <qthread.h>
#include "BgiLogger.h"
#include "BgiTcpSocket.h"

BgiSocketCommDriver::BgiSocketCommDriver()
{
}

BgiSocketCommDriver::~BgiSocketCommDriver()
{
}

void BgiSocketCommDriver::WaitCommandReplay(int status, int total, int ms)
{
	int count = 0;
	while (count < total)
	{
		QThread::msleep(10);

		m_BgiLock.Acquire();
		if (status == 1)
		{
			m_BgiLock.Release();
			break;
		}
		m_BgiLock.Release();
		
		count++;
	}
}


void BgiSocketCommDriver::SetTcpSocket(BgiTcpSocket* port)
{
	BGI_ASSERT(port != NULL);
	m_pSocket = port;
}

void BgiSocketCommDriver::ConnectSocketSlot()
{
	QObject::connect(m_pSocket, SIGNAL(SendBuf(QByteArray)), this, SLOT(GetSocketBuff(QByteArray)), Qt::DirectConnection);
}

void BgiSocketCommDriver::DisConnectSocketSlot()
{
	QObject::disconnect(m_pSocket, SIGNAL(SendBuf(QByteArray)), this, SLOT(GetSocketBuff(QByteArray)));
}

void BgiSocketCommDriver::GetSocketBuff(QByteArray buf)
{
	ExplainBuff(buf);
}