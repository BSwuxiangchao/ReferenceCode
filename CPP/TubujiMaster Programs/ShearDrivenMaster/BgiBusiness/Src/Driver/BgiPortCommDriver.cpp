#include "BgiPortCommDriver.h"
#include <qthread.h>
#include "BgiLogger.h"
#include "BgiSerialPort.h"

BgiPortCommDriver::BgiPortCommDriver()
{
}

BgiPortCommDriver::~BgiPortCommDriver()
{
}

void BgiPortCommDriver::WaitCommandReplay(int status, int total, int ms)
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


void BgiPortCommDriver::SetSerialPort(BgiSerialPort* port)
{
	BGI_ASSERT(port != NULL);
	m_pPort = port;
}

void BgiPortCommDriver::ConnectPortSlot()
{
	QObject::connect(m_pPort, SIGNAL(SendBuf(QByteArray)), this, SLOT(GetSerialPortBuff(QByteArray)));
}

void BgiPortCommDriver::DisConnectPortSlot()
{
	QObject::disconnect(m_pPort, SIGNAL(SendBuf(QByteArray)), this, SLOT(GetSerialPortBuff(QByteArray)));
	/*delete m_pPort;
	m_pPort = NULL;*/
}

void BgiPortCommDriver::GetSerialPortBuff(QByteArray buf)
{
	ExplainBuff(buf);
}