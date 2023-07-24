#include "StageRunMgrIPCCallbackI.h"

StageRunMgrIPCCallbackI::StageRunMgrIPCCallbackI()
{}

StageRunMgrIPCCallbackI::~StageRunMgrIPCCallbackI()
{}

void StageRunMgrIPCCallbackI::AddClient(const ::Ice::Identity& ident, const ::Ice::Current& current)
{}

void StageRunMgrIPCCallbackI::SendScriptName(const ::std::string& scriptName, const ::Ice::Current& current)
{}

void StageRunMgrIPCCallbackI::SendActionName(const ::std::string& actionName, const ::Ice::Current& current)
{}

void StageRunMgrIPCCallbackI::SendFlowcellBarcode(const ::std::string& flowcellBarcode, const ::Ice::Current& current)
{}

void StageRunMgrIPCCallbackI::SendPositionNumber(::Ice::Int cycleNum, const ::Ice::Current& current)
{}

void StageRunMgrIPCCallbackI::SendPcntDone(::Ice::Int pctDone, const ::Ice::Current& current)
{}

void StageRunMgrIPCCallbackI::SendScriptStatus(ScriptStateEnum curState, const ::Ice::Current& current)
{}

void StageRunMgrIPCCallbackI::SendScriptMessageId(::Ice::Int messageId, const ::Ice::Current& current)
{}

void StageRunMgrIPCCallbackI::SendStageMessage(const ::std::string& msgType, const ::std::string& message, const ::Ice::Current& current)
{}

void StageRunMgrIPCCallbackI::SendQCValue(const QCDataValue& qcValue, const ::Ice::Current& current)
{}

void StageRunMgrIPCCallbackI::SendQCValues(const QCDataValues& qcValues, const ::Ice::Current& current)
{}

void StageRunMgrIPCCallbackI::SendAlarmMessageId(::Ice::Int messageId, const ::Ice::Current& current)
{}

void StageRunMgrIPCCallbackI::SendLogMessageId(::Ice::Int messageId, const ::Ice::Current& current)
{}

void StageRunMgrIPCCallbackI::SendRemainingTime(::Ice::Double remainingTimeMinute, const ::Ice::Current& current)
{}

void StageRunMgrIPCCallbackI::SendDataList(const DataList& datas, const ::Ice::Current& current)
{}
