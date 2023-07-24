#include "MasterDevices.h"
#include "BgiTcpSocket.h"
#include "EpsonRobotDriver.h"
#include "PortConfigDoc.h"
#include "CoaterDriver.h"
#include "ImagerDriver.h"
#include "SchedulerDriver.h"
#include "ScannerDriver.h"
#include "BgiLogger.h"

MasterDevices::MasterDevices()
{

}

MasterDevices::~MasterDevices()
{

}

MasterDevices* MasterDevices::Instance()
{
	static MasterDevices instance;
	return &instance;
}

BgiTcpSocket* MasterDevices::GetRobotSocket()
{
	if (m_spRobotSocket.isNull())
	{
		BGI_MESSAGE("new init RobotSocket begin ...");
		QString strIpValue = "";
		QString strPortValue = "";
		PortConfigDoc::Instance()->GetPortValue("RobotIP", strIpValue);
		PortConfigDoc::Instance()->GetPortValue("RobotPort", strPortValue);
		
		m_spRobotSocket = QSharedPointer<BgiTcpSocket>(new BgiTcpSocket(strIpValue, strPortValue.toInt()));
		BGI_MESSAGE("new init RobotSocket end");
	}
	return m_spRobotSocket.data();
}

EpsonRobotDriver* MasterDevices::GetEpsonRobotDriver()
{
	if (m_spEpsonRobotDriver.isNull())
	{
		BGI_MESSAGE("new init EpsonRobotDriver begin ...");
		m_spEpsonRobotDriver = QSharedPointer<EpsonRobotDriver>(new EpsonRobotDriver(GetRobotSocket()));
		QObject::connect(m_spRobotSocket.data(), &BgiTcpSocket::SignalConnectStatus, m_spEpsonRobotDriver.data(), &EpsonRobotDriver::SlotConnectStatus);
		QObject::connect(m_spEpsonRobotDriver.data(), &EpsonRobotDriver::SignalMasterCloseRobotConnect, m_spRobotSocket.data(), &BgiTcpSocket::SlotMasterCloseRobotConnect);
		BGI_MESSAGE("new init EpsonRobotDriver end");
	}
	return m_spEpsonRobotDriver.data();
}

BgiTcpSocket* MasterDevices::GetCoaterSocket()
{
	if (m_spCoaterSocket.isNull())
	{
		BGI_MESSAGE("new init CoaterSocket begin ...");
		QString ip = "";
		QString port = "";
		PortConfigDoc::Instance()->GetPortValue("CoaterIP", ip);
		PortConfigDoc::Instance()->GetPortValue("CoaterPort", port);

		m_spCoaterSocket = QSharedPointer<BgiTcpSocket>(new BgiTcpSocket(ip, port.toInt()));
		BGI_MESSAGE("new init CoaterSocket end");
	}
	return m_spCoaterSocket.data();
}

CoaterDriver* MasterDevices::GetCoaterDriver()
{
	if (m_spCoaterDriver.isNull())
	{
		BGI_MESSAGE("new init CoaterDriver begin ...");
		m_spCoaterDriver = QSharedPointer<CoaterDriver>(new CoaterDriver(GetCoaterSocket()));
		connect(m_spCoaterSocket.data(), &BgiTcpSocket::SignalConnectStatus, m_spCoaterDriver.data(), &CoaterDriver::SlotConnectStatus, Qt::DirectConnection);
		BGI_MESSAGE("new init CoaterDriver end");
	}
	return m_spCoaterDriver.data();
}

ImagerDriver* MasterDevices::GetImagerDriver()
{
	if (m_spImagerDriver.isNull())
	{
		BGI_MESSAGE("new init ImagerDriver begin ...");
		m_spImagerDriver = QSharedPointer<ImagerDriver>(new ImagerDriver(this));
		BGI_MESSAGE("new init ImagerDriver end");
	}
	return m_spImagerDriver.data();
}

SchedulerDriver* MasterDevices::GetSchedulerDriver()
{
	if (m_spSchedulerDriver.isNull())
	{
		//加锁保护，防止重入、重复初始化
		m_InitLock.Acquire();
		if (m_spSchedulerDriver.isNull())
		{
			BGI_MESSAGE("new init SchedulerDriver begin ...");
			m_spSchedulerDriver = QSharedPointer<SchedulerDriver>(new SchedulerDriver(this));
			BGI_MESSAGE("new init SchedulerDriver end");
		}
		m_InitLock.Release();
	}
	return m_spSchedulerDriver.data();
}

ScannerDriver* MasterDevices::GetScannerDriver()
{
	if (m_spScannerDriver.isNull())
	{
		BGI_MESSAGE("new init ScannerDriver begin ...");
		QString ip = "";
		QString port = "";
		PortConfigDoc::Instance()->GetPortValue("ScannerIP", ip);
		PortConfigDoc::Instance()->GetPortValue("ScannerPort", port);
		if(m_spScannerSocket.isNull())
			m_spScannerSocket = QSharedPointer<BgiTcpSocket>(new BgiTcpSocket(ip, port.toInt()));
		m_spScannerDriver = QSharedPointer<ScannerDriver>(new ScannerDriver(m_spScannerSocket.data()));
		QObject::connect(m_spScannerSocket.data(), &BgiTcpSocket::SignalConnectStatus, m_spScannerDriver.data(), &ScannerDriver::SlotConnectStatus);
		BGI_MESSAGE("new init ScannerDriver end");
	}
	return m_spScannerDriver.data();
}