#pragma once

#include <QObject>
#include <Ice/Ice.h>
#include "ImagerControlMgrIPC.h"
#include <QSharedPointer>
#include "ImagerParaType.h"

class ImagerControlMgrIPCCallbackI;
class ImagerDriver;

class ImagerControlMgrIPCClient  : public QObject
{
	Q_OBJECT

public:
	ImagerControlMgrIPCClient(ImagerDriver* imagerDriver);
	~ImagerControlMgrIPCClient();

	bool Connect();

	bool GoHome();
	bool Reset();
	bool GoLoadingPos();
	bool IsAtLoadPosition();
	bool SwitchVacuum(bool valveStatus, bool pumpStatus = true);
	
	bool ImagerConfigGet(ImagerParaType::ImagerConfigPara& data);
	bool ImagerConfigSet(const ImagerParaType::ImagerConfigPara& data);

	bool GetAllRunInformations(QList<ImagerParaType::BasecallRunPara>& infos);
	bool GetRunInformation(const QString& slideID, int& startCycle);
	bool ImagerSetup(const ImagerParaType::ImagerSetupPara& info);
	bool SetupRun(const ImagerParaType::ImagerSetupPara& info);
	bool ReleaseRun(const ImagerParaType::ImagerSetupPara& info);
	bool StartRun();

	bool GetVersions(QString& bcsVer, QString& sbcVer, QString& imagerVer);
	bool GetDiskSpace(double& consumptionValue, double& availableValue);

	static ImagerParaType::ImagerSetupPara SwitchImagerSetupData(const BGI::RPC::ImagerSetupInfo& dataOri);

private:
	ImagerParaType::ImagerConfigPara SwitchImagerConfigData(const BGI::RPC::ImagerConfigData& dataOri);
	BGI::RPC::ImagerConfigData SwitchImagerConfigData(const ImagerParaType::ImagerConfigPara& dataOri);

	BGI::RPC::ImagerSetupInfo SwitchImagerSetupData(const ImagerParaType::ImagerSetupPara& dataOri);

	ImagerParaType::BasecallRunPara SwitchBasecallRunData(const BGI::RPC::RunInformation& dataOri);

private:
	QString m_ImagerIP;
	QString m_ImagerPort;
	QString m_ImagerCallbackPort;

	Ice::CommunicatorPtr m_IceHolder;
	BGI::RPC::ImagerControlMgrIPCPrx m_ImagerPrx;
	BGI::RPC::ImagerControlMgrIPCCallbackPrx m_ImagerCallbackPrx;
	QSharedPointer<ImagerControlMgrIPCCallbackI> m_pImagerCallback;

	ImagerDriver* m_ImagerDriver;
};
