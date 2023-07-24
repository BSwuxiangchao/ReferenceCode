#include "ElectricRelaySerialPort.h"

ElectricRelaySerialPort::ElectricRelaySerialPort()
	:BgiSerialPort()
{
}

ElectricRelaySerialPort::ElectricRelaySerialPort(QString strPortName,
	QSerialPort::BaudRate portBaudRate,
	QSerialPort::DataBits portDataBits,
	QSerialPort::FlowControl portFlowControl,
	QSerialPort::Parity portParity,
	QSerialPort::StopBits portStopBits)
	: BgiSerialPort(strPortName, portBaudRate, portDataBits, portFlowControl, portParity, portStopBits)
{
}


ElectricRelaySerialPort::~ElectricRelaySerialPort()
{
}

void ElectricRelaySerialPort::ReadBuf()
{
	m_BgiLock.Acquire();
	
	QByteArray temp = m_pSerialPort->readAll();

	if (temp.count() < 1)
	{
		m_BgiLock.Release();
		return;
	}
		

	if (temp.at(0) == (unsigned char)0x22)
	{
		m_ReadBuf.clear();
	}

	m_ReadBuf.append(temp);

	if (m_ReadBuf.count() == 8)
	{
		if (m_ReadBuf[0] == (unsigned char)0x22)
		{
			m_ReadBufMap[m_ReadBuf[7]] = m_ReadBuf;
		}
	}
	
	m_BgiLock.Release();
}


void ElectricRelaySerialPort::GetBuf(QByteArray& buf)
{
	m_BgiLock.Acquire();
	
	if (m_ReadBuf.count() == 8)
	{
		buf = m_ReadBuf;
		m_ReadBuf.clear();
	}

	m_BgiLock.Release();
}

bool ElectricRelaySerialPort::GetBuf(int addr, QByteArray& buf)
{
	m_BgiLock.Acquire();

	bool bSuc = false;
	QMap<int, QByteArray>::iterator iter = m_ReadBufMap.find(addr);

	if (iter != m_ReadBufMap.end())
	{
		if (iter.value().count() == 8)
		{
			buf = iter.value();
			iter.value().clear();
			bSuc = true;
		}
	}

	m_BgiLock.Release();

	return bSuc;
}