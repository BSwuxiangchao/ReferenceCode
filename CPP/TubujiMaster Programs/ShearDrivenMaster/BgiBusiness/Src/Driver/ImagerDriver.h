#pragma once

#include <QObject>
#include <QSharedPointer>
#include <QTimer>
#include <QDateTime>
#include "ImagerParaType.h"
#include "ParaType.h"

class ImagerControlMgrIPCClient;

class BGIBUSINE_CLASS_EXPORT ImagerDriver  : public QObject
{
	Q_OBJECT

public:
	ImagerDriver(QObject *parent = nullptr);
	~ImagerDriver();

	void Heartbeat();
	void GetConnStatus();

	bool GoLoadingPos();
	bool IsAtLoadPosition();
	bool GoHome();
	bool SwitchVacuum(bool bOpen);
	bool Reset();
	bool ImagerConfigGet(ImagerParaType::ImagerConfigPara& data);
	bool ImagerConfigGet();
	bool ImagerConfigSet(const ImagerParaType::ImagerConfigPara& data);

	bool GetBasecallMems();
	bool CheckMemory(const QString& slideID);
	bool ImagerSetup(const GeneralParaType::SequencingTableDesc& sequencingInfo);
	bool SetupRun(const GeneralParaType::SequencingTableDesc& sequencingInfo);
	bool ReleaseRun(const GeneralParaType::SequencingTableDesc& sequencingInfo);
	bool ReleaseRun(const QString& slideID);
	bool StartRun();

	bool GetVersions();
	bool GetDiskSpace();

	void RecvImagerTaskEnd(const ImagerParaType::ImagerSetupPara& info, int error);
	void RecvImagerError(int error);
	void RecvImagerRunInfo(const ImagerParaType::ImagerRunPara& info);
	void RecvImagerVacuumValue(const QString& value);
	void RecvImagerStatus(int basecall, int server, int imager);
	void RecvImagerData(const QString& key, const QString& value);

	float GetVacuumValue();

	//更新机械臂进入状态
	void UpdateRobotSafetyStatus(bool bSafety);

	static void ReadExposureFile(QString path, QList<double>& eExposureTime, QList<double>& exposureFactors);

signals:
	void SignalImagerConnectStatus(bool bSuccess);
	void SignalImagerConfigData(const ImagerParaType::ImagerConfigPara& data);
	void SignalImagerConfigSet(bool bSuccess);
	void SignalImagerRunInfo(const ImagerParaType::ImagerRunPara& info);
	void SignalImagerVacuumValue(float value);
	void SignalImagerStatus(int basecall, int server, int imager);
	void SignalBasecallRunInfos(const QList<ImagerParaType::BasecallRunPara>& infos);
	void SignalImagerData(const QString&, const QString&);
	void SignalImagerSafePos(bool);
	void SignalImagerVersions(const QString&, const QString&, const QString&);
	void SignalBasecallDiskSpace(double, double);

private slots:
	void CheckHeartbeat();

private:
	void SetConnectStatus(bool bConnected);
	void ResetValue();

	ImagerParaType::ImagerSetupPara GetImagerSetupPara(const GeneralParaType::SequencingTableDesc& sequencingInfo);
	ImagerParaType::ImagerSetupPara GetImagerSetupPara(const ImagerParaType::BasecallRunPara& info);

	QString GetAlarmString(int errorCode);

private:
	QThread* m_pThread;
	QTimer* m_pTimer;

	QSharedPointer<ImagerControlMgrIPCClient> m_pImagerIPCClient;

	QDateTime m_LastHeartbeat;
	bool m_bConnected;
	bool m_bSafePos;

	float m_VacuumValue;  //光机真空值

	int m_StateBasecall;
	int m_StateServer;
	int m_StateImager;
};
