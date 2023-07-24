#include "RobotParaHandler.h"
#include "BgiLogger.h"
#include "RobotParaType.h"
#include "ParaCmd.h"
#include "BgiProcId.h"
#include "OutputProcId.h"
#include "ParaEngine.h"
//#include "EpsonRobotEventHandler.h"

RobotParaHandler::RobotParaHandler()
{}

RobotParaHandler::~RobotParaHandler()
{}

void RobotParaHandler::SetEpsonRobotEventHandler(EpsonRobotEventHandler* handler)
{
	BGI_ASSERT(handler != NULL);
	//connect(handler, SIGNAL(SendEpsonRobotParaDesc(QVariant)), this, SLOT(ReceiveRobotParaDesc(QVariant)));
}

void RobotParaHandler::ReceiveRobotParaDesc(QVariant desc)
{
	RobotParaType::RobotPara para = desc.value<RobotParaType::RobotPara>();

	ParaCmd para_cmd;
	para_cmd.Type = ParaCmd::SetValue;
	para_cmd.ParaEnum = OutputProc::Robot::ParaOpt;
	para_cmd.ProcType = BgiProc::EProcType::OutputProcType;
	para_cmd.SystemType = BgiProc::ESystemType::RobotType;
	para_cmd.SystemId = BgiProc::ESystemId::SystemA;
	para_cmd.Value = &para;
	para_cmd.ValueType = &typeid(RobotParaType::RobotPara);

	bool result = ParaEngine::Instance()->ParaCommand(para_cmd);
}
