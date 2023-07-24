#pragma once

#include <Ice/Ice.h>
#include "StageRunMgrIPC.h"

using namespace std;
using namespace BGI::RPC;

class StageRunMgrIPCCallbackI  : public StageRunMgrIPCCallback
{
public:
	StageRunMgrIPCCallbackI();
	~StageRunMgrIPCCallbackI();

	virtual void AddClient(const ::Ice::Identity& ident, const ::Ice::Current& current) override;
	virtual void SendScriptName(const ::std::string& scriptName, const ::Ice::Current& current) override;
	virtual void SendActionName(const ::std::string& actionName, const ::Ice::Current& current) override;
	virtual void SendFlowcellBarcode(const ::std::string& flowcellBarcode, const ::Ice::Current& current) override;
	virtual void SendPositionNumber(::Ice::Int cycleNum, const ::Ice::Current& current) override;
	virtual void SendPcntDone(::Ice::Int pctDone, const ::Ice::Current& current) override;
	virtual void SendScriptStatus(ScriptStateEnum curState, const ::Ice::Current& current) override;
	virtual void SendScriptMessageId(::Ice::Int messageId, const ::Ice::Current& current) override;
	virtual void SendStageMessage(const ::std::string& msgType, const ::std::string& message, const ::Ice::Current& current) override;
	virtual void SendQCValue(const QCDataValue& qcValue, const ::Ice::Current& current) override;
	virtual void SendQCValues(const QCDataValues& qcValues, const ::Ice::Current& current) override;
	virtual void SendAlarmMessageId(::Ice::Int messageId, const ::Ice::Current& current) override;
	virtual void SendLogMessageId(::Ice::Int messageId, const ::Ice::Current& current) override;
	virtual void SendRemainingTime(::Ice::Double remainingTimeMinute, const ::Ice::Current& current) override;
	virtual void SendDataList(const DataList& datas, const ::Ice::Current& current) override;
};
