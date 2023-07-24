#include "ElectricRelayComm.h"
#include "CRC.h"
#include "BgiLogger.h"
#include "ConstDef.h"
#include "PortConfigDoc.h"

ElectricRelayComm::ElectricRelayComm()
	:m_bDirectRead(true)
{
	QString strPort;
	PortConfigDoc::Instance()->GetPortValue("ElectricRelay", strPort);
	m_pSerialPort = new ElectricRelaySerialPort(strPort, QSerialPort::Baud9600, QSerialPort::Data8,
		QSerialPort::NoFlowControl, QSerialPort::NoParity, QSerialPort::OneStop);
}

ElectricRelayComm::~ElectricRelayComm()
{
	if (m_pSerialPort != NULL)
	{
		delete m_pSerialPort;
		m_pSerialPort = NULL;
	}
}

bool ElectricRelayComm::CheckId(QByteArray arry)
{
	BGI_ASSERT(arry.size() == 8);

	if (0x22 == arry[0] && m_cardId == arry[1] && m_portId == arry[6])
	{
		return true;
	}

	return false;
	
}

int ElectricRelayComm::SwitchFluidPort(int card_id, int port_id, bool bOpen)
{
	m_cardId = card_id;
	m_portId = port_id;
	QByteArray command;
	command.resize(8);
	command[0] = 0x33;
	command[1] = m_cardId;
	if(bOpen)
		command[2] = 0x12;
	else
		command[2] = 0x11;
	command[3] = 0;
	command[4] = 0;
	command[5] = 0;
	command[6] = port_id;
	
	command[7] = 0;
	for (int j = 0; j < 7; j++)
	{
		command[7] += command[j];
	}
	
	m_pSerialPort->Wirte(command);

	if (!m_bDirectRead)
	{
		return 1;
	}

	int count = 0;
	while (count < WAIT_REPLY_SUM * 10)
	{
		QThread::msleep(WAIT_REPLY_SLEEP * 10);

		QByteArray buf;
		m_pSerialPort->GetBuf(buf);
		if (buf.size() == 8)
		{
			int status = -1;
			TryPraseSetElectricIO(buf, status);
			if (status == 0)
			{
				return 0; //成功
			}
		}
		count++;
	}

	BGI_WARNING("ElectricRelayComm::SwitchFluidPort open failed");

	return 1;
}
void ElectricRelayComm::TryPraseSetElectricIO(QByteArray buf, int& status)
{
	BGI_ASSERT(buf.size() == 8);

	if (0x22 == buf[0] && m_cardId == buf[1] && m_portId == buf[6])
	{
		int check = 0;
		for (int i = 0; i < 7; i++)
			check += buf[i];

		if ((check & 0xff) == (unsigned char)buf[7])
		{
			status = 0;
			return;
		}
			
	}

	status = 1;
}

void ElectricRelayComm::SetTakingBuffWay(bool bDirectRead)
{
	m_bDirectRead = bDirectRead;
}

int ElectricRelayComm::ReadPortValue(int /*card_id*/, int /*port_id*/, unsigned short& /*value*/)
{
	return 0;
}

short ElectricRelayComm::GetElecPressure(int /*card_id*/, int /*channel*/, double* /*elev_value*/)
{
	return 0;
}