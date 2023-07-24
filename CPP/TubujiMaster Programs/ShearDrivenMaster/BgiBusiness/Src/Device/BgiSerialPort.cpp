#include "BgiSerialPort.h"
#include "BgiLogger.h"

BgiSerialPort::BgiSerialPort()
{
	m_pThread = new QThread();
	m_pSerialPort = new QSerialPort();
	connect(m_pSerialPort, SIGNAL(readyRead()), this, SLOT(ReadData()));
	connect(this, SIGNAL(WirteBuf(QByteArray)), this, SLOT(WriteData(QByteArray)));

	this->moveToThread(m_pThread);
	m_pSerialPort->moveToThread(m_pThread);
}

BgiSerialPort::BgiSerialPort(QString strPortName,
	QSerialPort::BaudRate portBaudRate,
	QSerialPort::DataBits portDataBits,
	QSerialPort::FlowControl portFlowControl,
	QSerialPort::Parity portParity,
	QSerialPort::StopBits portStopBits)
	:m_strPortName(strPortName)
	, m_portBaudRate(portBaudRate)
	, m_portDataBits(portDataBits)
	, m_portFlowControl(portFlowControl)
	, m_portParity(portParity)
	, m_portStopBits(portStopBits)
{
	m_pThread = new QThread();
	m_pSerialPort = new QSerialPort();
	bool bsuc = connect(m_pSerialPort, SIGNAL(readyRead()), this, SLOT(ReadData()));
	bsuc = connect(this, SIGNAL(WirteBuf(QByteArray)), this, SLOT(WriteData(QByteArray)));

	this->moveToThread(m_pThread);
	m_pSerialPort->moveToThread(m_pThread);

	//Open();
}


BgiSerialPort::~BgiSerialPort()
{
	Close();
}


void BgiSerialPort::SetSerialPortPara(QString strPortName,
	QSerialPort::BaudRate portBaudRate,
	QSerialPort::DataBits portDataBits,
	QSerialPort::FlowControl portFlowControl,
	QSerialPort::Parity portParity,
	QSerialPort::StopBits portStopBits)
{
	m_strPortName = strPortName;
	m_portBaudRate = portBaudRate;
	m_portDataBits = portDataBits;
	m_portFlowControl = portFlowControl;
	m_portParity =portParity;
	m_portStopBits = portStopBits;

	Open();
}


bool BgiSerialPort::Open()
{
	m_pSerialPort->setPortName(m_strPortName);

	if (!m_pSerialPort->open(QIODevice::ReadWrite))
	{
		QString strInfo = m_strPortName + " BgiSerialPort open failed";
		BGI_WARNING(strInfo.toStdString().c_str());
		RecordToLogFile(QString("串口%1连接失败").arg(m_strPortName));
		return false;
	}

	m_pSerialPort->setBaudRate(m_portBaudRate);
	m_pSerialPort->setDataBits(m_portDataBits);
	m_pSerialPort->setFlowControl(m_portFlowControl);
	m_pSerialPort->setParity(m_portParity);
	m_pSerialPort->setStopBits(m_portStopBits);

	m_pThread->start(QThread::HighestPriority);
	QString strInfo = m_strPortName + " BgiSerialPort open Success";
	RecordToLogFile(QString("串口%1连接成功").arg(m_strPortName),gc_colorGreen);
	BGI_WARNING(strInfo.toStdString().c_str());
	return true;
}

bool BgiSerialPort::Close()
{
	if (m_pSerialPort->isOpen())
	{
		m_pSerialPort->close();
		m_pSerialPort->deleteLater();
		RecordToLogFile(QString("串口%1断开连接").arg(m_strPortName), gc_colorRed);
	}

	if (m_pThread->isRunning())
	{
		m_pThread->quit();
		m_pThread->wait();
		m_pThread->deleteLater();
	}
	return true;
}

void BgiSerialPort::RecordToLogFile(QString strContext, QColor color)
{
	RecordData data;
	data.m_strModelName = "调试工具";
	data.m_strInfoType = "串口连接";
	data.m_strContent = strContext;
	data.m_color = color;
	OperateRecordInfo::AddRecordData(data);
}

void BgiSerialPort::Wirte(QByteArray buf)
{
	//存在多个线程同时访问的情况
	m_BgiLock.Acquire();
	emit WirteBuf(buf);
	m_ReadBuf.clear();
	m_BgiLock.Release();
}

void BgiSerialPort::DelayWirte(QByteArray buf, int mSec)
{
	//存在多个线程同时访问的情况
	m_BgiLock.Acquire();
	emit WirteBuf(buf);
	QThread::msleep(mSec);
	m_ReadBuf.clear();
	m_BgiLock.Release();
}

void BgiSerialPort::WirteEx(QByteArray buf)
{
	//存在多个线程同时访问的情况
	m_BgiLock.Acquire();
	emit WirteBuf(buf);
	m_BgiLock.Release();
}

void BgiSerialPort::WriteData(QByteArray buf)
{
	//根据液路阀的通讯协议写开/关指令
	QString str = QString("WriteData buf:");
	for (int i = 0; i < buf.count(); i++)
	{
		QString str1 = QString("%1").arg(buf[i] & 0xFF, 2, 16, QLatin1Char('0')).toUpper() + QString(" ");   //2 字符宽度
		str += str1;
	}
	BGI_MESSAGE(str.toStdString().c_str());
	m_pSerialPort->write(buf);
}

void BgiSerialPort::ReadBuf()
{
	m_BgiLock.Acquire();


	m_ReadBuf.clear();
	m_ReadBuf = m_pSerialPort->readAll();

	//QString str = QString("ReadData buf:");
	//for (int i = 0; i < m_ReadBuf.count(); i++)
	//{
	//	QString str1 = QString("%1").arg(m_ReadBuf[i] & 0xFF, 2, 16, QLatin1Char('0')).toUpper() + QString(" ");   //2 字符宽度
	//	str += str1;
	//}
	//BGI_MESSAGE(str.toStdString().c_str());

	if (m_ReadBuf.count() >= 5)
	{
		emit SendBuf(m_ReadBuf);
	}
	else
	{
		QString str = QString("ReadData buf:");
		for (int i = 0; i < m_ReadBuf.count(); i++)
		{
			QString str1 = QString("%1").arg(m_ReadBuf[i] & 0xFF, 2, 16, QLatin1Char('0')).toUpper() + QString(" ");   //2 字符宽度
			str += str1;
		}
		BGI_WARNING(str.toStdString().c_str());
	}



	m_BgiLock.Release();
}

void BgiSerialPort::ReadData()
{
	ReadBuf();
}

void BgiSerialPort::GetBuf(QByteArray& buf)
{
	m_BgiLock.Acquire();
	buf = m_ReadBuf;

	m_BgiLock.Release();
}