#pragma once

#include <QObject>
#include <QTimer>
#include "OperateRecordInfo.h"
#include "OperateSqliteConfig.h"
#include "ImagerParaType.h"
#include "RobotParaType.h"

enum SchedulerTaskStateEnum
{
	StartTask = 0,     //开始任务
	LoadToCoater,      //上料到涂布机
	Coating,           //涂布
	UnloadFromCoater,  //从涂布机下料
	LoadToImager,      //上料到光机
	Imaging,           //拍照
	UnloadFromImager,  //从光机下料
	StopTask           //结束任务
};

enum RecordLevelEnum
{
	Info = 0,
	Note,
	Warning,
	Error
};

class ImagerDriver;
class EpsonRobotFlowCtrl;
class CoaterDriver;
class DeviceWarningLights;

class BGIBUSINE_CLASS_EXPORT SchedulerDriver  : public QObject
{
	Q_OBJECT

public:
	SchedulerDriver(QObject *parent = nullptr);
	~SchedulerDriver();

	//开始运行
	void StartRun(QString barcode);
	//暂停运行
	void PauseRun();
	//停止运行
	void StopRun();
	//设置结束Cycle
	void SetEndCycle(int cycle);
	int GetEndCycle();

	//光机拍照完成
	void RecvImagerTaskEnd(const ImagerParaType::ImagerSetupPara& info, int errorCode, const QString& errorStr);
	//光机拍照发生错误
	void RecvImagerError(const QString& error);
	//void RecvImagerRunInfo(const ImagerParaType::ImagerRunPara& info);
	//bool RecvImagerVacuumValue(float value);

	//机械臂更新安全工作空间状态
	void UpdateRobotSafetyStatus(bool bCoaterSafety, bool bImagerSafety);

signals:
	void SignalSchedulerTaskEnd();
	void SignalUpdateCycle(GeneralParaType::SequencingTableDesc strTableDesc);
	void SignalImagerTaskStart();

private slots:
	void SlotCoaterAlarm(QString alarm);
	void SlotCoatingResult(int result);
	void SlotImagerData(const QString& key, const QString& value);

private:
	void RunSchedulerTask();
	void SwitchTaskState(SchedulerTaskStateEnum state);
	void SwitchTaskStateDirect(SchedulerTaskStateEnum state);

	SchedulerTaskStateEnum DoInitPosTask();

	SchedulerTaskStateEnum DoLoadToCoaterTask();
	bool DoCoatingTask();
	SchedulerTaskStateEnum DoUnloadFromCoaterTask();

	SchedulerTaskStateEnum DoLoadToImagerTask();
	bool DoImagingTask();
	SchedulerTaskStateEnum DoUnloadFromImagerTask();

	void DoEndTask();

	SchedulerTaskStateEnum CoaterCycleEnd();
	SchedulerTaskStateEnum ImagerCycleEnd();
	SchedulerTaskStateEnum FixNextTask(int firstCycle, int secondCycle, bool bImager);
	SchedulerTaskStateEnum FixNextTask();
	void ImagerReleaseRun();

	void RecordInfo(bool bSucceed, const QString& info);
	void ReportInfo(RecordLevelEnum level, const QString& info);

	void SaveChipData();
	
	bool RobotGoHome();
	bool RobotGoCoater();
	bool RobotPutChipToCoater();
	bool RobotGetChipFromCoater();
	bool RobotGoRecycleFromCoater();
	bool RobotGoImager();
	bool RobotPutChipToImager();
	bool RobotGetChipFromImager();
	bool RobotGoRecycleFromImager();

	bool CoaterStartup();
	bool CoaterLoadReady();
	bool CoaterUnloadReady();
	bool CoaterOpenVacuum();
	bool CoaterCloseVacuum();
	bool CoaterStartWork();
	bool CoaterFinished();
	bool CoaterSetUnloadState();

	bool ImagerCheckMemory();
	bool ImagerGoLoadingPos();
	bool ImagerOpenVacuum();
	bool ImagerCloseVacuum();
	bool ImagerSetConfig();
	bool ImagerSetupParam();
	bool ImagerStartRun();
	bool ImagerFinished();

	//告警灯
	bool OpenAlarmLight(RobotParaType::RobotLightColor lightColor= RobotParaType::RobotLightColor::RED);

	bool IsRobotSimulated();
	bool IsCoaterSimulated();
	bool IsImagerSimulated();

	//检查是否任务已经被停止
	void CheckTaskEnd();
	bool FunMonitor(bool (SchedulerDriver::* taskFun)(), bool bAlarm = true);

	void ReadConfigValue(const QString& key, int& value, int defaultValue);
	void ReadImagerRetryErrors();

	bool CheckRobotready();
private:
	QThread* m_pThread;
	//QTimer* m_pTimer;

	ImagerDriver* m_pImagerDriver;
	EpsonRobotFlowCtrl* m_pEpsonRobotFlowCtrl;
	CoaterDriver* m_pCoaterDriver;
	DeviceWarningLights* m_pDeviceWarningLights;

	int m_MemoryTimeout;  //申请内存超时时间
	int m_ImagerVacuumTimeout;  //光机开关真空超时时间
	int m_ImagerVacuumMaxValue;  //光机真空最大值（开真空需要大于这个值）
	int m_ImagerVacuumMinValue;  //光机真空最小值（关真空需要小于这个值）
	int m_ImagingTimeout;  //光机拍照超时时间

	int m_CoaterVacuumTimeout;  //涂布机开关真空超时时间
	int m_CoaterVacuumMaxValue;  //涂布机真空最大值（开真空需要大于这个值）
	int m_CoaterVacuumMinValue;  //涂布机真空最小值（关真空需要小于这个值）
	int m_CoatingTimeout;  //涂布机生化超时时间
	int m_LoadTimeout;  //涂布机准备上料超时时间
	int m_UnloadTimeout;  //涂布机准备下料超时时间

	bool m_PauseState;  //是否处于暂停状态
	SchedulerTaskStateEnum m_NextTask;  //下一步任务

	RecordData m_RecordData;
	GeneralParaType::SqlCfgData m_CurChipData;  //芯片信息
	int m_EndCycle;  //设置的结束Cycle

	bool m_bFirstConfigSet;  //光机是否首次config设置
	bool m_bImagerFinished;  //光机拍照是否完成
	bool m_bImagerMemFailed;  //光机申请内存是否失败
	bool m_bVacuumOpenFailed;  //开真空是否失败
	bool m_RecycleChip;  //是否有错误需要回收
	bool m_CoatingSucceed;  //涂布机生化是否成功
	bool m_CoaterUnloading;  //涂布机是否正在下料
	
	bool m_bImagerRetryError;  //光机是否放生了可以重试的错误
	QSet<int> m_ImagerRetryErrors;  //光机可以进行重试的错误码
	int m_ImagerRetryMaxCount;  //光机重试拍照最大次数

	bool m_IsRobotSimulated;  //模拟机器臂
	bool m_IsCoaterSimulated;  //模拟涂布机
	bool m_IsImagerSimulated;  //模拟光机
};
