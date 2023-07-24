#include"EpsonRobotDriver.h"
#include <qcoreapplication.h>
#include <qthread.h>
#include <QVector>
#include "BgiLogger.h"
#include "BgiTcpSocket.h"
#include "RobotOutputParaUtil.h"
#include "EpsonRobotFlowCtrl.h"
#include "DeviceDetailCfg.h"
#include "ErrorCodeTranslate.h"
#include "SchedulerDriver.h"
#include "MasterDevices.h"

//获取参数测数
const int gc_GetParaTimes = 5;

//关键字
const QString gc_EXprintMotor = "printMotor"; 
const QString gc_EXprintPower = "printPower"; 
const QString gc_EXprintSafetyOn = "printSafetyOn";
const QString gc_EXGetStatus = "GetStatus";
const QString gc_EXGetCurRobot = "GetCurRobot";
const QString gc_EXprintSpeed = "printPTPSpeed";
const QString gc_EXprintAccel = "printPTPAccel";
const QString gc_EXprintSpeeds = "printCPSpeeds";
const QString gc_EXprintAccels = "printCPAccels";
const QString gc_EXprintSpeedr = "printRSpeedr";
const QString gc_EXprintAccelr = "printRAccelr";
const QString gc_TferWait = "TferGet1";

EpsonRobotDriver::EpsonRobotDriver(BgiTcpSocket* socket)
	:BgiSocketCommDriver()
	, m_pSocket(socket)
	, m_bEnd(true)
	, m_bLogin(false)
	, m_bGetPara(false)
	, m_bSetPara(false)
	, m_bAtAction(false)
	, m_bRobotConnect(false)
	, m_bEmergencyStop(false)
	, m_bAtSafetyWorkSpace(false)
	, m_bAutoExplainBuff(true)
	, m_iErrCount(0)
	, m_iErrCount_21(0)
	, m_bInitCalibPointsInfoSuc(false)
	, m_bPythonMove(false)
	, m_bStopUnexpected(false)
	, m_bChangeToGo(false)
	, m_bChangeToMove(false)
	, m_bExplainBuffFinish(true)
	, m_bAtTferWait(false)
{

	m_OperateLightsing = false;
	SetTcpSocket(m_pSocket);

	ConnectSocketSlot();
	m_mapCalibPoints.clear();
}

EpsonRobotDriver::~EpsonRobotDriver()
{
	DisConnectSocketSlot();
	EndRead();

}

void EpsonRobotDriver::StartRead()
{
	m_bEnd = false;
	m_bEmergencyStop = false;
	m_pSchedulerDriver = MasterDevices::Instance()->GetSchedulerDriver();
	m_pSchedulerDriver->UpdateRobotSafetyStatus(false, false);
	Start();
}

void EpsonRobotDriver::EndRead()
{
	m_bEnd = true;
	m_bEmergencyStop = true;
	if (!IsFinished())
		WaitForEnd();

}

void EpsonRobotDriver::ApplicationSleep(int mSec)
{
	QThread::msleep(mSec);
	//while (mSec)
	//{
	//	QCoreApplication::processEvents();
	//	//QThread::msleep(1);
	//	mSec--;
	//}
}

void EpsonRobotDriver::InitCalibPointsInfo()
{
	bool bOk = false;
	m_bAutoExplainBuff = true;
	m_bInitCalibPointsInfoSuc = false;
	m_mapCalibPoints.clear();
	QStringList PList = DeviceDetailCfg::Instance()->GetListPointVar();
	for (auto PVar : PList)
	{
		QVariant Value;
		QString keyWord = "RobotPosSign";
		QString strCommand = QString("$Execute,\"print %1;print \"%2\"\r\n").arg(PVar).arg(keyWord);
		RobotParaType::RobotPoint robotPosSign;
		if (ConductCommandUnit(strCommand, keyWord, Value))
		{
			robotPosSign = Value.value<RobotParaType::RobotPoint>();
			if (robotPosSign.f_X == 0.0 || robotPosSign.f_Y == 0.0 || robotPosSign.f_Z == 0.0) //xyz有一个为0 则判断为不正确的点
			{
				bOk = false;
				break;
			}
			/*QString strPos = QString("%1：%2 %3 %4 %5 %6 %7").arg(PVar).arg(robotPosSign.f_X).arg(robotPosSign.f_Y).arg(robotPosSign.f_Z)
				.arg(robotPosSign.f_U).arg(robotPosSign.f_V).arg(robotPosSign.f_W);
			ErrorCodeTranslate::Instance()->RecordInfo("EpsonRobotDriver", strPos, gc_colorBlack,true);*/
			m_mapCalibPoints.insert(PVar, robotPosSign);
			bOk = true;
		}
		else
		{
			bOk = false;
			break;
		}
	}
	if (bOk)
		if (PList.size() == m_mapCalibPoints.size())
			bOk = true;
		else
			bOk = false;
	m_bAutoExplainBuff = false;
	if (bOk)
		m_bInitCalibPointsInfoSuc = true;
}

QString EpsonRobotDriver::CheckStringPoint(const RobotParaType::RobotPoint robotPoint)
{
	QMap<QString, RobotParaType::RobotPoint>::iterator itStart = m_mapCalibPoints.begin();
	QMap<QString, RobotParaType::RobotPoint>::iterator itEnd = m_mapCalibPoints.end();
	for (auto itr =itStart;itr != itEnd;itr++)
	{
		if (itr.value() == robotPoint)
		{
			if (itr.key() == gc_TferWait)
				m_bAtTferWait = true;
			else
				m_bAtTferWait = false;
			return itr.key();
		}
	}
	m_bAtTferWait = false;
	return robotPoint.strPointName;
}

int CmpPoints(void const* arg1, void const* arg2)
{
	RobotParaType::RobotPoint a = *(RobotParaType::RobotPoint*)arg1;
	RobotParaType::RobotPoint b = *(RobotParaType::RobotPoint*)arg2;
	if (a.f_X < b.f_X && a.f_Y < b.f_Y && a.f_Z < b.f_Z && a.f_U < b.f_U && a.f_V < b.f_V && a.f_W < b.f_W)
		return 1;
	return 0;
}

void EpsonRobotDriver::RecordNearPointNameOfHere()
{
	QString keyWord = "RobotPosHere";
	QString strCommand = QString("$Execute,\"print here;print \"%1\"\r\n").arg(keyWord);
	QVariant Value;
	RobotParaType::RobotPoint robotPosHere;
	bool bOk = false;
	m_bGetPara = true;
	ApplicationSleep(2000);

	if (ConductCommandUnit(strCommand, keyWord, Value))
	{
		robotPosHere = Value.value<RobotParaType::RobotPoint>();
		bOk = true;
	}
	m_bGetPara = false;
	if (!bOk)
	{
		ErrorCodeTranslate::Instance()->RecordInfo("EpsonRobotDriver", "机械臂点击关闭时获取位置失败", gc_colorYellow, true);
		return;
	}
	if (m_mapCalibPoints.size() <= 0)
		return;
	QString strPointName = CheckStringPoint(robotPosHere);
	if (strPointName != robotPosHere.strPointName)
	{
		ErrorCodeTranslate::Instance()->RecordInfo("EpsonRobotDriver", QString("机械臂点击关闭时位于：%1！").arg(strPointName), gc_colorBlack,true);
	}
	else
	{
		QVector<RobotParaType::RobotPoint> regionPointsVec;
		QVector<double> vecDistance;
		QMap<QString, RobotParaType::RobotPoint> mapCalibPoints(m_mapCalibPoints);

		QMap<QString, RobotParaType::RobotPoint>::iterator itStart = mapCalibPoints.begin();
		QMap<QString, RobotParaType::RobotPoint>::iterator itEnd = mapCalibPoints.end();
		for (auto itr = itStart; itr != itEnd; itr++)
		{
			itr.value().strPointName = itr.key();
			regionPointsVec.push_back(itr.value());
			RobotParaType::RobotPoint robotPosSign = itr.value();
			double fDis = (robotPosHere.f_X - robotPosSign.f_X) * (robotPosHere.f_X - robotPosSign.f_X)
				+ (robotPosHere.f_Y - robotPosSign.f_Y) * (robotPosHere.f_Y - robotPosSign.f_Y)
				+ (robotPosHere.f_Z - robotPosSign.f_Z) * (robotPosHere.f_Z - robotPosSign.f_Z);
			vecDistance.append(fDis);
		}
		
		auto minVal = std::min_element(std::begin(vecDistance), std::end(vecDistance));
		auto minIndex = std::distance(std::begin(vecDistance), minVal);
		//_CoreCrtNonSecureSearchSortCompareFunction ff = (_CoreCrtNonSecureSearchSortCompareFunction)CmpPoints;
		//qsort(&regionPointsVec[0], regionPointsVec.size(), sizeof(RobotParaType::RobotPoint), CmpPoints);
		RobotParaType::RobotPoint robotPosSign = regionPointsVec.at(minIndex);
		ErrorCodeTranslate::Instance()->RecordInfo("EpsonRobotDriver", QString("机械臂点击关闭时接近：%1！").arg(robotPosSign.strPointName), gc_colorBlack, true);
		QString strPos = QString("%1：%2 %3 %4 %5 %6 %7").arg(robotPosSign.strPointName).arg(robotPosSign.f_X).arg(robotPosSign.f_Y).arg(robotPosSign.f_Z)
			.arg(robotPosSign.f_U).arg(robotPosSign.f_V).arg(robotPosSign.f_W);
		ErrorCodeTranslate::Instance()->RecordInfo("EpsonRobotDriver", strPos, gc_colorBlack, true);
		strPos = QString("%1：%2 %3 %4 %5 %6 %7").arg("Here").arg(robotPosHere.f_X).arg(robotPosHere.f_Y).arg(robotPosHere.f_Z)
			.arg(robotPosHere.f_U).arg(robotPosHere.f_V).arg(robotPosHere.f_W);
		ErrorCodeTranslate::Instance()->RecordInfo("EpsonRobotDriver", strPos, gc_colorBlack, true);
	}
	return ;
}

void EpsonRobotDriver::SlotConnectStatus(bool bSuccess)
{
	m_bRobotConnect = bSuccess;
	m_strRobotStatus.iConnect = bSuccess;
	RobotOutputParaUtil::SetRobotStatus(m_strRobotStatus);
	m_pSchedulerDriver->UpdateRobotSafetyStatus(false, false);
	if (!bSuccess)
	{
		m_bAtSafetyWorkSpace = false;
		m_strRobotStatus.iSafetySpace = 0;
		RobotOutputParaUtil::SetRobotStatus(m_strRobotStatus);
	}
}

void EpsonRobotDriver::Run()
{
	while (!m_bEnd)
	{
		if (!m_bRobotConnect)
		{
			// 相当于客户端发送心跳包
			QString strCommand = QString("$%1,CLRF").arg(gc_EXGetStatus);
			SendCommand(strCommand, false);
			ApplicationSleep(1000);
		}
		else if (!m_bGetPara && !m_bSetPara)
		{
			// 自动获取参数
			AutoGetPara();
			// 获取机械臂当前位置
			GetCurPos();
		}
		else
		{
			ApplicationSleep(WAIT_REPLY_SLEEP);
		}
	}
}

void EpsonRobotDriver::AutoGetPara()
{
	do
	{
		int iGapTime = 50;
		ApplicationSleep(iGapTime);
		QString strCommand = QString("$Execute,\"print Motor;print \"%1\"\r\n").arg(gc_EXprintMotor);
		SendCommand(strCommand, false);
		ApplicationSleep(iGapTime);
		if (m_bGetPara || m_bSetPara)
			break;
		strCommand = QString("$Execute,\"print power;print \"%1\"\r\n").arg(gc_EXprintPower);
		SendCommand(strCommand, false);
		ApplicationSleep(iGapTime);
		if (m_bGetPara || m_bSetPara)
			break;
		strCommand = QString("$Execute,\"print safetyOn;print \"%1\"\r\n").arg(gc_EXprintSafetyOn);
		SendCommand(strCommand, false);
		ApplicationSleep(iGapTime);
		if (m_bGetPara || m_bSetPara)
			break;
		strCommand = QString("$%1,CLRF").arg(gc_EXGetStatus);
		SendCommand(strCommand, false);
		ApplicationSleep(iGapTime);
		if (m_bGetPara || m_bSetPara)
			break;
		//RobotOutputParaUtil::SetRobotStatus(m_strRobotStatus);

		strCommand = QString("$%1,CLRF").arg(gc_EXGetCurRobot);
		SendCommand(strCommand, false);
		ApplicationSleep(iGapTime);
		if (m_bGetPara || m_bSetPara)
			break;
		strCommand = QString("$Execute,\"print speed;print \"%1\"\r\n").arg(gc_EXprintSpeed);
		SendCommand(strCommand, false);
		ApplicationSleep(iGapTime);
		if (m_bGetPara || m_bSetPara)
			break;
		strCommand = QString("$Execute,\"print Accel(1);print \"%1\"\r\n").arg(gc_EXprintAccel);
		SendCommand(strCommand, false);
		ApplicationSleep(iGapTime);
		if (m_bGetPara || m_bSetPara)
			break;
		strCommand = QString("$Execute,\"print speeds;print \"%1\"\r\n").arg(gc_EXprintSpeeds);
		SendCommand(strCommand, false);
		ApplicationSleep(iGapTime);
		if (m_bGetPara || m_bSetPara)
			break;
		strCommand = QString("$Execute,\"print Accels(1);print \"%1\"\r\n").arg(gc_EXprintAccels);
		SendCommand(strCommand, false);
		ApplicationSleep(iGapTime);
		if (m_bGetPara || m_bSetPara)
			break;
		strCommand = QString("$Execute,\"print speedr;print \"%1\"\r\n").arg(gc_EXprintSpeedr);
		SendCommand(strCommand, false);
		ApplicationSleep(iGapTime);
		if (m_bGetPara || m_bSetPara)
			break;
		strCommand = QString("$Execute,\"print Accelr(1);print \"%1\"\r\n").arg(gc_EXprintAccelr);
		SendCommand(strCommand, false);
		ApplicationSleep(iGapTime*3);
		if (m_bGetPara || m_bSetPara)
			break;
		//RobotOutputParaUtil::SetRobotPara(m_RobotParaDesc);
		m_bAutoExplainBuff = false;
		ApplicationSleep(iGapTime);
		IsInSafetyWorkSpace();
		ApplicationSleep(iGapTime);
		m_bAutoExplainBuff = true;
	} while (0);
}

void EpsonRobotDriver::GetCurPos()
{
	if (!m_bInitCalibPointsInfoSuc)
	{
		m_bAutoExplainBuff = false;
		InitCalibPointsInfo();
		m_bAutoExplainBuff = true;
	}

	QString strCommand = "$Execute,\"print here;print \"RobotPosHere\"\r\n";
	SendCommand(strCommand, false);
	ApplicationSleep(10);
}

void EpsonRobotDriver::ExplainBuff(QByteArray buf)
{
	m_BgiLock.Acquire();
	m_curBuf = buf;

	if (!m_bGetPara && !m_bSetPara && m_bAutoExplainBuff)
	{
		if(m_bExplainBuffFinish)
			TryPraseQueryOutputValueInThread(buf);
	}
	m_BgiLock.Release();
}

int EpsonRobotDriver::NeedBreakExplainBuff(QString strBuf, QString indexOfVar)
{
	//字符串不符合条件，但是B符合，返回0进行下一个判断
	if (!(strBuf.indexOf(indexOfVar) > 0) && (!m_bGetPara && !m_bSetPara && m_bAutoExplainBuff))
		return 0;
	//字符串符合条件，B符合，执行后跳出
	else if ((strBuf.indexOf(indexOfVar) > 0) && (!m_bGetPara && !m_bSetPara && m_bAutoExplainBuff))
		return 1;
	//字符串符合条件，B不符合，执行后跳出
	else if ((strBuf.indexOf(indexOfVar) > 0) && (!m_bGetPara && !m_bSetPara && m_bAutoExplainBuff))
		return 1;
	//直接返回
	return 2;
}

int EpsonRobotDriver::ReturnPaseValueToInt(QString strBUffer, QString KeyWords)
{
	QString ttt = strBUffer.remove("\r").remove("\n").remove("\"").trimmed();
	QVariant Value = ttt.mid(0, strlen(ttt.toStdString().c_str()) - strlen(KeyWords.toStdString().c_str())).trimmed();
	return Value.toInt();
}

void EpsonRobotDriver::TryPraseQueryOutputValueInThread(QByteArray buf)
{
	m_bExplainBuffFinish = false;
	QString strBuf = buf;
	QStringList list = strBuf.split(',');
	if (list.size() == 2 && list.at(1).toInt() == 11)
	{
		Login();
		m_bExplainBuffFinish = true;
		return;
	}

	bool isExecuteOK = false;
	if(strBuf.mid(0,1)=="#")
		isExecuteOK = true;
	if (isExecuteOK)
	{
		m_iErrCount = 0;

		if (NeedBreakExplainBuff(strBuf,"RobotPosHere") > 0)
		{
			QString strPos = strBuf.mid(11, 72);
			/*QString s1 = strPos.mid(2, 10);
			QString s2 = strPos.mid(14, 10);
			QString s3 = strPos.mid(26, 10);
			QString s4 = strPos.mid(38, 10);
			QString s5 = strPos.mid(50, 10);
			QString s6 = strPos.mid(62, 10);*/
			RobotParaType::RobotPoint robotPos;
			robotPos.f_X = strPos.mid(2, 10).trimmed().toFloat();
			robotPos.f_Y = strPos.mid(14, 10).trimmed().toFloat();
			robotPos.f_Z = strPos.mid(26, 10).trimmed().toFloat();
			robotPos.f_U = strPos.mid(38, 10).trimmed().toFloat();
			robotPos.f_V = strPos.mid(50, 10).trimmed().toFloat();
			robotPos.f_W = strPos.mid(62, 10).trimmed().toFloat();
			if (m_bInitCalibPointsInfoSuc)
				robotPos.strPointName = CheckStringPoint(robotPos);
			m_strPoint = robotPos;
			RobotOutputParaUtil::SetRobotPos(m_strPoint);
			m_bExplainBuffFinish = true;
			return;
		}
		if (list.size() == 2)
		{
			QString strBUffer = list[1];
			do
			{
				if (NeedBreakExplainBuff(strBuf, gc_EXGetCurRobot) > 0)
				{
					if (NeedBreakExplainBuff(strBuf, gc_EXGetCurRobot) > 1) break;
					m_RobotParaDesc.iMotorNo = ReturnPaseValueToInt(strBUffer, gc_EXGetCurRobot);
					RobotOutputParaUtil::SetRobotPara(m_RobotParaDesc);
				}
				else if (NeedBreakExplainBuff(strBuf, gc_EXprintSpeed) > 0)
				{
					if (NeedBreakExplainBuff(strBuf, gc_EXprintSpeed) > 1) break;
					m_RobotParaDesc.iPTPSpeed = ReturnPaseValueToInt(strBUffer, gc_EXprintSpeed);
					RobotOutputParaUtil::SetRobotPara(m_RobotParaDesc);
				}
				else if (NeedBreakExplainBuff(strBuf, gc_EXprintAccel) > 0)
				{
					if (NeedBreakExplainBuff(strBuf, gc_EXprintAccel) > 1) break;
					m_RobotParaDesc.iPTPAccel = ReturnPaseValueToInt(strBUffer, gc_EXprintAccel);
					RobotOutputParaUtil::SetRobotPara(m_RobotParaDesc);
				}
				else if (NeedBreakExplainBuff(strBuf, gc_EXprintSpeeds) > 0)
				{
					if (NeedBreakExplainBuff(strBuf, gc_EXprintSpeeds) > 1) break;
					m_RobotParaDesc.iCPSpeed = ReturnPaseValueToInt(strBUffer, gc_EXprintSpeeds);
					RobotOutputParaUtil::SetRobotPara(m_RobotParaDesc);
				}
				else if (NeedBreakExplainBuff(strBuf, gc_EXprintAccels) > 0)
				{
					if (NeedBreakExplainBuff(strBuf, gc_EXprintAccels) > 1) break;
					m_RobotParaDesc.iCPAccel = ReturnPaseValueToInt(strBUffer, gc_EXprintAccels);
					RobotOutputParaUtil::SetRobotPara(m_RobotParaDesc);
				}
				else if (NeedBreakExplainBuff(strBuf, gc_EXprintSpeedr) > 0)
				{
					if (NeedBreakExplainBuff(strBuf, gc_EXprintSpeedr) > 1) break;
					m_RobotParaDesc.iRSpeed = ReturnPaseValueToInt(strBUffer, gc_EXprintSpeedr);
					RobotOutputParaUtil::SetRobotPara(m_RobotParaDesc);
				}
				else if (NeedBreakExplainBuff(strBuf, gc_EXprintAccelr) > 0)
				{
					if (NeedBreakExplainBuff(strBuf, gc_EXprintAccelr) > 1) break;
					m_RobotParaDesc.iRAccel = ReturnPaseValueToInt(strBUffer, gc_EXprintAccelr);
					RobotOutputParaUtil::SetRobotPara(m_RobotParaDesc);
				}
				else if (NeedBreakExplainBuff(strBuf, gc_EXprintMotor) > 0)
				{
					if (NeedBreakExplainBuff(strBuf, gc_EXprintMotor) > 1) break;
					m_strRobotStatus.iMotorOn = strBuf.mid(11, 1).toInt();
					RobotOutputParaUtil::SetRobotStatus(m_strRobotStatus);
				}
				else if (NeedBreakExplainBuff(strBuf, gc_EXprintPower) > 0)
				{
					if (NeedBreakExplainBuff(strBuf, gc_EXprintPower) > 1) break;
					m_strRobotStatus.iPower = strBuf.mid(11, 1).toInt();
					RobotOutputParaUtil::SetRobotStatus(m_strRobotStatus);
				}
				else if (NeedBreakExplainBuff(strBuf, gc_EXprintSafetyOn) > 0)
				{
					if (NeedBreakExplainBuff(strBuf, gc_EXprintSafetyOn) > 1) break;
					m_strRobotStatus.iSafetyOn = strBuf.mid(11, 1).toInt();
					RobotOutputParaUtil::SetRobotStatus(m_strRobotStatus);
				}
			} while (0);

			m_bExplainBuffFinish = true;
			return;
		}

		if (list.size() == 3)
		{
			if (NeedBreakExplainBuff(strBuf, gc_EXGetStatus) > 0)
			{
				// 11位标志位含义：Test/Teach/Auto/Warning/SError/Safeguard/EStop/Error/Paused/Running/Ready
				QString strSignFlag = list[1];
				if (m_bEmergencyStop)
				{
					m_strRobotStatus.iEstopOn = 1;
					m_strRobotStatus.iErrorCode = 9999;
				}
				else
				{
					if (strSignFlag.length() > 6)
						m_strRobotStatus.iEstopOn = QString(strSignFlag.at(6)).toInt();
					m_strRobotStatus.iErrorCode = list[2].toInt();
				}
				RobotOutputParaUtil::SetRobotStatus(m_strRobotStatus);
			}
		}
	}
	else
	{
		if (list.size() == 2)
		{
			int iTotalCount = 100;
			if (m_iErrCount_21 > iTotalCount)
			{
				m_strRobotStatus.iErrorCode = list[1].toInt();
				RobotOutputParaUtil::SetRobotStatus(m_strRobotStatus);
				Logout();
				m_iErrCount_21 = 0;
				return;
			}
			if (m_iErrCount > 10)
			{
				m_strRobotStatus.iErrorCode = list[1].toInt();
				RobotOutputParaUtil::SetRobotStatus(m_strRobotStatus);
				m_iErrCount = 0;
			}
			if (list[1].toInt() == 21)
				m_iErrCount_21++;

			m_iErrCount++;
		}
		
	}
	m_bExplainBuffFinish = true;

}

bool EpsonRobotDriver::TryPraseQueryOutputValueByKeyWords(QString keyWord, QVariant& Value)
{
	int count = 0;
	QString strBuf = "";
	while (count < WAIT_REPLY_SUM)
	{
		if (m_bEmergencyStop && !m_OperateLightsing)
			return false;
		QByteArray buf;
		m_pSocket->Read(buf);
		strBuf = buf;
		ApplicationSleep(WAIT_REPLY_SLEEP*2);
		if (strBuf.indexOf(keyWord) > 0)
		{
			if (strBuf.indexOf("RobotPosHere") > 0 || strBuf.indexOf("RobotPosSign") > 0)
			{
				QString strPos = strBuf.mid(11, 72);
				/*QString s1 = strPos.mid(2, 10);
				QString s2 = strPos.mid(14, 10);
				QString s3 = strPos.mid(26, 10);
				QString s4 = strPos.mid(38, 10);
				QString s5 = strPos.mid(50, 10);
				QString s6 = strPos.mid(62, 10);*/
				RobotParaType::RobotPoint robotPos;
				robotPos.f_X = strPos.mid(2, 10).trimmed().toFloat();
				robotPos.f_Y = strPos.mid(14, 10).trimmed().toFloat();
				robotPos.f_Z = strPos.mid(26, 10).trimmed().toFloat();
				robotPos.f_U = strPos.mid(38, 10).trimmed().toFloat();
				robotPos.f_V = strPos.mid(50, 10).trimmed().toFloat();
				robotPos.f_W = strPos.mid(62, 10).trimmed().toFloat();
				if (m_bInitCalibPointsInfoSuc && !m_bPythonMove)
					robotPos.strPointName = CheckStringPoint(robotPos);
				m_strPoint = robotPos;
				RobotOutputParaUtil::SetRobotPos(m_strPoint);
				Value = QVariant::fromValue<RobotParaType::RobotPoint>(robotPos);
				return true;
			}

			QStringList templist = strBuf.split(',');
			if (templist.size() == 2)
			{
				QString ttt = templist[1].remove("\r").remove("\n").remove("\"").trimmed();
				Value = ttt.mid(0, strlen(ttt.toStdString().c_str()) - strlen(keyWord.toStdString().c_str())).trimmed();

				return true;
			}
		}
		count++;
	}
	if (!m_bAutoExplainBuff)
		return false;
	QStringList list = strBuf.split(',');
	if (list.size() == 2 )
	{
		if (list[1].toInt() != 0)
		{
			if (list[1].toInt() == 21)
			{
				m_strRobotStatus.iErrorCode = 21;
				RobotOutputParaUtil::SetRobotStatus(m_strRobotStatus);
			}
			else if (list[1].toInt() == 99)
			{
				//ErrorCodeTranslate::Instance()->RecordInfo("EpsonRobotDriver", QString("%1 stop").arg(strBuf), gc_colorYellow);
				if (m_bPythonMove)
					m_bStopUnexpected = true;
			}
		}
	}

	return false;
}

bool EpsonRobotDriver::TryPraseQueryOutputValueByKeyWords(QString keyWord)
{
	int count = 0;
	QString strBuf = "";
	while (count < WAIT_REPLY_SUM)
	{
		if (m_bEmergencyStop && !m_OperateLightsing)
			return false;
		QByteArray buf;
		m_pSocket->Read(buf);
		strBuf = buf;
		ApplicationSleep(WAIT_REPLY_SLEEP*2);
		if (strBuf.indexOf(keyWord) > 0)
		{
			QStringList templist = strBuf.split(',');
			if (templist.size() == 2)
			{
				if(templist[1].toInt() == 0 )
				{
					return true;
				}
				else
				{
					if (templist[1].toInt() == 99) //系统错误，运行时遇到碰撞检测点，返回的也是99
					{
						//ErrorCodeTranslate::Instance()->RecordInfo("EpsonRobotDriver", QString("%1 stop").arg(strBuf), gc_colorYellow);
						if (m_bPythonMove)
							m_bStopUnexpected = true;
					}
				}
			}
		}
		count++;
	}
	return false;
}

bool EpsonRobotDriver::ConductCommandUnit(QString strCommand,QString keyWord, QVariant& Value)
{
	int count = 0;
	bool bSuc = false;
	while (count < gc_GetParaTimes)
	{
		if (m_bEmergencyStop && !m_OperateLightsing)
			return false;
		SendCommand(strCommand);
		if(m_bGetPara)
			bSuc = TryPraseQueryOutputValueByKeyWords(keyWord, Value);
		else if(m_bSetPara)
			bSuc = TryPraseQueryOutputValueByKeyWords(keyWord);
		else
			bSuc = TryPraseQueryOutputValueByKeyWords(keyWord, Value);

		if (bSuc)
			break;
		
		ApplicationSleep(WAIT_REPLY_SLEEP*10);
		count++;
		if (m_bAtAction)
			break;
	}
	return bSuc;
}

void EpsonRobotDriver::SendCommand(QString strCommand, bool bRecord)
{
	QByteArray buf = strCommand.toUtf8();
	m_pSocket->CleanBuf();
	m_pSocket->DelayWrite(buf);
	if(bRecord)
		BGI_MESSAGE(strCommand.toStdString().c_str()); 
}

/* --------------------------机械臂指令集-------------------------------------*/

void EpsonRobotDriver::Login()
{
	m_bLogin = false;
	QString strCommand = "$Login,12345,CLRF";
	SendCommand(strCommand);
}

bool EpsonRobotDriver::Logout()
{
	//QString strCommand = "$Logout,CLRF";
	//SendCommand(strCommand);
	m_bAutoExplainBuff = false;
	QString keyWord = "Logout";
	QString strCommand = QString("$%1,CLRF").arg(keyWord);
	QVariant Value;
	bool bOk = false;
	if (ConductCommandUnit(strCommand, keyWord, Value))
		bOk = true;
	m_bAutoExplainBuff = true;
	return bOk;
}

bool EpsonRobotDriver::GetCurRobot(int& iMotorNo)
{
	m_bGetPara = true;
	QString keyWord = gc_EXGetCurRobot;
	QString strCommand = QString("$%1,CLRF").arg(keyWord);
	QVariant Value;
	bool bOk = false;
	if (ConductCommandUnit(strCommand, keyWord, Value))
	{
		iMotorNo = Value.toInt();
		m_RobotParaDesc.iMotorNo = Value.toInt();
		RobotOutputParaUtil::SetRobotPara(m_RobotParaDesc);
		bOk = true;
	}
	m_bGetPara = false;
	return bOk;
}

bool EpsonRobotDriver::SetMotorsOn()
{
	m_bEmergencyStop = false;
	m_bSetPara = true;
	//QString strCommand = "$SetMotorsOn,1,CLRF";
	//SendCommand(strCommand);
	QString keyWord = "SetMotorsOn";
	QString strCommand = QString("$%1,1,CLRF").arg(keyWord);
	QVariant Value;
	bool bOk = false;
	if (ConductCommandUnit(strCommand, keyWord, Value))
	{
		bOk = true;
		m_RobotParaDesc.iMotorNo = Value.toInt();
		RobotOutputParaUtil::SetRobotPara(m_RobotParaDesc);
	}
	m_bSetPara = false;
	return bOk;
}

bool EpsonRobotDriver::SetMotorsOff()
{
	m_bSetPara = true;
	QString keyWord = "SetMotorsOff";
	QString strCommand = QString("$%1,1,CLRF").arg(keyWord);
	QVariant Value;
	bool bOk = false;
	if (ConductCommandUnit(strCommand, keyWord, Value))
	{
		bOk = true;
		m_RobotParaDesc.iMotorNo = Value.toInt();
		RobotOutputParaUtil::SetRobotPara(m_RobotParaDesc);
	}
	m_bSetPara = false;
	if (bOk)
		RecordNearPointNameOfHere();
	return bOk;
}

bool EpsonRobotDriver::Reset()
{
	m_bSetPara = true;
	m_bEmergencyStop = false;
	//QString strCommand = "$Reset,CLRF";
	//SendCommand(strCommand);
	QString keyWord = "Reset";
	QString strCommand = QString("$%1,CLRF").arg(keyWord);
	QVariant Value;
	bool bOk = false;
	if (ConductCommandUnit(strCommand, keyWord, Value))
		bOk = true;
	m_bSetPara = false;
	m_bGetPara = false;
	/*if(bOk)
		bOk = SetMotorsOn();
	if (bOk)
		bOk = SetPower(true);*/
	if(bOk)
		OperateWaringLights(RobotParaType::RobotLightColor::NONE);
	return bOk;
}

bool EpsonRobotDriver::Pause()
{
	m_bSetPara = true;
	QString keyWord = "Pause";
	QString strCommand = QString("$%1,CLRF").arg(keyWord);
	QVariant Value;
	bool bOk = false;
	if (ConductCommandUnit(strCommand, keyWord, Value))
		bOk = true;
	m_bSetPara = false;
	return bOk;
}

bool EpsonRobotDriver::Stop()
{
	m_bSetPara = true;
	QString keyWord = "Stop";
	QString strCommand = QString("$%1,CLRF").arg(keyWord);
	QVariant Value;
	bool bOk = false;
	if (ConductCommandUnit(strCommand, keyWord, Value))
		bOk = true;
	m_bSetPara = false;
	return bOk;
}

bool EpsonRobotDriver::Continue()
{
	m_bSetPara = true;
	QString keyWord = "Continue";
	QString strCommand = QString("$%1,CLRF").arg(keyWord);
	QVariant Value;
	bool bOk = false;
	if (ConductCommandUnit(strCommand, keyWord, Value))
		bOk = true;
	m_bSetPara = false;
	return bOk;
}

bool EpsonRobotDriver::Abort()
{
	m_bStopUnexpected = false;
	m_bEmergencyStop = true;
	QString strCommand = "$Abort,CLRF";
	SendCommand(strCommand);
	OperateWaringLights(RobotParaType::RobotLightColor::RED);
	return true;
}

bool EpsonRobotDriver::SetPower(bool bHigh)
{
	m_bSetPara = true;
	QString keyWord = "execute";
	QString strCommand = QString("$%1,\"Power High\"\r\n").arg(keyWord);
	if(!bHigh)
		strCommand = QString("$%1,\"Power Low\"\r\n").arg(keyWord);
	QVariant Value;
	bool bOk = false;
	if (ConductCommandUnit(strCommand, keyWord, Value))
	{
		bOk = true;
		m_RobotParaDesc.iPower = Value.toInt();
		RobotOutputParaUtil::SetRobotPara(m_RobotParaDesc);
	}
	m_bSetPara = false;
	return bOk;
}

bool EpsonRobotDriver::SetPTP_Speed(int iSpeed)
{
	if (iSpeed <= 0 || iSpeed>100)
		return false;
	m_bChangeToMove = false;
	m_bChangeToGo = false;
	m_bSetPara = true;
	QString keyWord = "execute";
	QString strCommand = QString("$%1,\"Speed %2\"\r\n").arg(keyWord).arg(iSpeed);
	QVariant Value;
	bool bOk = false;
	if (ConductCommandUnit(strCommand, keyWord, Value))
	{
		bOk = true;
		m_RobotParaDesc.iPTPSpeed = Value.toInt();
		RobotOutputParaUtil::SetRobotPara(m_RobotParaDesc);
	}
	m_bSetPara = false;
	return bOk;
}

bool EpsonRobotDriver::GetPTP_Speed(int& iSpeed)
{
	m_bGetPara = true;
	QString keyWord = gc_EXprintSpeed;
	QString strCommand = QString("$Execute,\"print speed;print \"%1\"\r\n").arg(keyWord);
	QVariant Value;
	bool bOk = false;
	if (ConductCommandUnit(strCommand, keyWord, Value))
	{
		iSpeed = Value.toInt();
		bOk = true;
	}
	m_bGetPara = true;
	return bOk;
}

bool EpsonRobotDriver::SetPTP_Accel(int iAccel)
{
	if (iAccel <= 0 || iAccel > 120)
		return false;
	m_bChangeToMove = false;
	m_bChangeToGo = false;
	m_bSetPara = true;
	//PTP减速度永远设置为5（暂时方案，accel 需要加速度减速度一起才能设置生效）
	QString keyWord = "execute";
	QString strCommand = QString("$%1,\"Accel %2,%3\"\r\n").arg(keyWord).arg(iAccel).arg(iAccel);
	QVariant Value; 
	bool bOk = false;
	if (ConductCommandUnit(strCommand, keyWord, Value))
	{
		bOk = true;
		m_RobotParaDesc.iPTPAccel = Value.toInt();
		RobotOutputParaUtil::SetRobotPara(m_RobotParaDesc);
	}
	m_bSetPara = false;
	return bOk;
}

bool EpsonRobotDriver::GetPTP_Accel(int& iAccel)
{
	m_bGetPara = true;
	QString keyWord = gc_EXprintAccel;
	QString strCommand = QString("$Execute,\"print Accel(1);print \"%1\"\r\n").arg(keyWord);
	QVariant Value;
	bool bOk = false;
	if (ConductCommandUnit(strCommand, keyWord, Value))
	{
		iAccel = Value.toInt();
		bOk = true;
	}
	m_bGetPara = false;
	return bOk;
}

bool EpsonRobotDriver::SetCP_Speeds(int iSpeed)
{
	if (iSpeed <= 0 || iSpeed > 2000)
		return false;
	m_bChangeToMove = false;
	m_bChangeToGo = false;
	m_bSetPara = true;
	QString keyWord = "execute";
	QString strCommand = QString("$%1,\"Speeds %2\"\r\n").arg(keyWord).arg(iSpeed);
	QVariant Value;
	bool bOk = false;
	if (ConductCommandUnit(strCommand, keyWord, Value))
	{
		bOk = true;
		m_RobotParaDesc.iCPSpeed = Value.toInt();
		RobotOutputParaUtil::SetRobotPara(m_RobotParaDesc);
	}
	m_bSetPara = false;
	return bOk;
}

bool EpsonRobotDriver::GetCP_Speeds(int& iSpeed)
{
	m_bGetPara = true;
	QString keyWord = gc_EXprintSpeeds;
	QString strCommand = QString("$Execute,\"print speeds;print \"%1\"\r\n").arg(keyWord);
	QVariant Value;
	bool bOk = false;
	if (ConductCommandUnit(strCommand, keyWord, Value))
	{
		iSpeed = Value.toInt();
		bOk = true;
	}
	m_bGetPara = false;
	return bOk;
}

bool EpsonRobotDriver::SetCP_Accels(int iAccel)
{
	if (iAccel <= 0 || iAccel > 25000)
		return false;
	m_bChangeToMove = false;
	m_bChangeToGo = false;
	m_bSetPara = true;
	//CP减速度永远设置为120（暂时方案，accels 需要加速度减速度一起才能设置生效）
	QString keyWord = "execute";
	QString strCommand = QString("$%1,\"Accels %2,%3\"\r\n").arg(keyWord).arg(iAccel).arg(iAccel);
	QVariant Value;
	bool bOk = false;
	if (ConductCommandUnit(strCommand, keyWord, Value))
	{
		bOk = true;
		m_RobotParaDesc.iCPAccel = Value.toInt();
		RobotOutputParaUtil::SetRobotPara(m_RobotParaDesc);
	}
	m_bSetPara = false;
	return bOk;
}

bool EpsonRobotDriver::GetCP_Accels(int& iAccel)
{
	m_bGetPara = true;
	QString keyWord = gc_EXprintAccels;
	QString strCommand = QString("$Execute,\"print Accels(1);print \"%1\"\r\n").arg(keyWord);
	QVariant Value;
	bool bOk = false;
	if (ConductCommandUnit(strCommand, keyWord, Value))
	{
		iAccel = Value.toInt();
		bOk = true;
	}
	m_bGetPara = false;
	return bOk;
}

bool EpsonRobotDriver::SetR_Speedr(int iSpeed)
{
	if (iSpeed <= 0 || iSpeed > 1000)
		return false;
	m_bChangeToMove = false;
	m_bChangeToGo = false;
	m_bSetPara = true;
	QString keyWord = "execute";
	QString strCommand = QString("$%1,\"Speedr %2\"\r\n").arg(keyWord).arg(iSpeed);
	QVariant Value;
	bool bOk = false;
	if (ConductCommandUnit(strCommand, keyWord, Value))
	{
		bOk = true;
		m_RobotParaDesc.iRSpeed = Value.toInt();
		RobotOutputParaUtil::SetRobotPara(m_RobotParaDesc);
	}
	m_bSetPara = false;
	return bOk;
}

bool EpsonRobotDriver::GetR_Speedr(int& iSpeed)
{
	m_bGetPara = true;
	QString keyWord = gc_EXprintSpeedr;
	QString strCommand = QString("$Execute,\"print speedr;print \"%1\"\r\n").arg(keyWord);
	QVariant Value;
	bool bOk = false;
	if (ConductCommandUnit(strCommand, keyWord, Value))
	{
		iSpeed = Value.toInt();
		bOk = true;
	}
	m_bGetPara = false;
	return bOk;
}

bool EpsonRobotDriver::SetR_Accelr(int iAccel)
{
	if (iAccel <= 0 || iAccel > 5000)
		return false;
	m_bChangeToMove = false;
	m_bChangeToGo = false;
	m_bSetPara = true;
	//CP减速度永远设置为120（暂时方案，accels 需要加速度减速度一起才能设置生效）
	QString keyWord = "execute";
	QString strCommand = QString("$%1,\"Accelr %2\"\r\n").arg(keyWord).arg(iAccel);
	QVariant Value;
	bool bOk = false;
	if (ConductCommandUnit(strCommand, keyWord, Value))
	{
		bOk = true;
		m_RobotParaDesc.iRAccel = Value.toInt();
		RobotOutputParaUtil::SetRobotPara(m_RobotParaDesc);
	}
	m_bSetPara = false;
	return bOk;
}

bool EpsonRobotDriver::GetR_Accelr(int& iAccel)
{
	m_bGetPara = true;
	QString keyWord = gc_EXprintAccelr;
	QString strCommand = QString("$Execute,\"print Accelr(1);print \"%1\"\r\n").arg(keyWord);
	QVariant Value;
	bool bOk = false;
	if (ConductCommandUnit(strCommand, keyWord, Value))
	{
		iAccel = Value.toInt();
		bOk = true;
	}
	m_bGetPara = false;
	return bOk;
}

bool EpsonRobotDriver::GoBasePoint()
{
	if (!m_strRobotStatus.iMotorOn)
		return false;
	m_bSetPara = true;
	QString keyWord = "execute";
	QString strCommand = QString("$%1,\"pulse 0,0,0,0,0,0\"\r\n").arg(keyWord);
	QVariant Value;
	bool bOk = false;
	if (ConductCommandUnit(strCommand, keyWord, Value))
		bOk = true;
	m_bSetPara = false;
	return bOk;
}

bool EpsonRobotDriver::GoHome()
{
	if (!m_strRobotStatus.iMotorOn)
		return false;
	m_bSetPara = true;
	QString keyWord = "execute";
	QString strCommand = QString("$%1,\"home\"\r\n").arg(keyWord);
	QVariant Value;
	bool bOk = false;
	if (ConductCommandUnit(strCommand, keyWord, Value))
		bOk = true;
	m_bSetPara = false;
	return bOk;
}

bool EpsonRobotDriver::Execute_Go(QString strPosVar)
{
	m_bStopUnexpected = false;
	if (!m_strRobotStatus.iMotorOn)
		return false;
	m_bSetPara = true;
	QString keyWord = "execute";
	QString strCommand = QString("$%1,\"go %2\"\r\n").arg(keyWord).arg(strPosVar);
	QVariant Value;
	bool bOk = false;
	if (ConductCommandUnit(strCommand, keyWord, Value))
		bOk = true;
	m_bSetPara = false;

	return bOk;
}

bool EpsonRobotDriver::Execute_Pass(QString strPosVar)
{
	m_bStopUnexpected = false;
	if (!m_strRobotStatus.iMotorOn)
		return false;
	m_bSetPara = true;
	QString keyWord = "execute";
	QString strCommand = QString("$%1,\"pass %2\"\r\n").arg(keyWord).arg(strPosVar);
	QVariant Value;
	bool bOk = false;
	if (ConductCommandUnit(strCommand, keyWord, Value))
		bOk = true;
	m_bSetPara = false;
	return bOk;
}


bool EpsonRobotDriver::Execute_Move(QString strPosVar)
{
	m_bStopUnexpected = false;
	if (!m_strRobotStatus.iMotorOn)
		return false;
	m_bSetPara = true;
	QString keyWord = "execute";
	QString strCommand = QString("$%1,\"move %2\"\r\n").arg(keyWord).arg(strPosVar);
	QVariant Value;
	bool bOk = false;
	if (ConductCommandUnit(strCommand, keyWord, Value))
		bOk = true;
	m_bSetPara = false;
	return bOk;
}

bool EpsonRobotDriver::On_OutputIO(int iIObit)
{
	bool bReadOk = false;
	int iStatus = -1;
	bReadOk = Oport_OutputIO(iIObit, iStatus);
	if (bReadOk)
	{
		if (1 == iStatus)
			return true;
	}
	else
		return false;
	m_bSetPara = true;
	QString keyWord = "execute";
	QString strCommand = QString("$%1,\"On %2\"\r\n").arg(keyWord).arg(iIObit);
	QVariant Value;
	bool bOk = false;
	if (ConductCommandUnit(strCommand, keyWord, Value))
		bOk = true;
	m_bSetPara = false;
	bReadOk = false;
	if (bOk)
	{
		iStatus = -1;
		bReadOk = Oport_OutputIO(iIObit, iStatus);
		if (bReadOk)
			if (1 == iStatus)
				bReadOk = true;
			else
				bReadOk = false;
	}
	return bOk && bReadOk;
}

bool EpsonRobotDriver::On_OutputIO(QString strIOName)
{
	bool bReadOk = false;
	int iStatus = -1;
	bReadOk = Oport_OutputIO(strIOName, iStatus);
	if (bReadOk)
	{
		if (1 == iStatus)
			return true;
	}
	else
		return false;
	m_bSetPara = true;
	QString keyWord = "execute";
	QString strCommand = QString("$%1,\"On %2\"\r\n").arg(keyWord).arg(strIOName);
	QVariant Value;
	bool bOk = false;
	if (ConductCommandUnit(strCommand, keyWord, Value))
		bOk = true;
	m_bSetPara = false;
	bReadOk = false;
	if (bOk)
	{
		iStatus = -1;
		bReadOk = Oport_OutputIO(strIOName, iStatus);
		if (bReadOk)
			if (1 == iStatus)
				bReadOk = true;
			else
				bReadOk = false;
	}
	return bOk && bReadOk;
}

bool EpsonRobotDriver::Off_OutputIO(int iIObit)
{
	bool bReadOk = false;
	int iStatus = -1;
	bReadOk = Oport_OutputIO(iIObit, iStatus);
	if (bReadOk)
	{
		if (0 == iStatus)
			return true;
	}
	else
		return false;
	m_bSetPara = true;
	QString keyWord = "execute";
	QString strCommand = QString("$%1,\"Off %2\"\r\n").arg(keyWord).arg(iIObit);
	QVariant Value;
	bool bOk = false;
	if (ConductCommandUnit(strCommand, keyWord, Value))
		bOk = true;
	m_bSetPara = false;
	bReadOk = false;
	if (bOk)
	{
		iStatus = -1;
		bReadOk = Oport_OutputIO(iIObit, iStatus);
		if (bReadOk)
			if (0 == iStatus)
				bReadOk = true;
			else
				bReadOk = false;
	}
	return bOk && bReadOk;
}

bool EpsonRobotDriver::Off_OutputIO(QString strIOName)
{
	bool bReadOk = false;
	int iStatus = -1;
	bReadOk = Oport_OutputIO(strIOName, iStatus);
	if (bReadOk)
	{
		if (0 == iStatus)
			return true;
	}
	else
		return false;
	m_bSetPara = true;
	QString keyWord = "execute";
	QString strCommand = QString("$%1,\"Off %2\"\r\n").arg(keyWord).arg(strIOName);
	QVariant Value;
	bool bOk = false;
	if (ConductCommandUnit(strCommand, keyWord, Value))
		bOk = true;
	m_bSetPara = false;
	bReadOk = false;
	if (bOk)
	{
		iStatus = -1;
		bReadOk = Oport_OutputIO(strIOName, iStatus);
		if (bReadOk)
			if (0 == iStatus)
				bReadOk = true;
			else
				bReadOk = false;
	}
	return bOk && bReadOk;
}

bool EpsonRobotDriver::Oport_OutputIO(int iIObit, int& iStatus)
{
	m_bGetPara = true;
	QString keyWord = QString("PrintOutputIO_%1").arg(iIObit);
	QString strCommand = QString("$Execute,\"print Oport(%1);print \"%2\"\r\n").arg(iIObit).arg(keyWord);
	QVariant Value;
	bool bOk = false;
	if (ConductCommandUnit(strCommand, keyWord, Value))
	{
		iStatus = Value.toInt();
		bOk = true;
	}
	m_bGetPara = false;
	return bOk;
}

bool EpsonRobotDriver::Oport_OutputIO(QString strIOName, int& iStatus)
{
	m_bGetPara = true;
	QString keyWord = QString("PrintOutputIO_%1").arg(strIOName);
	QString strCommand = QString("$Execute,\"print Oport(%1);print \"%2\"\r\n").arg(strIOName).arg(keyWord);
	QVariant Value;
	bool bOk = false;
	if (ConductCommandUnit(strCommand, keyWord, Value))
	{
		iStatus = Value.toInt();
		bOk = true;
	}
	m_bGetPara = false;
	return bOk;
}

bool EpsonRobotDriver::Oport_OutputIO_Auto(int iIObit, int& iStatus)
{
	QString keyWord = QString("PrintOutputIO_%1").arg(iIObit);
	QString strCommand = QString("$Execute,\"print Oport(%1);print \"%2\"\r\n").arg(iIObit).arg(keyWord);
	QVariant Value;
	bool bOk = false;
	m_bAtAction = true;
	if (ConductCommandUnit(strCommand, keyWord, Value))
	{
		iStatus = Value.toInt();
		bOk = true;
	}
	m_bAtAction = false;
	return bOk;
}

bool EpsonRobotDriver::Oport_OutputIO_Auto(QString strIOName, int& iStatus)
{
	QString keyWord = QString("PrintOutputIO_%1").arg(strIOName);
	QString strCommand = QString("$Execute,\"print Oport(%1);print \"%2\"\r\n").arg(strIOName).arg(keyWord);
	QVariant Value;
	bool bOk = false;
	m_bAtAction = true;
	if (ConductCommandUnit(strCommand, keyWord, Value))
	{
		iStatus = Value.toInt();
		bOk = true;
	}
	m_bAtAction = false;
	return bOk;
}

bool EpsonRobotDriver::Sw_InputIO(int iIObit, int &iStatus)
{
	m_bGetPara = true;
	QString keyWord = "PrintInputIO";
	QString strCommand = QString("$Execute,\"print Sw(%1);print \"%2\"\r\n").arg(iIObit).arg(keyWord);
	QVariant Value;
	bool bOk = false;
	if (ConductCommandUnit(strCommand, keyWord, Value))
	{
		iStatus = Value.toInt();
		bOk = true;
	}
	m_bGetPara = false;
	return bOk;
}

bool EpsonRobotDriver::Sw_InputIO(QString strIOName, int &iStatus)
{
	m_bGetPara = true;
	QString keyWord = "PrintInputIO";
	QString strCommand = QString("$Execute,\"print Sw(%1);print \"%2\"\r\n").arg(strIOName).arg(keyWord);
	QVariant Value;
	bool bOk = false;
	if (ConductCommandUnit(strCommand, keyWord, Value))
	{
		iStatus = Value.toInt();
		bOk = true;
	}
	m_bGetPara = false;
	return bOk;
}

bool EpsonRobotDriver::Sw_InputIO_Auto(int iIObit, int& iStatus)
{
	QString keyWord = "PrintInputIO";
	QString strCommand = QString("$Execute,\"print Sw(%1);print \"%2\"\r\n").arg(iIObit).arg(keyWord);
	QVariant Value;
	if (ConductCommandUnit(strCommand, keyWord, Value))
	{
		iStatus = Value.toInt();
		return true;
	}
	return false;
}

bool EpsonRobotDriver::Sw_InputIO_Auto(QString strIOName, int& iStatus)
{
	QString keyWord = "PrintInputIO";
	QString strCommand = QString("$Execute,\"print Sw(%1);print \"%2\"\r\n").arg(strIOName).arg(keyWord);
	QVariant Value;
	bool bOk = false;
	if (ConductCommandUnit(strCommand, keyWord, Value))
	{
		iStatus = Value.toInt();
		bOk = true;
	}
	return bOk;
}

//@type 0:X 1:Y 2:Z 3:U 4:V 5:W
bool EpsonRobotDriver::Execute_ConsecutiveSport(RobotParaType::RobotSportMode mode)
{
	QString strCommand;
	//X
	if(mode.iType == 0 && mode.iDirection == 0)
		strCommand = QString("$execute,\"%1 Here -X(%2) LJM\"\r\n").arg(mode.strMode).arg(mode.iStep);
	else if (mode.iType == 0 && mode.iDirection == 1)
		strCommand = QString("$execute,\"%1 Here +X(%2) LJM\"\r\n").arg(mode.strMode).arg(mode.iStep);
	//Y
	else if (mode.iType == 1 && mode.iDirection == 0)
		strCommand = QString("$execute,\"%1 Here -Y(%2) LJM\"\r\n").arg(mode.strMode).arg(mode.iStep);
	else if (mode.iType == 1 && mode.iDirection == 1)
		strCommand = QString("$execute,\"%1 Here +Y(%2) LJM\"\r\n").arg(mode.strMode).arg(mode.iStep);
	//Z
	else if (mode.iType == 2 && mode.iDirection == 0)
		strCommand = QString("$execute,\"%1 Here -Z(%2) LJM\"\r\n").arg(mode.strMode).arg(mode.iStep);
	else if (mode.iType == 2 && mode.iDirection == 1)
		strCommand = QString("$execute,\"%1 Here +Z(%2) LJM\"\r\n").arg(mode.strMode).arg(mode.iStep);
	//U
	else if (mode.iType == 3 && mode.iDirection == 0)
		strCommand = QString("$execute,\"%1 Here -U(%2) LJM\"\r\n").arg(mode.strMode).arg(mode.iStep);
	else if (mode.iType == 3 && mode.iDirection == 1)
		strCommand = QString("$execute,\"%1 Here +U(%2) LJM\"\r\n").arg(mode.strMode).arg(mode.iStep);
	//V
	else if (mode.iType == 4 && mode.iDirection == 0)
		strCommand = QString("$execute,\"%1 Here -V(%2) LJM\"\r\n").arg(mode.strMode).arg(mode.iStep);
	else if (mode.iType == 4 && mode.iDirection == 1)
		strCommand = QString("$execute,\"%1 Here +V(%2) LJM\"\r\n").arg(mode.strMode).arg(mode.iStep);
	//W
	else if (mode.iType == 5 && mode.iDirection == 0)
		strCommand = QString("$execute,\"%1 Here -W(%2) LJM\"\r\n").arg(mode.strMode).arg(mode.iStep);
	else if (mode.iType == 5 && mode.iDirection == 1)
		strCommand = QString("$execute,\"%1 Here +W(%2) LJM\"\r\n").arg(mode.strMode).arg(mode.iStep);

	QString keyWord = "execute";
	QVariant Value;
	bool bOk = false;
	m_bSetPara = true;
	m_bAtAction = true;
	if (ConductCommandUnit(strCommand, keyWord, Value))
		bOk = true;
	m_bAtAction = false;
	m_bSetPara = false;

	return bOk;
}

bool EpsonRobotDriver::IsMoveToPositionSuccess(QString strPosSign)
{
	m_bPythonMove = true;
	if (m_bEmergencyStop || m_bEnd)
		return false;
	QString keyWord = "RobotPosHere";
	QString strCommand = QString("$Execute,\"print here;print \"%1\"\r\n").arg(keyWord);
	QVariant Value;
	RobotParaType::RobotPoint robotPosHere,robotPosSign;
	bool bOk = false;
	m_bGetPara = true;
	
	if (ConductCommandUnit(strCommand, keyWord, Value))
	{
		robotPosHere = Value.value<RobotParaType::RobotPoint>();
		keyWord = "RobotPosSign";
		strCommand = QString("$Execute,\"print %1;print \"%2\"\r\n").arg(strPosSign).arg(keyWord);
		if (ConductCommandUnit(strCommand, keyWord, Value))
		{
			robotPosSign = Value.value<RobotParaType::RobotPoint>();
			if (robotPosHere == robotPosSign)
				bOk = true;
		}
	}
	if (bOk)
	{
		if (strPosSign == gc_TferWait)
			m_bAtTferWait = true;
		IsInSafetyWorkSpace();
	}
	m_bGetPara = false;
	m_bPythonMove = false;
	return bOk;
}

// 判断机械臂是否满足移动条件
bool EpsonRobotDriver::IsMotorReadyMove()
{
	if (m_bEmergencyStop || m_strRobotStatus.iMotorOn == 0 || m_strRobotStatus.iEstopOn == 1 ||!m_bRobotConnect)
		return false;
	return true;
}

bool EpsonRobotDriver::IsMoveStopUnexpected()
{
	return m_bStopUnexpected;
}

// 判断在安全工作空间
bool EpsonRobotDriver::IsInSafetyWorkSpace()
{
	if (m_bAtTferWait)
	{
		m_bAtSafetyWorkSpace = true;
		m_strRobotStatus.iSafetySpace = true;
		RobotOutputParaUtil::SetRobotStatus(m_strRobotStatus);
		RobotOutputParaUtil::SetRobotSafetyState(true);
		m_pSchedulerDriver->UpdateRobotSafetyStatus(true, true);
		return true;
	}
	QString Value = "";
	if (!DeviceDetailCfg::Instance()->GetCfgValue("RobotCfg/CoaterInsideBoxIO", Value))
		return false;
	int iCoaterStatus = -1;
	if (!Oport_OutputIO_Auto(Value.toInt(), iCoaterStatus))
	{
		return false;
	}
	if (!DeviceDetailCfg::Instance()->GetCfgValue("RobotCfg/ImagerInsideBoxIO", Value))
		return false;
	int iImagerStatus = -1;
	if (!Oport_OutputIO_Auto(Value.toInt(), iImagerStatus))
	{
		return false;
	}
	bool AtSafetyWorkSpace = false;
	if (1 == iCoaterStatus && 1 == iImagerStatus)
		AtSafetyWorkSpace = true;
	//if(m_bGetPara)
	//	ErrorCodeTranslate::Instance()->RecordInfo("EpsonRobotDriver", QString("AtSafetyWorkSpace %1").arg(AtSafetyWorkSpace), gc_colorBlue, true);
	if (m_bAtSafetyWorkSpace != AtSafetyWorkSpace) //有变化发出信号
	{
		m_bAtSafetyWorkSpace = AtSafetyWorkSpace;
		m_strRobotStatus.iSafetySpace = AtSafetyWorkSpace;
		RobotOutputParaUtil::SetRobotStatus(m_strRobotStatus);
		RobotOutputParaUtil::SetRobotSafetyState(AtSafetyWorkSpace);
		m_pSchedulerDriver->UpdateRobotSafetyStatus(1 == iCoaterStatus, 1 == iImagerStatus);
	}
	return AtSafetyWorkSpace;
}

bool EpsonRobotDriver::IsMotorPowerHigh()
{
	return 1 == m_strRobotStatus.iPower ? true : false;
}

void EpsonRobotDriver::CloseRobotSocket()
{
	Logout();
	emit SignalMasterCloseRobotConnect();
}

// 返回机械臂网络连接状态
bool EpsonRobotDriver::GetConnectStatus()
{
	return m_bRobotConnect;
}

void EpsonRobotDriver::TestShowMsgBox()
{
	BGI_MESSAGE("我是一个测试对话框");
	SetMotorsOff();
}

// 判断夹手打开是否成功
bool EpsonRobotDriver::IsOpenGripperSuccess()
{
	QString strOpenIO = "";
	if (!DeviceDetailCfg::Instance()->GetCfgValue("RobotCfg/OpenGripper_InIO", strOpenIO))
		return false;
	int count  = 0;
	while (count < 20)
	{
		int iStatus = -1;
		if (Sw_InputIO(strOpenIO.toInt(), iStatus))
		{
			if (1 == iStatus)
				return true;
		}
		count++;
	}
	return false;
}

// 打开夹爪
bool EpsonRobotDriver::OpenGripper()
{
	QString strOpenIO = "";
	QString strCloseIO = "";
	if (!DeviceDetailCfg::Instance()->GetCfgValue("RobotOutIO/GriperSwitchOpen", strOpenIO))
		return false;
	if (!DeviceDetailCfg::Instance()->GetCfgValue("RobotOutIO/GriperSwitchClose", strCloseIO))
		return false;
	int iCount = 1;
	int iTotalCount = gc_GetParaTimes;
	while (iCount <= iTotalCount)
	{
		bool bOk = false;
		bOk = Off_OutputIO(strCloseIO.toInt());
		if (bOk)
			break;
		else
		{
			if (iTotalCount == iCount)
			{
				ErrorCodeTranslate::Instance()->RecordInfo("EpsonRobotDriver", QString("第a%1次打开夹手失败！").arg(iCount + 1), gc_colorYellow);
				return false;
			}
		}
		iCount ++;
	}
	iCount = 1;
	while (iCount <= iTotalCount)
	{
		bool bOk = false;
		bOk = On_OutputIO(strOpenIO.toInt());
		if (bOk)
		{
			ErrorCodeTranslate::Instance()->RecordInfo("EpsonRobotDriver", "打开夹手成功！", gc_colorBlack);
			break;
		}
		else
		{
			if (iTotalCount == iCount)
			{
				ErrorCodeTranslate::Instance()->RecordInfo("EpsonRobotDriver", QString("第b%1次打开夹手失败！").arg(iCount + 1), gc_colorYellow);
				return false;
			}
		}
		iCount++;
	}
	if (IsOpenGripperSuccess())
		return true;
	return false;
}

// 判断是否夹取芯片成功
bool EpsonRobotDriver::IsGripChipSuccess()
{
	QString strCloseIO = "";
	if (!DeviceDetailCfg::Instance()->GetCfgValue("RobotCfg/CloseGripper_InIO", strCloseIO))
		return false;
	int count = 0;
	while (count < 100)
	{
		int iStatus = -1;
		if (Sw_InputIO(strCloseIO.toInt(), iStatus))
		{
			if (1 == iStatus)
				return true;
		}
		count++;
	}
	return false;
}

// 关闭夹爪
bool EpsonRobotDriver::CloseGripper()
{
	QString strOpenIO = "";
	QString strCloseIO = "";
	if (!DeviceDetailCfg::Instance()->GetCfgValue("RobotOutIO/GriperSwitchOpen", strOpenIO))
		return false;
	if (!DeviceDetailCfg::Instance()->GetCfgValue("RobotOutIO/GriperSwitchClose", strCloseIO))
		return false;
	int iCount = 1;
	int iTotalCount = gc_GetParaTimes;
	while (iCount <= iTotalCount)
	{
		bool bOk = false;
		bOk = Off_OutputIO(strOpenIO.toInt());
		if (bOk)
			break;
		else
		{
			if (iTotalCount == iCount)
			{
				ErrorCodeTranslate::Instance()->RecordInfo("EpsonRobotDriver", QString("第a%1次关闭夹手失败！").arg(iCount), gc_colorYellow);
				return false;
			}
		}
		iCount++;
	}
	iCount = 1;
	while (iCount <= iTotalCount)
	{
		bool bOk = false;
		bOk = On_OutputIO(strCloseIO.toInt());
		if (bOk)
		{
			ErrorCodeTranslate::Instance()->RecordInfo("EpsonRobotDriver", "关闭夹手成功！", gc_colorBlack);
			return true;
		}
		else
		{
			if (iTotalCount == iCount)
				ErrorCodeTranslate::Instance()->RecordInfo("EpsonRobotDriver", QString("第b%1次关闭夹手失败！").arg(iCount), gc_colorYellow);
		}
		iCount++;
	}
	return false;
}

bool EpsonRobotDriver::MotorInit()
{
	return MotorSetLowSpeed();
}

bool EpsonRobotDriver::GetRobotSpeedPara(QString strKey, int iMax, int& iValue)
{
	QString Value;
	QString strAllKey = "RobotCfg/" + strKey;
	if (!DeviceDetailCfg::Instance()->GetCfgValue(strAllKey, Value))
	{
		ErrorCodeTranslate::Instance()->RecordInfo("EpsonRobotDriver", QString("配置文件未获取到 %1 的值！").arg(strKey), gc_colorYellow);
		return false;
	}
	iValue = Value.toInt();
	if (iValue > 0 && iValue <= iMax)
		return true;
	ErrorCodeTranslate::Instance()->RecordInfo("EpsonRobotDriver", QString("%1 配置项超出范围！").arg(strKey), gc_colorYellow);
	return false;
}

// 设置低速
bool EpsonRobotDriver::MotorSetLowSpeed()
{
	if (m_bChangeToMove)
		return true;
	int iValue = -1;
	int iTimes = 5;
	if (!GetRobotSpeedPara("InitPTPSpeed_Move",100, iValue))
		return false;
	ApplicationSleep(WAIT_REPLY_SLEEP* iTimes);
	if (!SetPTP_Speed(iValue))
	{
		ErrorCodeTranslate::Instance()->RecordInfo("EpsonRobotDriver", "设置InitPTPSpeed_Move 失败！", gc_colorYellow);
		return false;
	}
	if (!GetRobotSpeedPara("InitPTPAccel_Move",120, iValue))
		return false;
	ApplicationSleep(WAIT_REPLY_SLEEP* iTimes);
	if (!SetPTP_Accel(iValue))
	{
		ErrorCodeTranslate::Instance()->RecordInfo("EpsonRobotDriver", "设置InitPTPAccel_Move 失败！", gc_colorYellow);
		return false;
	}
	if (!GetRobotSpeedPara("InitCPSpeed_Move", 2000,iValue))
		return false;
	ApplicationSleep(WAIT_REPLY_SLEEP * iTimes);
	if (!SetCP_Speeds(iValue))
	{
		ErrorCodeTranslate::Instance()->RecordInfo("EpsonRobotDriver", "设置InitCPSpeed_Move 失败！", gc_colorYellow);
		return false;
	}
	if (!GetRobotSpeedPara("InitCPAccel_Move", 25000,iValue))
		return false;
	ApplicationSleep(WAIT_REPLY_SLEEP * iTimes);
	if (!SetCP_Accels(iValue))
	{
		ErrorCodeTranslate::Instance()->RecordInfo("EpsonRobotDriver", "设置InitCPAccel_Move 失败！", gc_colorYellow);
		return false;
	}
	if (!GetRobotSpeedPara("InitRSpeed_Move",1000, iValue))
		return false;
	ApplicationSleep(WAIT_REPLY_SLEEP * iTimes);
	if (!SetR_Speedr(iValue))
	{
		ErrorCodeTranslate::Instance()->RecordInfo("EpsonRobotDriver", "设置InitRSpeed_Move 失败！", gc_colorYellow);
		return false;
	}
	if (!GetRobotSpeedPara("InitRAccel_Move", 5000,iValue))
		return false;	
	ApplicationSleep(WAIT_REPLY_SLEEP * iTimes);
	if (!SetR_Accelr(iValue))
	{
		ErrorCodeTranslate::Instance()->RecordInfo("EpsonRobotDriver", "设置InitRAccel_Move 失败！", gc_colorYellow);
		return false;
	}
	m_bChangeToMove = true;
	m_bChangeToGo = false;
	return true;
}

// 设置高速
bool EpsonRobotDriver::MotorSetHighSpeed()
{
	if (m_bChangeToGo)
		return true;
	int iValue = -1;
	int iTimes = 5;
	if (!GetRobotSpeedPara("InitPTPSpeed_Go", 100,iValue))
		return false;
	if (!SetPTP_Speed(iValue))
	{
		ErrorCodeTranslate::Instance()->RecordInfo("EpsonRobotDriver", "设置InitPTPSpeed_Go 失败！", gc_colorYellow);
		return false;
	}
	if (!GetRobotSpeedPara("InitPTPAccel_Go", 120,iValue))
		return false;
	ApplicationSleep(WAIT_REPLY_SLEEP * iTimes);
	if (!SetPTP_Accel(iValue))
	{
		ErrorCodeTranslate::Instance()->RecordInfo("EpsonRobotDriver", "设置InitPTPAccel_Go 失败！", gc_colorYellow);
		return false;
	}
	if (!GetRobotSpeedPara("InitCPSpeed_Go", 2000,iValue))
		return false;
	ApplicationSleep(WAIT_REPLY_SLEEP * iTimes);
	if (!SetCP_Speeds(iValue))
	{
		ErrorCodeTranslate::Instance()->RecordInfo("EpsonRobotDriver", "设置InitCPSpeed_Go 失败！", gc_colorYellow);
		return false;
	}
	if (!GetRobotSpeedPara("InitCPAccel_Go", 25000,iValue))
		return false;
	ApplicationSleep(WAIT_REPLY_SLEEP * iTimes);
	if (!SetCP_Accels(iValue))
	{
		ErrorCodeTranslate::Instance()->RecordInfo("EpsonRobotDriver", "设置InitCPAccel_Go 失败！", gc_colorYellow);
		return false;
	}
	if (!GetRobotSpeedPara("InitRSpeed_Go", 1000,iValue))
		return false;
	ApplicationSleep(WAIT_REPLY_SLEEP * iTimes);
	if (!SetR_Speedr(iValue))
	{
		ErrorCodeTranslate::Instance()->RecordInfo("EpsonRobotDriver", "设置InitRSpeed_Go 失败！", gc_colorYellow);
		return false;
	}
	if (!GetRobotSpeedPara("InitRAccel_Go",5000, iValue))
		return false;
	ApplicationSleep(WAIT_REPLY_SLEEP * iTimes);
	if (!SetR_Accelr(iValue))
	{
		ErrorCodeTranslate::Instance()->RecordInfo("EpsonRobotDriver", "设置InitRAccel_Go 失败！", gc_colorYellow);
		return false;
	}
	m_bChangeToMove = false;
	m_bChangeToGo = true;
	return true;
}

bool EpsonRobotDriver::OperateIODeviceUnit(int iIO,bool bOpen)
{
	bool bOk = false;
	int iCount = 1;
	int iTotalCount = gc_GetParaTimes;
	while (iCount <= iTotalCount)
	{
		if (bOpen)
			bOk = On_OutputIO(iIO);
		else
			bOk = Off_OutputIO(iIO);
		if (bOk)
			return true;
		iCount++;
	}

	return false;
}

// 风刀口吹风
bool EpsonRobotDriver::BlowChip(bool bOpen)
{
	QString strBlowChipIO = "";
	if (!DeviceDetailCfg::Instance()->GetCfgValue("RobotOutIO/AirKnife", strBlowChipIO))
		return false;
	
	return OperateIODeviceUnit(strBlowChipIO.toInt(), bOpen);
}

// 操作蜂鸣器
bool EpsonRobotDriver::OperateBuzzer(bool bOpen)
{
	return true;
}

bool EpsonRobotDriver::OperateWaringLights(RobotParaType::RobotLightColor color)
{
	if (m_OperateLightsing)
		return false;
	m_OperateLightsing = true;
	bool bOk = false;
	do
	{
		if (!m_bRobotConnect)
		{
			bOk = false;
			break;
		}
		QString strIO_red = "", strIO_Green = "";
		if (!DeviceDetailCfg::Instance()->GetCfgValue("RobotOutIO/Lights_Red", strIO_red))
		{
			bOk = false;
			break;
		}
		if (!DeviceDetailCfg::Instance()->GetCfgValue("RobotOutIO/Lights_Green", strIO_Green))
		{
			bOk = false;
			break;
		}
		if (color == RobotParaType::RobotLightColor::NONE)
		{
			bOk = OperateIODeviceUnit(strIO_red.toInt(), false) && OperateIODeviceUnit(strIO_Green.toInt(), false);
			break;
		}
		else if (color == RobotParaType::RobotLightColor::RED)
		{
			bOk = OperateIODeviceUnit(strIO_red.toInt(), true) && OperateIODeviceUnit(strIO_Green.toInt(), false);
			break;
		}
		else if (color == RobotParaType::RobotLightColor::GREEN)
		{
			bOk = OperateIODeviceUnit(strIO_red.toInt(), false) && OperateIODeviceUnit(strIO_Green.toInt(), true);
			break;
		}
		else if (color == RobotParaType::RobotLightColor::YELLO)
		{
			bOk = OperateIODeviceUnit(strIO_red.toInt(), true) && OperateIODeviceUnit(strIO_Green.toInt(), true);
			break;
		}
	} while (0);
	
	m_OperateLightsing = false;
	if(bOk)
		return true;
	return false;
}

// 判断中转槽是否有芯片
bool EpsonRobotDriver::HaveChipInTransferTank(int iSlotID)
{
	QString strChipSlotIO = "";
	if (!DeviceDetailCfg::Instance()->GetCfgValue(QString("RobotCfg/ChipSlot%1_InIO").arg(iSlotID), strChipSlotIO))
	{
		ErrorCodeTranslate::Instance()->RecordInfo("EpsonRobotDriver", "获取ChipSlotIO 失败！", gc_colorYellow);
		return true;
	}
	int iSum = 0;
	int count = 0;
	bool bReadOk = false;
	while (count < 5)
	{
		int iStatus = -1;
		if (Sw_InputIO(strChipSlotIO.toInt(), iStatus))
		{
			iSum += iStatus;
			bReadOk = true;
		}
		count++;
	}
	if (bReadOk)
	{
		if (iSum == 0)
		{
			ErrorCodeTranslate::Instance()->RecordInfo("EpsonRobotDriver", QString("判断结果：中转槽%1 没有芯片！").arg(iSlotID), gc_colorBlack);
			return false;
		}
	}
	ErrorCodeTranslate::Instance()->RecordInfo("EpsonRobotDriver", QString("判断结果：中转槽%1 有芯片！").arg(iSlotID), gc_colorBlack);
	return true;
}