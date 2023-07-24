#include "BgiTcpSocket.h"
#include "BgiLogger.h"

BgiTcpSocket::BgiTcpSocket()
	:m_pTcpSocket(NULL)
	, m_bTcpConnect(false)
	, m_iConnectFailCount(0)
{
	connect(this, SIGNAL(SignalConnect()), this, SLOT(SlotConnect()));
	connect(this, SIGNAL(WirteBuf(QByteArray)), this, SLOT(SlotWriteData(QByteArray)));

	m_pThread = new QThread();
	//m_pTcpSocket = new QTcpSocket(this);

	//connect(m_pTcpSocket, SIGNAL(connected()), this, SLOT(SlotConnectSuc()));
	//connect(m_pTcpSocket, SIGNAL(disConnected()), this, SLOT(SlotConnectFail()));
	//connect(m_pTcpSocket, SIGNAL(readyRead()), this, SLOT(SlotReadData()));
	//connect(m_pTcpSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(showError(QAbstractSocket::SocketError)));

	//bool success = connect(m_pTcpSocket, &QTcpSocket::connected, this, &BgiTcpSocket::SlotConnectSuc);
	//success = connect(m_pTcpSocket, &QTcpSocket::disconnected, this, &BgiTcpSocket::SlotConnectFail);
	//success = connect(m_pTcpSocket, &QTcpSocket::readyRead, this, &BgiTcpSocket::SlotReadData);

	//success = connect(m_pTcpSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(showError(QAbstractSocket::SocketError)));

	this->moveToThread(m_pThread);
	//m_pTcpSocket->moveToThread(m_pThread);
}

BgiTcpSocket::BgiTcpSocket(QString ip, int port)
	:m_IpAdress(ip)
	, m_iNetPort(port)
	, m_pTcpSocket(NULL)
	, m_bTcpConnect(false)
{
	connect(this, SIGNAL(SignalConnect()), this, SLOT(SlotConnect()));
	connect(this, SIGNAL(WirteBuf(QByteArray)), this, SLOT(SlotWriteData(QByteArray)));

	m_pThread = new QThread();
	//m_pTcpSocket = new QTcpSocket(this);

	this->moveToThread(m_pThread);
	//m_pTcpSocket->moveToThread(m_pThread);

	Open();
}


BgiTcpSocket::~BgiTcpSocket()
{
	Close();
}


void BgiTcpSocket::SlotConnect()
{
	if (m_pTcpSocket == NULL)
	{
		m_pTcpSocket = new QTcpSocket(this);
		m_pTcpSocket->moveToThread(m_pThread);

		bool success = connect(m_pTcpSocket, &QTcpSocket::connected, this, &BgiTcpSocket::SlotConnectSuc);
		success = connect(m_pTcpSocket, &QTcpSocket::disconnected, this, &BgiTcpSocket::SlotConnectFail);
		success = connect(m_pTcpSocket, &QTcpSocket::readyRead, this, &BgiTcpSocket::SlotReadData);
		success = connect(m_pTcpSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(showError(QAbstractSocket::SocketError)));
	}

	if (m_pTcpSocket != NULL)
	{
		m_pTcpSocket->connectToHost(QHostAddress(m_IpAdress), m_iNetPort);
		m_bTcpConnect = m_pTcpSocket->waitForConnected(500);
	}
}

void BgiTcpSocket::SetTcpSocketPara(QString ip, int port)
{
	m_IpAdress = ip;
	m_iNetPort = port;

	Open();
}


bool BgiTcpSocket::Open()
{
	//m_pTcpSocket->connectToHost(QHostAddress(m_IpAdress), m_iNetPort);
	//m_bTcpConnect = m_pTcpSocket->waitForConnected(500);
	m_pThread->start(QThread::HighestPriority);

	emit SignalConnect();

	return true;
}

bool BgiTcpSocket::Close()
{
	if (m_pThread->isRunning())
	{
		m_pThread->quit();
		m_pThread->wait();
		m_pThread->deleteLater();
	}
	if (m_pTcpSocket != NULL)
	{
		m_pTcpSocket->disconnectFromHost();
		m_pTcpSocket->deleteLater();
		m_pTcpSocket = NULL;
	}
	
	return true;
}

void BgiTcpSocket::SlotConnectSuc()
{
	m_bTcpConnect = true;
	QString strInfo = QString("%1 connect Sucess By TcpSocket").arg(m_IpAdress);
	BGI_MESSAGE(strInfo.toStdString().c_str());
	m_iConnectFailCount = 0;
	emit SignalConnectStatus(true);
}

void BgiTcpSocket::SlotConnectFail()
{
	m_bTcpConnect = false;
	emit SignalConnectStatus(false);

	BGI_WARNING("Device Connect Fail By TcpSocket");
}

void BgiTcpSocket::SlotShowError(QAbstractSocket::SocketError /*socket_error*/)
{
	m_bTcpConnect = false;
	emit SignalConnectStatus(false);
}

void BgiTcpSocket::SlotMasterCloseRobotConnect()
{
	emit SignalConnectStatus(false);
	m_pTcpSocket->disconnectFromHost();
	m_pTcpSocket->deleteLater();
	m_pTcpSocket = NULL;
	if (m_pThread->isRunning())
	{
		m_pThread->quit();
		m_pThread->wait();
		//m_pThread->deleteLater();

	}
}

void BgiTcpSocket::Write(QByteArray buf)
{
	//存在多个线程同时访问的情况
	m_BgiLock.Acquire();
	emit WirteBuf(buf);
	m_BgiLock.Release();
}

void BgiTcpSocket::DelayWrite(QByteArray buf)
{
	//存在多个线程同时访问的情况
	m_BgiLock.Acquire();
	QThread::msleep(10);
	emit WirteBuf(buf);
	m_BgiLock.Release();
}

void BgiTcpSocket::SlotWriteData(QByteArray buf)
{
	if (!m_bTcpConnect)
	{
		emit SignalConnect();

		/*m_pTcpSocket->connectToHost(QHostAddress(m_IpAdress), m_iNetPort);
		m_bTcpConnect = m_pTcpSocket->waitForConnected(500);*/
	}

	if (m_bTcpConnect)
	{
		//发送指令
		m_pTcpSocket->write(buf);
	}
	else
	{
		//提示连接失败
		if (m_iConnectFailCount % 8 == 0)
		{
			QString strInfo = m_IpAdress;
			strInfo += " connect failed";
			BGI_WARNING(strInfo.toStdString().c_str());
			emit SignalConnectStatus(false);
			m_iConnectFailCount = 0;
		}
		m_iConnectFailCount++;
	}
}

void BgiTcpSocket::SlotReadData()
{
	m_ReadBuf.clear();
	m_ReadBuf = m_pTcpSocket->readAll();

	emit SendBuf(m_ReadBuf);
}

void BgiTcpSocket::Read(QByteArray& buf)
{
	buf = m_ReadBuf;
}

void BgiTcpSocket::CleanBuf()
{
	m_BgiLock.Acquire();
	m_ReadBuf.clear();
	m_BgiLock.Release();
}