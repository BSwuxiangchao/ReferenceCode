#include "ImagerDriver.h"
#include <QThread>
#include "ImagerControlMgrIPCClient.h"
#include <QDateTime>
#include "BgiLogger.h"
#include "ParaType.h"
#include <QFile.h>
#include "MasterDevices.h"
#include "SchedulerDriver.h"
#include "DeviceErrorDescription.h"

ImagerDriver::ImagerDriver(QObject *parent)
	: QObject(parent)
	, m_bConnected(false)
	, m_VacuumValue(0)
	, m_StateBasecall(-1)
	, m_StateServer(-1)
	, m_StateImager(-1)
	, m_bSafePos(false)
{
	m_pThread = new QThread();
	m_pThread->start(QThread::NormalPriority);
	this->moveToThread(m_pThread);

	m_pImagerIPCClient = QSharedPointer<ImagerControlMgrIPCClient>(new ImagerControlMgrIPCClient(this));

	m_LastHeartbeat = QDateTime::currentDateTime().addDays(-1);
	m_pTimer = new QTimer();
	QObject::connect(m_pTimer, SIGNAL(timeout()), this, SLOT(CheckHeartbeat()), Qt::DirectConnection);
	m_pTimer->start(2000);
	m_pTimer->moveToThread(m_pThread);
}

ImagerDriver::~ImagerDriver()
{
	m_pTimer->stop();
	m_pTimer->deleteLater();
}

void ImagerDriver::Heartbeat()
{
	m_LastHeartbeat = QDateTime::currentDateTime();

	if (!m_bConnected)
	{
		SetConnectStatus(true);
	}
}

void ImagerDriver::CheckHeartbeat()
{
	if (m_bConnected)
	{
		int n = m_LastHeartbeat.secsTo(QDateTime::currentDateTime());
		if (n > 3)
		{
			SetConnectStatus(false);
		}
	}
	else
	{
		m_pImagerIPCClient->Connect();
	}
}

void ImagerDriver::SetConnectStatus(bool bConnected)
{
	if (m_bConnected != bConnected)
	{
		m_bConnected = bConnected;
		emit SignalImagerConnectStatus(m_bConnected);

		QString str = "Imager ConnectStatus changed: " + QString("%1").arg(bConnected);
		BGI_MESSAGE(str.toStdString().c_str());

		if (!bConnected)
		{
			ResetValue();
		}
	}
}

void ImagerDriver::GetConnStatus()
{
	emit SignalImagerConnectStatus(m_bConnected);
}

void ImagerDriver::ResetValue()
{
	m_VacuumValue = 0;
	m_StateBasecall = -1;
	m_StateServer = -1;
	m_StateImager = -1;
}

bool ImagerDriver::GoLoadingPos()
{
	if (m_bConnected)
	{
		return m_pImagerIPCClient->GoLoadingPos();
	}
	else
	{
		return false;
	}
}

bool ImagerDriver::IsAtLoadPosition()
{
	if (m_bConnected)
	{
		return m_pImagerIPCClient->IsAtLoadPosition();
	}
	else
	{
		return false;
	}
}

bool ImagerDriver::GoHome()
{
	if (m_bConnected)
	{
		return m_pImagerIPCClient->GoHome();
	}
	else
	{
		return false;
	}
}

bool ImagerDriver::SwitchVacuum(bool bOpen)
{
	if (m_bConnected)
	{
		return m_pImagerIPCClient->SwitchVacuum(bOpen);
	}
	else
	{
		return false;
	}
}

bool ImagerDriver::Reset()
{
	if (m_bConnected)
	{
		return m_pImagerIPCClient->Reset();
	}
	else
	{
		return false;
	}
}

bool ImagerDriver::ImagerConfigGet(ImagerParaType::ImagerConfigPara& data)
{
	if (m_bConnected)
	{
		return m_pImagerIPCClient->ImagerConfigGet(data);
	}
	else
	{
		return false;
	}
}

bool ImagerDriver::ImagerConfigGet()
{
	if (m_bConnected)
	{
		ImagerParaType::ImagerConfigPara data;
		bool bRet = m_pImagerIPCClient->ImagerConfigGet(data);
		if (bRet)
		{
			emit SignalImagerConfigData(data);
		}
		
		return bRet;
	}
	else
	{
		return false;
	}
}

bool ImagerDriver::ImagerConfigSet(const ImagerParaType::ImagerConfigPara& data)
{
	bool bRet = false;

	if (m_bConnected)
	{
		bRet = m_pImagerIPCClient->ImagerConfigSet(data);
	}

	emit SignalImagerConfigSet(bRet);
	return bRet;
}

void ImagerDriver::ReadExposureFile(QString path, QList<double>& exposureTime, QList<double>& exposureFactors)
{
	QFile file(path);
	if (file.open(QIODevice::ReadOnly))
	{
		QTextStream stream(&file);

		exposureTime.clear();
		exposureFactors.clear();

		QString line = "";
		bool bRet = false;
		double value = 0;
		while (!stream.atEnd())
		{
			line = stream.readLine();
			if (!line.isNull())
			{
				auto parts = line.split(',');
				if (parts.count() > 0)
				{
					value = parts[0].toDouble(&bRet);
					if (bRet)
					{
						exposureTime.append(value);
					}
				}
				if (parts.count() > 1)
				{
					value = parts[1].toDouble(&bRet);
					if (bRet)
					{
						exposureFactors.append(value);
					}
				}
			}
		}

		file.close();
	}
}

bool ImagerDriver::GetBasecallMems()
{
	QString str = "Imager GetBasecallMems";
	BGI_MESSAGE(str.toStdString().c_str());

	if (!m_bConnected)
	{
		return false;
	}

	QList<ImagerParaType::BasecallRunPara> infos;
	if (m_pImagerIPCClient->GetAllRunInformations(infos))
	{
		emit SignalBasecallRunInfos(infos);

		QString str = "GetBasecallMems size=" + QString::number(infos.size());
		BGI_MESSAGE(str.toStdString().c_str());
		return true;
	}
	else
	{
		BGI_MESSAGE("GetBasecallMems failed!");
		return false;
	}
}

bool ImagerDriver::CheckMemory(const QString& slideID)
{
	QString str = "Imager CheckMemory SlideID:" + slideID;
	BGI_MESSAGE(str.toStdString().c_str());

	if (!m_bConnected)
	{
		return false;
	}

	int startCycle = 0;
	if (m_pImagerIPCClient->GetRunInformation(slideID, startCycle))
	{
		QString str = "CheckMemory is Ready. SlideID:" + slideID + ", StartCycle=" + QString::number(startCycle);
		BGI_MESSAGE(str.toStdString().c_str());
		return true;
	}
	else
	{
		BGI_MESSAGE("CheckMemory not Ready!");
		return false;
	}
}

bool ImagerDriver::ImagerSetup(const GeneralParaType::SequencingTableDesc& sequencingInfo)
{
	QString str = "Imager ImagerSetup SlideID:" + sequencingInfo.strChipNo + ", ImagerCycle:" + QString("%1").arg(sequencingInfo.iPhotographTime);
	BGI_MESSAGE(str.toStdString().c_str());

	if (!m_bConnected)
	{
		return false;
	}

	return m_pImagerIPCClient->ImagerSetup(GetImagerSetupPara(sequencingInfo));
}

bool ImagerDriver::SetupRun(const GeneralParaType::SequencingTableDesc& sequencingInfo)
{
	QString str = "Imager SetupRun SlideID:" + sequencingInfo.strChipNo + ", ImagerCycle:" + QString("%1").arg(sequencingInfo.iPhotographTime);
	BGI_MESSAGE(str.toStdString().c_str());

	if (!m_bConnected)
	{
		return false;
	}

	return m_pImagerIPCClient->SetupRun(GetImagerSetupPara(sequencingInfo));
}

bool ImagerDriver::ReleaseRun(const GeneralParaType::SequencingTableDesc& sequencingInfo)
{
	QString str = "Imager ReleaseRun SlideID:" + sequencingInfo.strChipNo + ", ImagerCycle:" + QString("%1").arg(sequencingInfo.iPhotographTime);
	BGI_MESSAGE(str.toStdString().c_str());

	if (!m_bConnected)
	{
		return false;
	}

	return m_pImagerIPCClient->ReleaseRun(GetImagerSetupPara(sequencingInfo));
}

bool ImagerDriver::ReleaseRun(const QString& slideID)
{
	GeneralParaType::SqlCfgData data;
	if (OperateSqliteConfig::Instance()->QureyCfgDataByBarcode(slideID, data))
	{
		return ReleaseRun(data.strTableDesc);
	}
	else
	{
		//如果本地没有此芯片信息，尝试用Basecall返回的信息转换
		QList<ImagerParaType::BasecallRunPara> infos;
		if (m_pImagerIPCClient->GetAllRunInformations(infos))
		{
			foreach (auto info, infos)
			{
				if (info.SlideID == slideID)
				{
					QString str = "Imager ReleaseRun SlideID:" + info.SlideID + ", CurrentCycle:" + QString("%1").arg(info.CurrentCycle);
					BGI_MESSAGE(str.toStdString().c_str());

					return m_pImagerIPCClient->ReleaseRun(GetImagerSetupPara(info));
				}
			}
		}
	}

	return false;
}

ImagerParaType::ImagerSetupPara ImagerDriver::GetImagerSetupPara(const GeneralParaType::SequencingTableDesc& sequencingInfo)
{
	ImagerParaType::ImagerSetupPara data;

	data.SlideID = sequencingInfo.strChipNo;
	data.ScriptName = "img.py";
	data.ExperimentName = "TBJSeq";
	data.UserName = "TBJUser";
	data.ImagerID = 101;

	data.ZAFOffset = 0;
	data.IsCalAFOffset = (sequencingInfo.iPhotographTime == 1) || (sequencingInfo.iPhotographTime == sequencingInfo.iR1 + 1);

	data.Read1Len = sequencingInfo.iR1;
	data.Read2Len = sequencingInfo.iR2;
	data.Barcode1Len = sequencingInfo.iB1;
	data.Barcode2Len = sequencingInfo.iB2;

	data.Reference = sequencingInfo.strReference;
	data.BioBarcode.clear();

	data.TotalCycle = sequencingInfo.iAll;
	data.BiochemCycle = sequencingInfo.iBiochemistryTimes;
	data.ImagerCycle = sequencingInfo.iPhotographTime;

	data.TolerancePattern = 1;

	data.sequencingType = sequencingInfo.strSequencingMode;

	data.FqBarcode1Length = sequencingInfo.iB1 + sequencingInfo.iB2;
	data.FqBarcode1StartPos = sequencingInfo.iAll - data.FqBarcode1Length;
	data.FqSpeciesMismatch = 0;
	data.FqBarcode2Length = 0;
	data.FqBarcode2StartPos = 0;
	data.FqSpeciesMismatch2 = 0;

	data.FilterMode = 0;
	data.WriteFastqMode = 0;
	data.BarcodeReverse = 0;

	return data;
}

ImagerParaType::ImagerSetupPara ImagerDriver::GetImagerSetupPara(const ImagerParaType::BasecallRunPara& info)
{
	ImagerParaType::ImagerSetupPara data;

	data.SlideID = info.SlideID;
	data.ScriptName = "img.py";
	data.ExperimentName = "TBJSeq";
	data.UserName = "TBJUser";
	data.ImagerID = 101;

	data.ZAFOffset = 0;
	data.IsCalAFOffset = (info.CurrentCycle == 1) || (info.CurrentCycle == info.Read1Length + 1);

	data.Read1Len = info.Read1Length;
	data.Read2Len = info.Read2Length;
	data.Barcode1Len = info.BarcodeLength;
	data.Barcode2Len = 0;

	data.Reference = "NULL";
	data.BioBarcode.clear();

	data.TotalCycle = info.TotalCycle;
	data.BiochemCycle = info.CurrentCycle;
	data.ImagerCycle = info.CurrentCycle;

	data.TolerancePattern = 1;

	data.sequencingType = "Custom";

	data.FqBarcode1Length = info.BarcodeLength;
	data.FqBarcode1StartPos = info.TotalCycle - data.FqBarcode1Length;
	data.FqSpeciesMismatch = 0;
	data.FqBarcode2Length = 0;
	data.FqBarcode2StartPos = 0;
	data.FqSpeciesMismatch2 = 0;

	data.FilterMode = 0;
	data.WriteFastqMode = 0;
	data.BarcodeReverse = 0;

	return data;
}

bool ImagerDriver::StartRun()
{
	BGI_MESSAGE("Imager StartRun");

	if (!m_bConnected)
	{
		return false;
	}

	return m_pImagerIPCClient->StartRun();
}

bool ImagerDriver::GetVersions()
{
	BGI_MESSAGE("Imager GetVersions");

	if (!m_bConnected)
	{
		return false;
	}

	QString imagerVer = "";
	QString sbcVer = "";
	QString bcsVer = "";
	if (m_pImagerIPCClient->GetVersions(bcsVer, sbcVer, imagerVer))
	{
		emit SignalImagerVersions(imagerVer, sbcVer, bcsVer);

		QString str = "GetVersions imagerVer=" + imagerVer + ", sbcVer=" + sbcVer + ", bcsVer=" + bcsVer;
		BGI_MESSAGE(str.toStdString().c_str());
		return true;
	}
	else
	{
		BGI_MESSAGE("GetVersions failed!");
		return false;
	}
}

bool ImagerDriver::GetDiskSpace()
{
	BGI_MESSAGE("Imager GetDiskSpace");

	if (!m_bConnected)
	{
		return false;
	}

	double consumptionValue = 0;
	double availableValue = 0;
	if (m_pImagerIPCClient->GetDiskSpace(consumptionValue, availableValue))
	{
		emit SignalBasecallDiskSpace(consumptionValue, availableValue);

		QString str = "GetDiskSpace consumptionValue=" + QString::number(consumptionValue) + ", availableValue=" + QString::number(availableValue);
		BGI_MESSAGE(str.toStdString().c_str());
		return true;
	}
	else
	{
		BGI_MESSAGE("GetDiskSpace failed!");
		return false;
	}
}

void ImagerDriver::RecvImagerTaskEnd(const ImagerParaType::ImagerSetupPara& info, int error)
{
	QString str = "Imager RecvImagerTaskEnd SlideID:" + info.SlideID + ", ImagerCycle:" + QString("%1").arg(info.ImagerCycle) + ", ErrorEnum:" + QString::number(error);
	BGI_MESSAGE(str.toStdString().c_str());

	if (error == 0)
	{
		MasterDevices::Instance()->GetSchedulerDriver()->RecvImagerTaskEnd(info, 0, "");
	}
	else
	{
		QString str = GetAlarmString(error);
		MasterDevices::Instance()->GetSchedulerDriver()->RecvImagerTaskEnd(info, error, str);
	}
}

void ImagerDriver::RecvImagerError(int error)
{
	QString str = "Imager RecvImagerError ErrorEnum:" + QString::number(error);
	BGI_MESSAGE(str.toStdString().c_str());

	if (error != 0)
	{
		QString str = GetAlarmString(error);
		MasterDevices::Instance()->GetSchedulerDriver()->RecvImagerError(QString::number(error) + "=" + str);
	}
}

void ImagerDriver::RecvImagerRunInfo(const ImagerParaType::ImagerRunPara& info)
{
	QString str = "Imager RecvImagerRunInfo SlideID:" + info.SlideID
		+ ", CurrentRow:" + QString::number(info.CurrentRow)
		+ ", CurrentCol:" + QString::number(info.CurrentCol)
		+ ", CurrentCycle:" + QString::number(info.CurrentCycle)
		+ ", CurrentLane:" + QString::number(info.CurrentLane)
		+ ", StartRow:" + QString::number(info.StartRow)
		+ ", EndRow:" + QString::number(info.EndRow)
		+ ", StartCol:" + QString::number(info.StartCol)
		+ ", EndCol:" + QString::number(info.EndCol)
		+ ", TotalLane:" + QString::number(info.TotalLane)
		+ ", ScanDirect:" + QString::number(info.ScanDirect);
	BGI_MESSAGE(str.toStdString().c_str());

	emit SignalImagerRunInfo(info);
}

void ImagerDriver::RecvImagerVacuumValue(const QString& value)
{
	/*QString str = "Imager RecvImagerVacuumValue:" + value;
	BGI_MESSAGE(str.toStdString().c_str());*/

	bool bRet = false;
	float f = value.toFloat(&bRet);
	if (bRet)
	{
		//界面显示2位小数，是否需要刷新界面
		bool bViewSame = ((int)(m_VacuumValue * 100) == (int)(f * 100));

		m_VacuumValue = f;

		if (!bViewSame)
		{
			emit SignalImagerVacuumValue(f);
		}
	}
}

float ImagerDriver::GetVacuumValue()
{
	return m_VacuumValue;
}

void ImagerDriver::RecvImagerStatus(int basecall, int server, int imager)
{
	if (m_StateBasecall != basecall || m_StateServer != server || m_StateImager != imager)
	{
		m_StateBasecall = basecall;
		m_StateServer = server;
		m_StateImager = imager;
		emit SignalImagerStatus(basecall, server, imager);
	}
}

void ImagerDriver::RecvImagerData(const QString& key, const QString& value)
{
	QString log = "Imager RecvData:" + key + "=";
	if (value.size() > 1024)
	{
		log += value.mid(0, 1024);
	}
	else
	{
		log += value;
	}
	BGI_MESSAGE(log.toStdString().c_str());

	if (key != "QCData")
	{
		emit SignalImagerData(key, value);
	}
}

QString ImagerDriver::GetAlarmString(int errorCode)
{
	QString key = "ImagerError/" + QString::number(errorCode);
	QString str = "";
	DeviceErrorDescription::Instance()->GetCfgValue(key, str);
	return str;
}

void ImagerDriver::UpdateRobotSafetyStatus(bool bSafety)
{
	if (m_bSafePos != bSafety)
	{
		m_bSafePos = bSafety;
		emit SignalImagerSafePos(m_bSafePos);
	}
}
