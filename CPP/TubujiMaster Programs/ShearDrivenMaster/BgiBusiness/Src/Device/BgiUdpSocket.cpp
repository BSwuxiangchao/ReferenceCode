#include "BgiUdpSocket.h"
#include "BgiLogger.h"

BgiUdpSocket::BgiUdpSocket()
	:m_pUdpSocket(NULL)
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

BgiUdpSocket::BgiUdpSocket(QString ip, int port)
	:m_IpAdress(ip)
	,m_iNetPort(port)
	, m_pUdpSocket(NULL)
{
	connect(this, SIGNAL(SignalConnect()), this, SLOT(SlotConnect()));
	connect(this, SIGNAL(WirteBuf(QByteArray)), this, SLOT(SlotWriteData(QByteArray)));

	m_pThread = new QThread();
	//m_pTcpSocket = new QTcpSocket(this);

	this->moveToThread(m_pThread);
	//m_pTcpSocket->moveToThread(m_pThread);

	Open();
}


BgiUdpSocket::~BgiUdpSocket()
{
	Close();
}


void BgiUdpSocket::SlotConnect()
{
	//m_pTcpSocket = new QTcpSocket(this);
	//m_pTcpSocket->moveToThread(m_pThread);

	//bool success = connect(m_pTcpSocket, &QTcpSocket::connected, this, &BgiTcpSocket::SlotConnectSuc);
	//success = connect(m_pTcpSocket, &QTcpSocket::disconnected, this, &BgiTcpSocket::SlotConnectFail);
	//success = connect(m_pTcpSocket, &QTcpSocket::readyRead, this, &BgiTcpSocket::SlotReadData);
	//success = connect(m_pTcpSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(showError(QAbstractSocket::SocketError)));

	//success = m_pTcpSocket->bind(QHostAddress(m_IpAdress));
	//m_pTcpSocket->connectToHost(QHostAddress(m_IpAdress), m_iNetPort);
	//QHostAddress address = m_pTcpSocket->localAddress();
	//int dd = m_pTcpSocket->localPort();
	//m_bTcpConnect = m_pTcpSocket->waitForConnected(3000);

	if (m_pUdpSocket == NULL)
	{
		bool success = false;
		m_pUdpSocket = new QUdpSocket(this);
		m_pUdpSocket->moveToThread(m_pThread);
		success = m_pUdpSocket->bind(QHostAddress::Any);
		//success = m_pUdpSocket->bind(QHostAddress(m_IpAdress), 44818, QAbstractSocket::ShareAddress);
		success = connect(m_pUdpSocket, &QUdpSocket::readyRead, this, &BgiUdpSocket::SlotReadData);
	}
}

void BgiUdpSocket::SetUdpSocketPara(QString ip, int port)
{
	m_IpAdress = ip;
	m_iNetPort = port;

	Open();
}


bool BgiUdpSocket::Open()
{
	//m_pTcpSocket->connectToHost(QHostAddress(m_IpAdress), m_iNetPort);
	//m_bTcpConnect = m_pTcpSocket->waitForConnected(500);
	m_pThread->start(QThread::HighestPriority);
	
	emit SignalConnect();

	return true;
}

bool BgiUdpSocket::Close()
{
	m_pUdpSocket->disconnectFromHost();

	if (m_pThread->isRunning())
	{
		m_pThread->quit();
		m_pThread->wait();
		m_pThread->deleteLater();
	}

	return true;
}

void BgiUdpSocket::SlotConnectSuc()
{
	m_bUdpConnect = true;
	BGI_MESSAGE("Sorting Monitor Connect Sucess");
}

void BgiUdpSocket::SlotConnectFail()
{
	m_bUdpConnect = false;

	BGI_WARNING("Sorting Monitor Connect Fail");
}

void BgiUdpSocket::SlotShowError(QAbstractSocket::SocketError socket_error)
{
	m_bUdpConnect = false;
}

void BgiUdpSocket::Wirte(QByteArray buf)
{
	//存在多个线程同时访问的情况
	m_BgiLock.Acquire();
	emit WirteBuf(buf);
	m_BgiLock.Release();
}

void BgiUdpSocket::SlotWriteData(QByteArray buf)
{
	while (m_pUdpSocket->hasPendingDatagrams())
	{
		m_ReadBuf.resize(m_pUdpSocket->pendingDatagramSize());
		m_pUdpSocket->readDatagram(m_ReadBuf.data(), m_ReadBuf.size());
	}

	//根据液路阀的通讯协议写开/关指令
	//m_pTcpSocket->write(buf);
	m_pUdpSocket->write(buf);

	while (m_pUdpSocket->hasPendingDatagrams())
	{
		m_ReadBuf.resize(m_pUdpSocket->pendingDatagramSize());
		m_pUdpSocket->readDatagram(m_ReadBuf.data(), m_ReadBuf.size());
	}
}

void BgiUdpSocket::SlotReadData()
{
	m_ReadBuf.clear();
	m_ReadBuf = m_pUdpSocket->readAll();

	emit SendBuf(m_ReadBuf);

	while (m_pUdpSocket->hasPendingDatagrams())
	{
		m_ReadBuf.resize(m_pUdpSocket->pendingDatagramSize());
		m_pUdpSocket->readDatagram(m_ReadBuf.data(), m_ReadBuf.size());
	}
}

void BgiUdpSocket::Read(QByteArray& buf)
{
	buf = m_ReadBuf;
}