#include "SchedulerDriver.h"
#include "BgiLogger.h"
#include <QThread>
#include <QtConcurrentRun>
#include "MasterDevices.h"
#include "ImagerDriver.h"
#include "EpsonRobotFlowCtrl.h"
#include "CoaterDriver.h"
#include "CommonColors.h"
#include "DeviceDetailCfg.h"
#include "ErrorCodeTranslate.h"
#include "DeviceWarningLights.h"


#define CATCHTASKEXCEPTION(x)\
	catch (const QString& ex)\
	{\
		QString str = "do task " + (QString)x + " exception: " + ex;\
		BGI_ERROR(str.toStdString().c_str());\
		if (ex == "StopTask")\
		{\
			throw;\
		}\
		return SchedulerTaskStateEnum::StopTask;\
	}\
	catch (...)\
	{\
		QString str = "do task " + (QString)x + " unknown exception!";\
		BGI_ERROR(str.toStdString().c_str());\
		return SchedulerTaskStateEnum::StopTask;\
	}

SchedulerDriver::SchedulerDriver(QObject* parent)
	: QObject(parent)
	, m_NextTask(SchedulerTaskStateEnum::StopTask)
	, m_PauseState(false)
	, m_EndCycle(0)
	, m_bFirstConfigSet(true)
	, m_bImagerFinished(false)
	, m_bImagerRetryError(false)
	, m_RecycleChip(false)
	, m_bVacuumOpenFailed(false)
	, m_bImagerMemFailed(false)
	, m_CoatingSucceed(true)
	, m_CoaterUnloading(false)
{
	BGI_MESSAGE("SchedulerDriver init begin ...");

	m_pImagerDriver = MasterDevices::Instance()->GetImagerDriver();
	m_pEpsonRobotFlowCtrl = EpsonRobotFlowCtrl::Instance();
	m_pCoaterDriver = MasterDevices::Instance()->GetCoaterDriver();
	m_pDeviceWarningLights = new DeviceWarningLights();

	QObject::connect(m_pImagerDriver, SIGNAL(SignalImagerData(const QString&, const QString&)), this, SLOT(SlotImagerData(const QString&, const QString&)));
	QObject::connect(m_pCoaterDriver, SIGNAL(SignalNewAlarm(QString)), this, SLOT(SlotCoaterAlarm(QString)));
	QObject::connect(m_pCoaterDriver, SIGNAL(SignalCoatingResult(int)), this, SLOT(SlotCoatingResult(int)));

	ReadConfigValue("ImagerCfg/VacuumTime", m_ImagerVacuumTimeout, 20);
	ReadConfigValue("ImagerCfg/VacuumMaxValue", m_ImagerVacuumMaxValue, -40);
	ReadConfigValue("ImagerCfg/VacuumMinValue", m_ImagerVacuumMinValue, -20);
	ReadConfigValue("ImagerCfg/MemoryTime", m_MemoryTimeout, 600);
	ReadConfigValue("ImagerCfg/ImagingTime", m_ImagingTimeout, 900);
	ReadConfigValue("ImagerCfg/RetryCount", m_ImagerRetryMaxCount, 2);
	ReadImagerRetryErrors();

	ReadConfigValue("CoaterCfg/VacuumTime", m_CoaterVacuumTimeout, 20);
	ReadConfigValue("CoaterCfg/VacuumMaxValue", m_CoaterVacuumMaxValue, -40);
	ReadConfigValue("CoaterCfg/VacuumMinValue", m_CoaterVacuumMinValue, -20);
	ReadConfigValue("CoaterCfg/CoatingTime", m_CoatingTimeout, 900);
	ReadConfigValue("CoaterCfg/LoadTime", m_LoadTimeout, 600);
	ReadConfigValue("CoaterCfg/UnloadTime", m_UnloadTimeout, 600);

	int cfgValue = 0;
	ReadConfigValue("GeneralCfg/SimulatedRobot", cfgValue, 0);
	m_IsRobotSimulated = (cfgValue == 1);
	ReadConfigValue("GeneralCfg/SimulatedCoater", cfgValue, 0);
	m_IsCoaterSimulated = (cfgValue == 1);
	ReadConfigValue("GeneralCfg/SimulatedImager", cfgValue, 0);
	m_IsImagerSimulated = (cfgValue == 1);

	m_pThread = new QThread();
	m_pThread->start(QThread::NormalPriority);
	this->moveToThread(m_pThread);

	/*m_pTimer = new QTimer();
	QObject::connect(m_pTimer, SIGNAL(timeout()), this, SLOT(HandleTask()), Qt::DirectConnection);
	m_pTimer->start(1000);
	m_pTimer->moveToThread(m_pThread);*/

	BGI_MESSAGE("SchedulerDriver init end");
}

SchedulerDriver::~SchedulerDriver()
{
	SwitchTaskStateDirect(SchedulerTaskStateEnum::StopTask);
	if (NULL != m_pDeviceWarningLights)
	{
		m_pDeviceWarningLights->Stop();
		m_pDeviceWarningLights->wait();
		delete m_pDeviceWarningLights;
		m_pDeviceWarningLights = NULL;
	}
}

void SchedulerDriver::ReadConfigValue(const QString& key, int& value, int defaultValue)
{
	value = defaultValue;

	QString str = "";
	if (DeviceDetailCfg::Instance()->GetCfgValue(key, str))
	{
		bool bRet = false;
		int i = str.toInt(&bRet);
		if (bRet)
		{
			value = i;
		}
	}
}

void SchedulerDriver::ReadImagerRetryErrors()
{
	m_ImagerRetryErrors.clear();
	QString content = "";
	if (DeviceDetailCfg::Instance()->GetCfgValue("ImagerCfg/RetryErrors", content))
	{
		auto strs = content.split(' ', Qt::SkipEmptyParts);
		foreach(auto str, strs)
		{
			bool bRet = false;
			int code = str.toInt(&bRet);
			if (bRet)
			{
				if (!m_ImagerRetryErrors.contains(code))
				{
					m_ImagerRetryErrors.insert(code);
				}
			}
		}
	}
}

bool SchedulerDriver::CheckRobotready()
{
	if (m_IsRobotSimulated)
		return true;
	if (!m_pEpsonRobotFlowCtrl->HaveChipInTransferTank(1))
		return false;
	if (!m_pEpsonRobotFlowCtrl->IsMotorReadyMove())
	{
		ErrorCodeTranslate::Instance()->ShowErrinfoDetail("R_9001", InfoType::RobotType);
		return false;
	}
	return true;
}

//void SchedulerDriver::HandleTask()
//{
//	//BGI_MESSAGE("HandleTask");
//}

void SchedulerDriver::StartRun(QString barcode)
{
	QString str = "SchedulerDriver StartRun " + barcode;
	BGI_MESSAGE(str.toStdString().c_str());

	if (!CheckRobotready())
	{
		SwitchTaskStateDirect(SchedulerTaskStateEnum::StopTask);
		emit SignalSchedulerTaskEnd();
		//亮红灯
		OpenAlarmLight(RobotParaType::RobotLightColor::RED);
		return;
	}

	if (m_PauseState)
	{
		//暂停恢复
		m_PauseState = false;
		ReportInfo(RecordLevelEnum::Note, "任务已恢复！");
	}
	else
	{
		//全新开始
		if (!OperateSqliteConfig::Instance()->QureyCfgDataByBarcode(barcode, m_CurChipData))
		{
			ReportInfo(RecordLevelEnum::Error, "查询不到芯片信息，无法开始任务！");
			emit SignalSchedulerTaskEnd();
			return;
		}

		m_bFirstConfigSet = true;  //每次重新开始都重设一次光机配置
		m_RecycleChip = false;

		SwitchTaskStateDirect(SchedulerTaskStateEnum::StartTask);
		RunSchedulerTask();
	}
}

void SchedulerDriver::PauseRun()
{
	BGI_MESSAGE("SchedulerDriver PauseRun");
	m_PauseState = true;
	ReportInfo(RecordLevelEnum::Note, "接收到暂停！将在芯片放回中转槽后暂停");
}

void SchedulerDriver::StopRun()
{
	BGI_MESSAGE("SchedulerDriver StopRun");

	if (m_NextTask != SchedulerTaskStateEnum::StopTask)
	{
		SchedulerTaskStateEnum lastTask = m_NextTask;
		SwitchTaskStateDirect(SchedulerTaskStateEnum::StopTask);

		//通知机械臂停止
		try
		{
			ReportInfo(RecordLevelEnum::Note, "通知机械臂停止");
			m_pEpsonRobotFlowCtrl->EmergencyStop();
			OpenAlarmLight(RobotParaType::RobotLightColor::RED);
		}
		catch (...)
		{
		}

		//通知涂布机停止
		try
		{
			if (lastTask == SchedulerTaskStateEnum::Coating)
			{
				ReportInfo(RecordLevelEnum::Note, "通知涂布机停止");
				m_pCoaterDriver->Stop();
			}
		}
		catch (...)
		{
		}
		//通知光机停止
		//光机目前没有停止接口
	}
}

void SchedulerDriver::SetEndCycle(int cycle)
{
	QString str = "SchedulerDriver SetEndCycle " + QString::number(cycle);
	BGI_MESSAGE(str.toStdString().c_str());

	m_EndCycle = cycle;
	ReportInfo(RecordLevelEnum::Note, "回收Cycle已设置为" + QString::number(cycle));
}

int SchedulerDriver::GetEndCycle()
{
	return m_EndCycle;
}

void SchedulerDriver::UpdateRobotSafetyStatus(bool bCoaterSafety, bool bImagerSafety)
{
	try
	{
		m_pImagerDriver->UpdateRobotSafetyStatus(bImagerSafety);
	}
	catch (...)
	{
		BGI_ERROR("Update Imager Safety Status exception!");
	}

	try
	{
		m_pCoaterDriver->UpdateRobotSafetyStatus(bCoaterSafety);

		if (bCoaterSafety && m_CoaterUnloading)
		{
			m_CoaterUnloading = false;
			//通知涂布机下料完成
			if (!FunMonitor(&SchedulerDriver::CoaterSetUnloadState, false))
			{
				//如果失败重试一次，失败也可以继续，主要看下次的上料状态
				ReportInfo(RecordLevelEnum::Warning, "重试一次");
				FunMonitor(&SchedulerDriver::CoaterSetUnloadState, false);
			}
		}
	}
	catch (...)
	{
		BGI_ERROR("Update Coater Safety Status exception!");
	}
}

void SchedulerDriver::SwitchTaskState(SchedulerTaskStateEnum state)
{
	if (m_NextTask != SchedulerTaskStateEnum::StopTask)
	{
		QString str = "SwitchTaskState: " + QString("%1").arg(state);
		BGI_MESSAGE(str.toStdString().c_str());

		m_NextTask = state;
	}
}

void SchedulerDriver::SwitchTaskStateDirect(SchedulerTaskStateEnum state)
{
	QString str = "SwitchTaskState Direct: " + QString("%1").arg(state);
	BGI_MESSAGE(str.toStdString().c_str());

	m_NextTask = state;
}

void SchedulerDriver::RecordInfo(bool bSucceed, const QString& info)
{
	if (bSucceed)
	{
		ReportInfo(RecordLevelEnum::Info, info + "完成");
	}
	else
	{
		ReportInfo(RecordLevelEnum::Error, info + "失败！");
	}
}

void SchedulerDriver::ReportInfo(RecordLevelEnum level, const QString& info)
{
	m_RecordData.m_strInfoType = "调度";
	m_RecordData.m_bWrite = true;
	m_RecordData.m_strContent = info;
	switch (level)
	{
	case Info:
		m_RecordData.m_color = gc_colorBlack;
		BGI_MESSAGE(info.toStdString().c_str());
		break;
	case Note:
		m_RecordData.m_color = gc_colorBlue;
		BGI_MESSAGE(info.toStdString().c_str());
		break;
	case Warning:
		m_RecordData.m_color = gc_colorYellow;
		BGI_WARNING(info.toStdString().c_str());
		break;
	case Error:
		m_RecordData.m_color = gc_colorRed;
		BGI_ERROR(info.toStdString().c_str());
		break;
	default:
		m_RecordData.m_color = gc_colorBlack;
		BGI_MESSAGE(info.toStdString().c_str());
		break;
	}
	OperateRecordInfo::AddRecordData(m_RecordData);
}

void SchedulerDriver::RunSchedulerTask()
{
	QFuture<void> f1 = QtConcurrent::run([=]() 
		{
			SchedulerTaskStateEnum curTask = SchedulerTaskStateEnum::StopTask;
			while (true)
			{
				try
				{
					if (m_NextTask == SchedulerTaskStateEnum::StopTask)
					{
						//任务结束
						DoEndTask();
						break;
					}
					if (curTask == m_NextTask)
					{
						//无新任务
						QThread::msleep(1000);
						continue;
					}
					if (m_PauseState && (m_NextTask == SchedulerTaskStateEnum::LoadToCoater || m_NextTask == SchedulerTaskStateEnum::LoadToImager))
					{
						//任务只在上料前暂停
						QThread::msleep(1000);
						continue;
					}

					//有新任务
					QString str = "RunSchedulerTask find new task: " + QString("%1").arg(m_NextTask);
					BGI_MESSAGE(str.toStdString().c_str());

					curTask = m_NextTask;
					switch (curTask)
					{
					case StartTask:
						//任务开始
						SwitchTaskState(DoInitPosTask());
						break;
					case LoadToCoater:
						//上料到涂布机
						SwitchTaskState(DoLoadToCoaterTask());
						break;
					case Coating:
						//涂布
						if (DoCoatingTask())
						{
							//成功则转到下一状态
							SwitchTaskState(SchedulerTaskStateEnum::UnloadFromCoater);
						}
						else
						{
							//失败则进行芯片回收
							ReportInfo(RecordLevelEnum::Warning, "进行芯片回收");
							m_RecycleChip = true;
							SwitchTaskState(SchedulerTaskStateEnum::UnloadFromCoater);
						}
						break;
					case UnloadFromCoater:
						//从涂布机下料
						SwitchTaskState(DoUnloadFromCoaterTask());
						break;
					case LoadToImager:
						//上料到光机
						SwitchTaskState(DoLoadToImagerTask());
						break;
					case Imaging:
						//拍照
						if (DoImagingTask())
						{
							//成功则转到下一状态
							SwitchTaskState(SchedulerTaskStateEnum::UnloadFromImager);
						}
						else
						{
							//失败则进行芯片回收
							ReportInfo(RecordLevelEnum::Warning, "进行芯片回收");
							m_RecycleChip = true;
							SwitchTaskState(SchedulerTaskStateEnum::UnloadFromImager);
						}
						break;
					case UnloadFromImager:
						//从光机下料
						SwitchTaskState(DoUnloadFromImagerTask());
						break;
					case StopTask:
						//任务结束
						DoEndTask();
						return;
					default:
						//未知状态
						break;
					}

					QThread::msleep(1000);
				}
				catch (...)
				{
					//结束任务
					DoEndTask();
					break;
				}
			}

			BGI_MESSAGE("RunSchedulerTask End");
		});
}

SchedulerTaskStateEnum SchedulerDriver::DoInitPosTask()
{
	try
	{
		BGI_MESSAGE("do task InitPos");
		ReportInfo(RecordLevelEnum::Note, "任务已开始！");
		emit SignalImagerTaskStart();

		//机械臂归位
		if (!FunMonitor(&SchedulerDriver::RobotGoHome))
		{
			return SchedulerTaskStateEnum::StopTask;
		}

		//点亮绿灯
		OpenAlarmLight(RobotParaType::RobotLightColor::GREEN);

		BGI_MESSAGE("do task InitPos succeed.");
		ReportInfo(RecordLevelEnum::Info, "设备初始化成功");

		//成功则转到下一状态
		return FixNextTask();
	}
	CATCHTASKEXCEPTION("InitPos");
}

SchedulerTaskStateEnum SchedulerDriver::DoLoadToCoaterTask()
{
	try
	{
		BGI_MESSAGE("do task LoadToCoater");
		ReportInfo(RecordLevelEnum::Info, "涂布机1/3：开始上料到涂布机");

		//涂布机开始一轮新的Cycle
		++m_CurChipData.strTableDesc.iBiochemistryTimes;

		//等待涂布机准备好
		if (!FunMonitor(&SchedulerDriver::CoaterLoadReady))
		{
			return SchedulerTaskStateEnum::StopTask;
		}
		//把芯片夹到涂布机
		if (!FunMonitor(&SchedulerDriver::RobotGoCoater))
		{
			return SchedulerTaskStateEnum::StopTask;
		}
		//涂布机开真空
		m_bVacuumOpenFailed = !FunMonitor(&SchedulerDriver::CoaterOpenVacuum);
		if (m_bVacuumOpenFailed)
		{
			//开真空失败，回收芯片
			ReportInfo(RecordLevelEnum::Warning, "进行芯片回收");
			m_RecycleChip = true;
			return SchedulerTaskStateEnum::UnloadFromCoater;
		}
		//把芯片给涂布机，机械臂退到等待位
		if (!FunMonitor(&SchedulerDriver::RobotPutChipToCoater))
		{
			return SchedulerTaskStateEnum::StopTask;
		}
		
		BGI_MESSAGE("do task LoadToCoater succeed.");
		//成功则转到下一状态
		return SchedulerTaskStateEnum::Coating;
	}
	CATCHTASKEXCEPTION("LoadToCoater");
}

bool SchedulerDriver::DoCoatingTask()
{
	try
	{
		BGI_MESSAGE("do task Coating");
		ReportInfo(RecordLevelEnum::Info, "涂布机2/3：开始生化，Cycle=" + QString::number(m_CurChipData.strTableDesc.iBiochemistryTimes));

		//告诉涂布机上料完成，让涂布机开始生化
		if (!FunMonitor(&SchedulerDriver::CoaterStartWork, false))
		{
			//重试一次
			ReportInfo(RecordLevelEnum::Warning, "重试一次");
			if (!FunMonitor(&SchedulerDriver::CoaterStartWork))
			{
				return false;
			}
		}
		//等待涂布机生化完成
		if (!FunMonitor(&SchedulerDriver::CoaterFinished))
		{
			return false;
		}

		BGI_MESSAGE("do task Coating succeed.");
		return true;
	}
	catch (const QString& ex)
	{
		QString str = "do task Coating exception: " + ex;
		BGI_ERROR(str.toStdString().c_str());

		if (ex == "StopTask")
		{
			//急停
			throw;
		}
		
		return false;
	}
	catch (...)
	{
		QString str = "do task Coating unknown exception!";
		BGI_ERROR(str.toStdString().c_str());
		return false;
	}
}

SchedulerTaskStateEnum SchedulerDriver::DoUnloadFromCoaterTask()
{
	try
	{
		BGI_MESSAGE("do task UnloadFromCoater");
		ReportInfo(RecordLevelEnum::Info, "涂布机3/3：开始从涂布机下料");

		//开真空失败时，芯片还在机械臂上，不用再去夹取
		if (!m_bVacuumOpenFailed)
		{
			//等待涂布机准备好下料
			if (!CoaterUnloadReady())
			{
				return SchedulerTaskStateEnum::StopTask;
			}
			//从等待区去涂布机夹住芯片
			if (!FunMonitor(&SchedulerDriver::RobotGetChipFromCoater))
			{
				return SchedulerTaskStateEnum::StopTask;
			}
		}

		//关真空
		if (!FunMonitor(&SchedulerDriver::CoaterCloseVacuum, false))
		{
			//如果关真空失败，重试一次
			ReportInfo(RecordLevelEnum::Warning, "重试一次");
			if (!FunMonitor(&SchedulerDriver::CoaterCloseVacuum))
			{
				return SchedulerTaskStateEnum::StopTask;
			}
		}

		m_CoaterUnloading = true;
		//把芯片送回中转槽
		if (!FunMonitor(&SchedulerDriver::RobotGoRecycleFromCoater))
		{
			return SchedulerTaskStateEnum::StopTask;
		}

		if (m_CoaterUnloading)
		{
			m_CoaterUnloading = false;
			//通知涂布机下料完成
			if (!FunMonitor(&SchedulerDriver::CoaterSetUnloadState, false))
			{
				//如果失败重试一次，失败也可以继续，主要看下次的上料状态
				ReportInfo(RecordLevelEnum::Warning, "重试一次");
				FunMonitor(&SchedulerDriver::CoaterSetUnloadState, false);
			}
		}
		
		BGI_MESSAGE("do task UnloadFromCoater succeed.");
		//成功则转到下一状态
		return CoaterCycleEnd();
	}
	CATCHTASKEXCEPTION("UnloadFromCoater");
}

SchedulerTaskStateEnum SchedulerDriver::DoLoadToImagerTask()
{
	try
	{
		BGI_MESSAGE("do task LoadToImager");
		ReportInfo(RecordLevelEnum::Info, "光机1/3：开始上料到光机");
		emit SignalImagerTaskStart();

		//光机开始一轮新的Cycle
		++m_CurChipData.strTableDesc.iPhotographTime;

		//检查内存，如果没有则申请
		if (!FunMonitor(&SchedulerDriver::ImagerCheckMemory))
		{
			return SchedulerTaskStateEnum::StopTask;
		}
		//光机到加载位
		if (!FunMonitor(&SchedulerDriver::ImagerGoLoadingPos, false))
		{
			//如果失败重试一次
			ReportInfo(RecordLevelEnum::Warning, "重试一次");
			if (!FunMonitor(&SchedulerDriver::ImagerGoLoadingPos))
			{
				return SchedulerTaskStateEnum::StopTask;
			}
		}
		//把芯片送到光机
		if (!FunMonitor(&SchedulerDriver::RobotGoImager))
		{
			return SchedulerTaskStateEnum::StopTask;
		}
		//开真空
		m_bVacuumOpenFailed = !FunMonitor(&SchedulerDriver::ImagerOpenVacuum);
		if (m_bVacuumOpenFailed)
		{
			//开真空失败，回收芯片
			ReportInfo(RecordLevelEnum::Warning, "进行芯片回收");
			m_RecycleChip = true;
			return SchedulerTaskStateEnum::UnloadFromImager;
		}
		//机械臂把芯片释放给光机，并退到等待位
		if (!FunMonitor(&SchedulerDriver::RobotPutChipToImager))
		{
			return SchedulerTaskStateEnum::StopTask;
		}
		
		BGI_MESSAGE("do task LoadToImager succeed.");
		//成功则转到下一状态
		return SchedulerTaskStateEnum::Imaging;
	}
	CATCHTASKEXCEPTION("LoadToImager");
}

bool SchedulerDriver::DoImagingTask()
{
	try
	{
		BGI_MESSAGE("do task Imaging");
		ReportInfo(RecordLevelEnum::Info, "光机2/3：开始拍照，Cycle=" + QString::number(m_CurChipData.strTableDesc.iPhotographTime));

		//首次进行一次config设置
		if (m_bFirstConfigSet)
		{
			m_bFirstConfigSet = false;

			if (!FunMonitor(&SchedulerDriver::ImagerSetConfig, false))
			{
				//如果失败重试一次
				ReportInfo(RecordLevelEnum::Warning, "重试一次");
				if (!FunMonitor(&SchedulerDriver::ImagerSetConfig))
				{
					return false;
				}
			}
		}

		//如果光机拍照失败，有可能重试几次
		//如果拍照成功，则一次就完成
		for (int i = 0; i <= m_ImagerRetryMaxCount; ++i)
		{
			//设置本次拍照参数
			if (!FunMonitor(&SchedulerDriver::ImagerSetupParam, false))
			{
				//如果失败重试一次
				ReportInfo(RecordLevelEnum::Warning, "重试一次");
				if (!FunMonitor(&SchedulerDriver::ImagerSetupParam))
				{
					return false;
				}
			}
			//启动拍照
			m_bImagerFinished = false;
			m_bImagerRetryError = false;
			if (!FunMonitor(&SchedulerDriver::ImagerStartRun))
			{
				return false;
			}
			//等待光机拍照完成
			if (!FunMonitor(&SchedulerDriver::ImagerFinished))
			{
				//拍照失败
				if (m_bImagerRetryError)
				{
					//可以重试的错误，重新拍照
					if (i < m_ImagerRetryMaxCount)
					{
						ReportInfo(RecordLevelEnum::Warning, "光机发生可以重试的错误，重新拍照");
					}
					else
					{
						ReportInfo(RecordLevelEnum::Error, "重试拍照次数已达上限，仍然失败！");
					}
					continue;
				}
				else
				{
					//不能重试的错误，直接失败
					return false;
				}
			}
			else
			{
				//拍照成功，跳出循环
				BGI_MESSAGE("do task Imaging succeed.");

				if (i > 0)
				{
					//重新亮绿灯
					OpenAlarmLight(RobotParaType::RobotLightColor::GREEN);
				}
				
				return true;
			}
		}

		BGI_ERROR("do task Imaging retry failed.");
		return false;
	}
	catch (const QString& ex)
	{
		QString str = "do task Imaging exception: " + ex;
		BGI_ERROR(str.toStdString().c_str());

		if (ex == "StopTask")
		{
			//急停
			throw;
		}

		return false;
	}
	catch (...)
	{
		QString str = "do task Imaging unknown exception!";
		BGI_ERROR(str.toStdString().c_str());
		return false;
	}
}

SchedulerTaskStateEnum SchedulerDriver::DoUnloadFromImagerTask()
{
	try
	{
		BGI_MESSAGE("do task UnloadFromImager");
		ReportInfo(RecordLevelEnum::Info, "光机3/3：开始从光机下料");

		//开真空失败时，芯片还在机械臂上，不用再去夹取
		if (!m_bVacuumOpenFailed)
		{
			//光机回加载位
			if (!FunMonitor(&SchedulerDriver::ImagerGoLoadingPos, false))
			{
				//如果失败重试一次
				ReportInfo(RecordLevelEnum::Warning, "重试一次");
				if (!FunMonitor(&SchedulerDriver::ImagerGoLoadingPos))
				{
					return SchedulerTaskStateEnum::StopTask;
				}
			}
			//机械臂从等待区去光机夹住芯片
			if (!FunMonitor(&SchedulerDriver::RobotGetChipFromImager))
			{
				return SchedulerTaskStateEnum::StopTask;
			}
		}
		//光机关真空
		if (!FunMonitor(&SchedulerDriver::ImagerCloseVacuum, false))
		{
			//如果关真空失败，重试一次
			ReportInfo(RecordLevelEnum::Warning, "重试一次");
			if (!FunMonitor(&SchedulerDriver::ImagerCloseVacuum))
			{
				return SchedulerTaskStateEnum::StopTask;
			}
		}
		//把芯片送回中转槽
		if (!FunMonitor(&SchedulerDriver::RobotGoRecycleFromImager))
		{
			return SchedulerTaskStateEnum::StopTask;
		}
		
		BGI_MESSAGE("do task UnloadFromImager succeed.");
		//成功则转到下一状态
		return ImagerCycleEnd();
	}
	CATCHTASKEXCEPTION("UnloadFromImager");
}

void SchedulerDriver::DoEndTask()
{
	BGI_MESSAGE("do task EndTask");
	ReportInfo(RecordLevelEnum::Note, "任务已结束！");
	OpenAlarmLight(RobotParaType::RobotLightColor::NONE);
	m_PauseState = false;
	emit SignalSchedulerTaskEnd();
}

SchedulerTaskStateEnum SchedulerDriver::CoaterCycleEnd()
{
	if (m_RecycleChip)
	{
		//遇到了错误，回收芯片后结束
		return SchedulerTaskStateEnum::StopTask;
	}
	else
	{
		SaveChipData();
		return FixNextTask(m_CurChipData.strTableDesc.iBiochemistryTimes, m_CurChipData.strTableDesc.iPhotographTime, false);
	}
}

SchedulerTaskStateEnum SchedulerDriver::ImagerCycleEnd()
{
	if (m_RecycleChip)
	{
		//遇到了错误，回收芯片后结束
		return SchedulerTaskStateEnum::StopTask;
	}
	else
	{
		//正常继续进行
		SaveChipData();
		return FixNextTask(m_CurChipData.strTableDesc.iPhotographTime, m_CurChipData.strTableDesc.iBiochemistryTimes, true);
	}
}

SchedulerTaskStateEnum SchedulerDriver::FixNextTask(int firstCycle, int secondCycle, bool bImager)
{
	QString str = "生化Cycle是" + QString::number(m_CurChipData.strTableDesc.iBiochemistryTimes) + "，拍照Cycle是" + QString::number(m_CurChipData.strTableDesc.iPhotographTime) + "，";

	int end1 = m_CurChipData.strTableDesc.iR1;
	int end2 = m_CurChipData.strTableDesc.iR1 + m_CurChipData.strTableDesc.iR2;
	int end3 = m_CurChipData.strTableDesc.iR1 + m_CurChipData.strTableDesc.iR2 + m_CurChipData.strTableDesc.iB1;
	int end4 = m_CurChipData.strTableDesc.iR1 + m_CurChipData.strTableDesc.iR2 + m_CurChipData.strTableDesc.iB1 + +m_CurChipData.strTableDesc.iB2;

	if (firstCycle == end1 && secondCycle >= end1)
	{
		//一链结束
		ReportInfo(RecordLevelEnum::Note, str + "一链完成，任务结束！");
		if (bImager)
		{
			ImagerReleaseRun();
		}
		return SchedulerTaskStateEnum::StopTask;
	}
	else if (firstCycle == end2 && secondCycle >= end2)
	{
		//二链结束
		ReportInfo(RecordLevelEnum::Note, str + "二链完成，任务结束！");
		if (bImager)
		{
			ImagerReleaseRun();
		}
		return SchedulerTaskStateEnum::StopTask;
	}
	else if (firstCycle == end3 && secondCycle >= end3)
	{
		//Barcode1结束
		ReportInfo(RecordLevelEnum::Note, str + "Barcode1完成，任务结束！");
		if (bImager)
		{
			ImagerReleaseRun();
		}
		return SchedulerTaskStateEnum::StopTask;
	}
	else if (firstCycle == end4 && secondCycle >= end4)
	{
		//Barcode2结束
		ReportInfo(RecordLevelEnum::Note, str + "Barcode2完成，任务结束！");
		if (bImager)
		{
			ImagerReleaseRun();
		}
		return SchedulerTaskStateEnum::StopTask;
	}
	else
	{
		return FixNextTask();
	}
}

SchedulerTaskStateEnum SchedulerDriver::FixNextTask()
{
	QString str = "生化Cycle是" + QString::number(m_CurChipData.strTableDesc.iBiochemistryTimes) + "，拍照Cycle是" + QString::number(m_CurChipData.strTableDesc.iPhotographTime) + "，";
	
	if (m_CurChipData.strTableDesc.iBiochemistryTimes >= m_CurChipData.strTableDesc.iAll && m_CurChipData.strTableDesc.iPhotographTime >= m_CurChipData.strTableDesc.iAll)
	{
		//已全部结束
		ReportInfo(RecordLevelEnum::Note, str + "全部Cycle已完成，任务结束！");
		return SchedulerTaskStateEnum::StopTask;
	}
	else if (m_EndCycle > 0 && m_CurChipData.strTableDesc.iBiochemistryTimes >= m_EndCycle && m_CurChipData.strTableDesc.iPhotographTime >= m_EndCycle)
	{
		//到达结束Cycle
		ReportInfo(RecordLevelEnum::Note, str + "到达回收Cycle，任务结束！");
		return SchedulerTaskStateEnum::StopTask;
	}
	else
	{
		SchedulerTaskStateEnum state = SchedulerTaskStateEnum::LoadToImager;
		if (m_CurChipData.strTableDesc.iPhotographTime < m_CurChipData.strTableDesc.iBiochemistryTimes)
		{
			//拍照cycle小于生化cycle，进行拍照
			ReportInfo(RecordLevelEnum::Info, str + "将进行拍照任务");
			state = SchedulerTaskStateEnum::LoadToImager;
		}
		else
		{
			//拍照cycle等于或大于生化cycle，进行生化
			ReportInfo(RecordLevelEnum::Info, str + "将进行生化任务");
			state = SchedulerTaskStateEnum::LoadToCoater;
		}

		if (m_PauseState)
		{
			ReportInfo(RecordLevelEnum::Note, "任务已暂停！");
			m_pDeviceWarningLights->StartRun(false, RobotParaType::RobotLightColor::GREEN);
		}

		return state;
	}
}

void SchedulerDriver::ImagerReleaseRun()
{
	if (!m_pImagerDriver->ReleaseRun(m_CurChipData.strTableDesc))
	{
		ReportInfo(RecordLevelEnum::Warning, m_CurChipData.strBarCode + " 内存释放失败！");
	}
	else
	{
		ReportInfo(RecordLevelEnum::Info, m_CurChipData.strBarCode + " 内存释放成功！");
	}
}

void SchedulerDriver::SaveChipData()
{
	OperateSqliteConfig::Instance()->WriteCfgData(m_CurChipData);
	emit SignalUpdateCycle(m_CurChipData.strTableDesc);
}

bool SchedulerDriver::RobotGoHome()
{
	ReportInfo(RecordLevelEnum::Info, "机械臂回HOME...");

	if (IsRobotSimulated())
	{
		return true;
	}

	bool bRet = m_pEpsonRobotFlowCtrl->Ctrl_AnyWhere2Home();
	//bRet = true;  //for test, need delete
	RecordInfo(bRet, "机械臂回HOME");

	return bRet;
}

bool SchedulerDriver::RobotGoCoater()
{
	ReportInfo(RecordLevelEnum::Info, "将芯片送给涂布机...");

	if (IsRobotSimulated())
	{
		return true;
	}

	bool bRet = m_pEpsonRobotFlowCtrl->GoCoaterFromRecycle();
	//bRet = true;  //for test, need delete
	RecordInfo(bRet, "机械臂将芯片从中转槽送到涂布机");
	return bRet;
}

bool SchedulerDriver::RobotPutChipToCoater()
{
	ReportInfo(RecordLevelEnum::Info, "机械臂退到涂布机等待位...");

	if (IsRobotSimulated())
	{
		return true;
	}

	bool bRet = m_pEpsonRobotFlowCtrl->ReleaseChipToCoater();
	//bRet = true;  //for test, need delete
	RecordInfo(bRet, "机械臂将芯片释放给涂布机");
	return bRet;
}

bool SchedulerDriver::RobotGetChipFromCoater()
{
	ReportInfo(RecordLevelEnum::Info, "进入涂布机夹芯片...");

	if (IsRobotSimulated())
	{
		return true;
	}

	bool bRet = m_pEpsonRobotFlowCtrl->HoldChipFromCoater();
	//bRet = true;  //for test, need delete
	RecordInfo(bRet, "机械臂到涂布机夹芯片");
	return bRet;
}

bool SchedulerDriver::RobotGoRecycleFromCoater()
{
	ReportInfo(RecordLevelEnum::Info, "将芯片从涂布机送回中转槽...");

	if (IsRobotSimulated())
	{
		return true;
	}

	bool bRet = m_pEpsonRobotFlowCtrl->GoRecycleFromCoater();
	//bRet = true;  //for test, need delete
	RecordInfo(bRet, "机械臂将芯片从涂布机送回中转槽");
	return bRet;
}

bool SchedulerDriver::RobotGoImager()
{
	ReportInfo(RecordLevelEnum::Info, "将芯片送给光机...");

	if (IsRobotSimulated())
	{
		return true;
	}

	bool bRet = m_pEpsonRobotFlowCtrl->GoImagerFromRecycle();
	//bRet = true;  //for test, need delete
	RecordInfo(bRet, "机械臂将芯片从中转槽送到光机");
	return bRet;
}

bool SchedulerDriver::RobotPutChipToImager()
{
	ReportInfo(RecordLevelEnum::Info, "机械臂退到光机等待位...");

	if (IsRobotSimulated())
	{
		return true;
	}

	bool bRet = m_pEpsonRobotFlowCtrl->ReleaseChipToImager();
	//bRet = true;  //for test, need delete
	RecordInfo(bRet, "机械臂将芯片释放给光机");
	return bRet;
}

bool SchedulerDriver::RobotGetChipFromImager()
{
	ReportInfo(RecordLevelEnum::Info, "进入光机夹芯片...");

	if (IsRobotSimulated())
	{
		return true;
	}

	bool bRet = m_pEpsonRobotFlowCtrl->HoldChipFromImager();
	//bRet = true;  //for test, need delete
	RecordInfo(bRet, "机械臂到光机夹芯片");
	return bRet;
}

bool SchedulerDriver::RobotGoRecycleFromImager()
{
	ReportInfo(RecordLevelEnum::Info, "将芯片从光机送回中转槽...");

	if (IsRobotSimulated())
	{
		return true;
	}

	bool bRet = m_pEpsonRobotFlowCtrl->GoRecycleFromImager();
	//bRet = true;  //for test, need delete
	RecordInfo(bRet, "机械臂将芯片从光机送回中转槽");
	return bRet;
}

bool SchedulerDriver::CoaterStartWork()
{
	if (IsCoaterSimulated())
	{
		return true;
	}

	bool bRet = m_pCoaterDriver->SetLoadFinish();
	RecordInfo(bRet, "启动涂布机生化");
	return bRet;
}

bool SchedulerDriver::CoaterSetUnloadState()
{
	if (IsCoaterSimulated())
	{
		return true;
	}

	bool bRet = m_pCoaterDriver->SetUnloadFinish();
	RecordInfo(bRet, "设置涂布机下料状态");
	return bRet;
}

bool SchedulerDriver::ImagerSetupParam()
{
	if (IsImagerSimulated())
	{
		return true;
	}

	bool bRet = m_pImagerDriver->ImagerSetup(m_CurChipData.strTableDesc);
	RecordInfo(bRet, "光机拍照参数设置");
	return bRet;
}

bool SchedulerDriver::ImagerStartRun()
{
	if (IsImagerSimulated())
	{
		return true;
	}

	bool bRet = m_pImagerDriver->StartRun();
	//bRet = true;  //for test, need delete
	RecordInfo(bRet, "启动光机拍照");
	return bRet;
}

bool SchedulerDriver::ImagerGoLoadingPos()
{
	if (IsImagerSimulated())
	{
		return true;
	}

	if (!m_pImagerDriver->GoLoadingPos())
	{
		//命令就失败了
		ReportInfo(RecordLevelEnum::Error, "光机回加载位失败");
		return false;
	}

	//等待回加载位完成
	for (int i = 0; i < 10; ++i)
	{
		QThread::sleep(1);

		if (m_NextTask == SchedulerTaskStateEnum::StopTask)
		{
			return false;
		}

		if (m_pImagerDriver->IsAtLoadPosition())
		{
			//回加载位完成
			ReportInfo(RecordLevelEnum::Info, "光机回加载位成功");
			return true;
		}
	}

	//走到这里代表超时了
	ReportInfo(RecordLevelEnum::Error, "光机回加载位超时！");
	return false;
}

bool SchedulerDriver::ImagerCheckMemory()
{
	if (IsImagerSimulated())
	{
		return true;
	}

	if (m_pImagerDriver->CheckMemory(m_CurChipData.strBarCode))
	{
		//原来就有内存
		ReportInfo(RecordLevelEnum::Info, m_CurChipData.strBarCode + " 内存已准备好");
		return true;
	}
	else
	{
		//原来没有内存，需要申请
		ReportInfo(RecordLevelEnum::Info, m_CurChipData.strBarCode + " 暂无内存，需要申请！StartCycle=" + QString::number(m_CurChipData.strTableDesc.iPhotographTime));

		m_bImagerMemFailed = false;
		if (!m_pImagerDriver->SetupRun(m_CurChipData.strTableDesc))
		{
			//申请就失败了
			ReportInfo(RecordLevelEnum::Error, m_CurChipData.strBarCode + " 内存申请失败！");
			return false;
		}
		else
		{
			//申请下发成功，还要等待申请完成
			ReportInfo(RecordLevelEnum::Info, m_CurChipData.strBarCode + " 已下发内存申请，需等待申请完成，" + QString::number(m_MemoryTimeout) + "秒超时...");

			for (int i = 0; i < m_MemoryTimeout; ++i)
			{
				QThread::sleep(1);

				if (m_NextTask == SchedulerTaskStateEnum::StopTask)
				{
					return false;
				}

				if (m_bImagerMemFailed)
				{
					ReportInfo(RecordLevelEnum::Error, m_CurChipData.strBarCode + " 内存申请失败！");
					return false;
				}

				//每隔2秒查询一次
				if (i % 2 == 1)
				{
					if (m_pImagerDriver->CheckMemory(m_CurChipData.strBarCode))
					{
						//内存申请已经完成了
						ReportInfo(RecordLevelEnum::Info, m_CurChipData.strBarCode + " 内存已准备好");
						return true;
					}
				}

				//每隔10秒打印一次继续等待
				if (i % 10 == 9)
				{
					ReportInfo(RecordLevelEnum::Info, m_CurChipData.strBarCode + " 内存申请未完成，可继续等待" + QString::number(m_MemoryTimeout - 1 - i) + "秒");
				}
			}

			//走到这里代表超时了还未完成
			ReportInfo(RecordLevelEnum::Error, m_CurChipData.strBarCode + " 内存申请超时！");
			return false;
		}
	}
}

bool SchedulerDriver::ImagerOpenVacuum()
{
	if (IsImagerSimulated())
	{
		return true;
	}

	if (!m_pImagerDriver->SwitchVacuum(true))
	{
		//开真空就失败了
		ReportInfo(RecordLevelEnum::Error, "光机开真空失败");
		return false;
	}

	//等待真空值达标
	ReportInfo(RecordLevelEnum::Info, "光机真空泵已开，等待真空值达到" + QString::number(m_ImagerVacuumMaxValue) + "，" + QString::number(m_ImagerVacuumTimeout) + "秒超时...");
	for (int i = 0; i < m_ImagerVacuumTimeout; ++i)
	{
		QThread::sleep(1);

		if (m_NextTask == SchedulerTaskStateEnum::StopTask)
		{
			return false;
		}

		if (fabsf(m_pImagerDriver->GetVacuumValue()) >= fabsf(m_ImagerVacuumMaxValue))
		{
			//真空值达标
			ReportInfo(RecordLevelEnum::Info, "光机开真空成功");
			return true;
		}
	}

	//走到这里代表超时了
	ReportInfo(RecordLevelEnum::Error, "光机开真空超时！真空值=" + QString::number(m_pImagerDriver->GetVacuumValue(), 'f', 2));
	return false;
}

bool SchedulerDriver::ImagerCloseVacuum()
{
	if (IsImagerSimulated())
	{
		return true;
	}

	if (!m_pImagerDriver->SwitchVacuum(false))
	{
		//关真空就失败了
		ReportInfo(RecordLevelEnum::Error, "光机关真空失败");
		return false;
	}

	//等待真空值达标
	ReportInfo(RecordLevelEnum::Info, "光机真空泵已关，等待真空值达到" + QString::number(m_ImagerVacuumMinValue) + "，" + QString::number(m_ImagerVacuumTimeout) + "秒超时...");
	for (int i = 0; i < m_ImagerVacuumTimeout; ++i)
	{
		QThread::sleep(1);

		if (m_NextTask == SchedulerTaskStateEnum::StopTask)
		{
			return false;
		}

		if (fabsf(m_pImagerDriver->GetVacuumValue()) <= fabsf(m_ImagerVacuumMinValue))
		{
			//真空值达标
			ReportInfo(RecordLevelEnum::Info, "光机关真空成功");
			return true;
		}
	}

	//走到这里代表超时了
	ReportInfo(RecordLevelEnum::Error, "光机关真空超时！真空值=" + QString::number(m_pImagerDriver->GetVacuumValue(), 'f', 2));
	return false;
}

bool SchedulerDriver::ImagerSetConfig()
{
	if (IsImagerSimulated())
	{
		return true;
	}

	ImagerParaType::ImagerConfigPara data;
	if (!m_pImagerDriver->ImagerConfigGet(data))
	{
		ReportInfo(RecordLevelEnum::Error, "获取光机配置参数失败！");
		return false;
	}

	QString fileName = "C:\\BGI\\recipes\\" + m_CurChipData.strTableDesc.strSequencingMode + "_ExposureTime.csv";;
	ImagerDriver::ReadExposureFile(fileName, data.ExposureTime, data.ExposureFactors);

	bool bRet = m_pImagerDriver->ImagerConfigSet(data);
	RecordInfo(bRet, "设置光机配置参数");
	return bRet;
}

bool SchedulerDriver::ImagerFinished()
{
	ReportInfo(RecordLevelEnum::Info, "等待光机拍照，" + QString::number(m_ImagingTimeout) + "秒超时...");

	if (IsImagerSimulated())
	{
		return true;
	}

	for (int i = 0; i < m_ImagingTimeout; ++i)
	{
		QThread::sleep(1);

		if (m_NextTask == SchedulerTaskStateEnum::StopTask)
		{
			return false;
		}

		if (m_bImagerFinished)
		{
			//拍照已完成
			//ReportInfo(RecordLevelEnum::Info, "光机拍照已完成");
			return !m_RecycleChip;
		}
	}

	//走到这里代表超时了
	ReportInfo(RecordLevelEnum::Error, "光机拍照超时！");
	return false;
}

void SchedulerDriver::RecvImagerTaskEnd(const ImagerParaType::ImagerSetupPara& info, int errorCode, const QString& errorStr)
{
	if (info.SlideID != m_CurChipData.strBarCode)
	{
		return;
	}
	if (m_NextTask != SchedulerTaskStateEnum::Imaging)
	{
		return;
	}

	if (errorCode == 0)
	{
		//成功
		ReportInfo(RecordLevelEnum::Info, "光机拍照成功！ImagerCycle="  + QString::number(info.ImagerCycle));

		//更新拍照Cycle
		SaveChipData();

		m_RecycleChip = false;
	}
	else
	{
		//失败
		ReportInfo(RecordLevelEnum::Error, "光机拍照失败！ImagerCycle=" + QString::number(info.ImagerCycle) + ", 错误码：" + QString::number(errorCode) + "=" + errorStr);

		m_bImagerRetryError = m_ImagerRetryErrors.contains(errorCode);

		//拍照失败，回收芯片结束
		m_RecycleChip = true;
	}

	m_bImagerFinished = true;
}

void SchedulerDriver::RecvImagerError(const QString& error)
{
	ReportInfo(RecordLevelEnum::Error, "收到光机拍照错误码：" + error);
}

//void SchedulerDriver::RecvImagerRunInfo(const ImagerParaType::ImagerRunPara& info)
//{
//
//}

//bool SchedulerDriver::RecvImagerVacuumValue(float value)
//{
//
//}

bool SchedulerDriver::CoaterOpenVacuum()
{
	if (IsCoaterSimulated())
	{
		return true;
	}

	if (!m_pCoaterDriver->SwitchVacuum(true))
	{
		//开真空就失败了
		ReportInfo(RecordLevelEnum::Error, "涂布机开真空失败");
		return false;
	}

	//等待真空值达标
	ReportInfo(RecordLevelEnum::Info, "涂布机真空泵已开，等待真空值达到" + QString::number(m_CoaterVacuumMaxValue) + "，" + QString::number(m_CoaterVacuumTimeout) + "秒超时...");
	for (int i = 0; i < m_CoaterVacuumTimeout; ++i)
	{
		QThread::sleep(1);

		if (m_NextTask == SchedulerTaskStateEnum::StopTask)
		{
			return false;
		}

		if (fabsf(m_pCoaterDriver->GetVacuumValue()) >= fabsf(m_CoaterVacuumMaxValue))
		{
			//真空值达标
			ReportInfo(RecordLevelEnum::Info, "涂布机开真空成功");
			return true;
		}
	}

	//走到这里代表超时了
	ReportInfo(RecordLevelEnum::Error, "涂布机开真空超时！真空值=" + QString::number(m_pCoaterDriver->GetVacuumValue(), 'f', 2));
	return false;
}

bool SchedulerDriver::CoaterCloseVacuum()
{
	if (IsCoaterSimulated())
	{
		return true;
	}

	if (!m_pCoaterDriver->SwitchVacuum(false))
	{
		//关真空就失败了
		ReportInfo(RecordLevelEnum::Error, "涂布机关真空失败");
		return false;
	}

	//等待真空值达标
	ReportInfo(RecordLevelEnum::Info, "涂布机真空泵已关，等待真空值达到" + QString::number(m_CoaterVacuumMinValue) + "，" + QString::number(m_CoaterVacuumTimeout) + "秒超时...");
	for (int i = 0; i < m_CoaterVacuumTimeout; ++i)
	{
		QThread::sleep(1);

		if (m_NextTask == SchedulerTaskStateEnum::StopTask)
		{
			return false;
		}

		if (fabsf(m_pCoaterDriver->GetVacuumValue()) <= fabsf(m_CoaterVacuumMinValue))
		{
			//真空值达标
			ReportInfo(RecordLevelEnum::Info, "涂布机关真空成功");
			return true;
		}
	}

	//走到这里代表超时了
	ReportInfo(RecordLevelEnum::Error, "涂布机关真空超时！真空值=" + QString::number(m_pCoaterDriver->GetVacuumValue(), 'f', 2));
	return false;
}

bool SchedulerDriver::CoaterStartup()
{
	if (IsCoaterSimulated())
	{
		return true;
	}

	return m_pCoaterDriver->Startup();
}

bool SchedulerDriver::CoaterLoadReady()
{
	ReportInfo(RecordLevelEnum::Info, "检查涂布机上料状态");

	if (IsCoaterSimulated())
	{
		return true;
	}

	if (m_pCoaterDriver->IsLoadState())
	{
		ReportInfo(RecordLevelEnum::Info, "涂布机已准备好上料");
		return true;
	}

	//还没准备好，等待
	ReportInfo(RecordLevelEnum::Info, "等待涂布机可上料，" + QString::number(m_LoadTimeout) + "秒超时...");

	//尝试下发一次启动，促进涂布机准备好上料
	CoaterStartup();

	for (int i = 0; i < m_LoadTimeout; ++i)
	{
		QThread::sleep(1);

		if (m_NextTask == SchedulerTaskStateEnum::StopTask)
		{
			return false;
		}

		if (m_pCoaterDriver->IsLoadState())
		{
			//涂布机已经准备好
			ReportInfo(RecordLevelEnum::Info, "涂布机已准备好上料");
			return true;
		}
	}

	//走到这里代表超时了
	ReportInfo(RecordLevelEnum::Error, "等待涂布机可上料超时！");
	return false;
}

bool SchedulerDriver::CoaterUnloadReady()
{
	ReportInfo(RecordLevelEnum::Info, "检查涂布机下料状态");

	if (IsCoaterSimulated())
	{
		return true;
	}

	if (m_pCoaterDriver->IsUnloadState())
	{
		ReportInfo(RecordLevelEnum::Info, "涂布机已准备好下料");
		return true;
	}

	//还没准备好，等待
	ReportInfo(RecordLevelEnum::Info, "等待涂布机可下料，" + QString::number(m_UnloadTimeout) + "秒超时...");

	for (int i = 0; i < m_UnloadTimeout; ++i)
	{
		QThread::sleep(1);

		if (m_NextTask == SchedulerTaskStateEnum::StopTask)
		{
			return false;
		}

		if (m_pCoaterDriver->IsUnloadState())
		{
			//涂布机已经准备好
			ReportInfo(RecordLevelEnum::Info, "涂布机已准备好下料");
			return true;
		}
	}

	//走到这里代表超时了
	ReportInfo(RecordLevelEnum::Error, "等待涂布机可下料超时！");
	return false;
}

bool SchedulerDriver::CoaterFinished()
{
	ReportInfo(RecordLevelEnum::Info, "等待涂布机生化，" + QString::number(m_CoatingTimeout) + "秒超时...");
	
	if (IsCoaterSimulated())
	{
		return true;
	}

	for (int i = 0; i < m_CoatingTimeout; ++i)
	{
		QThread::sleep(1);

		if (m_NextTask == SchedulerTaskStateEnum::StopTask)
		{
			return false;
		}

		if (m_pCoaterDriver->IsUnloadState())
		{
			//生化已完成
			RecordInfo(m_CoatingSucceed, "涂布机生化");
			
			if (m_CoatingSucceed)
			{
				//更新生化Cycle
				SaveChipData();
			}
			return m_CoatingSucceed;
		}
	}

	//走到这里代表超时了
	ReportInfo(RecordLevelEnum::Error, "涂布机生化超时！");
	return false;
}

void SchedulerDriver::SlotCoaterAlarm(QString alarm)
{
	ReportInfo(RecordLevelEnum::Error, "涂布机报警：" + alarm);
}

void SchedulerDriver::SlotCoatingResult(int result)
{
	QString str = "Recv Coating Task Result: " + QString::number(result);
	BGI_MESSAGE(str.toStdString().c_str());

	m_CoatingSucceed = (result == 0);
}

void SchedulerDriver::SlotImagerData(const QString& key, const QString& value)
{
	if (key == "Error")
	{
		ReportInfo(RecordLevelEnum::Warning, "收到光机错误: " + value);
	}
	else if (key == "SetupRun")
	{
		m_bImagerMemFailed = (value != "0");
	}
}

void SchedulerDriver::CheckTaskEnd()
{
	if (m_NextTask == SchedulerTaskStateEnum::StopTask)
	{
		ReportInfo(RecordLevelEnum::Note, "任务已停止");
		throw QString("StopTask");
	}
}

bool SchedulerDriver::FunMonitor(bool (SchedulerDriver::* taskFun)(), bool bAlarm)
{
	//调用前核对是否任务已被停止
	CheckTaskEnd();

	bool bRet = false;
	try
	{
		bRet = (this->*taskFun)();
	}
	catch (...)
	{
	}

	//如果失败，且允许告警，则亮红灯
	if (!bRet && bAlarm && m_NextTask != SchedulerTaskStateEnum::StopTask)
	{
		OpenAlarmLight(RobotParaType::RobotLightColor::RED);
	}

	//调用后核对是否任务已被停止
	CheckTaskEnd();

	return bRet;
}

bool SchedulerDriver::OpenAlarmLight(RobotParaType::RobotLightColor lightColor)
{
	if (IsRobotSimulated())
	{
		return true;
	}
	m_pDeviceWarningLights->Stop();
	QString strTip = "";
	if (lightColor == RobotParaType::RobotLightColor::NONE)
		strTip = "熄灭告警灯";
	else if (lightColor == RobotParaType::RobotLightColor::RED)
		strTip = "点亮红灯";
	else if (lightColor == RobotParaType::RobotLightColor::GREEN)
		strTip = "点亮绿灯";
	else if (lightColor == RobotParaType::RobotLightColor::YELLO)
		strTip = "点亮黄灯";
	int iCount = 0, iTotal = 3;
	bool bRet = false;
	while (iCount<iTotal)
	{
		bRet = m_pEpsonRobotFlowCtrl->OperateWarningLights(lightColor);
		if (bRet)
			break;
		iCount++;
		QThread::msleep(500);
	}
	RecordInfo(bRet, strTip);
	return bRet;
}

bool SchedulerDriver::IsRobotSimulated()
{
	if (m_IsRobotSimulated)
	{
		ReportInfo(RecordLevelEnum::Warning, "机械臂模拟完成");
		return true;
	}
	else
	{
		return false;
	}
}

bool SchedulerDriver::IsCoaterSimulated()
{
	if (m_IsCoaterSimulated)
	{
		ReportInfo(RecordLevelEnum::Warning, "涂布机模拟完成");
		return true;
	}
	else
	{
		return false;
	}
}

bool SchedulerDriver::IsImagerSimulated()
{
	if (m_IsImagerSimulated)
	{
		ReportInfo(RecordLevelEnum::Warning, "光机模拟完成");
		return true;
	}
	else
	{
		return false;
	}
}
