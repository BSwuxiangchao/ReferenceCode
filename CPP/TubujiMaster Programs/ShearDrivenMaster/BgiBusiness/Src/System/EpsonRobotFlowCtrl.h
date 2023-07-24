/*
* @Abrief 调用Python脚本
* @note Eposn机械臂的相关流程用python脚本控制，这个类用于调python脚本方法
* @version 1.0
* @author wuxiangchao 2023-2-16
*/
#pragma once

#include "python.h"
#include "qstring.h"
#include "RobotParaType.h"

class EpsonRobotDriver;

class EpsonRobotFlowCtrl
{
public:
	~EpsonRobotFlowCtrl();
	static EpsonRobotFlowCtrl* Instance();
	bool TestMsgBox();

	void ApplicationSleep(int mSec);
	//重新加载脚本
	bool Ctrl_RealoadScript();

	/******************************************************************************
	* 功能描述：根据起点Index和终点Index移动机械臂
	* 时    间：2022/2/22
	* 入    参：iStartIndex：起点[0：AnyWhere，1：Home，2：中转槽等待区，3：风刀口，4：风刀口等待区，5：涂布机等待区，6：光机等待区，7：沥液区]
	* 入    参：iEndStart：终点  [             0：Home，1：中转槽等待区，2：风刀口，3：风刀口等待区，4：涂布机等待区，5：光机等待区，6：沥液区]
	* 出    参：无
	* 返    回：动作是否成功执行
	******************************************************************************/
	bool TransferPos(int iStartIndex, int iEndStart);

	/******************************************************************************
	* 功能描述：转运芯片
	* 时    间：2023/5/6
	* 入    参：iPosIndex：位置[0：中转槽，1：涂布机，2：光机]
	* 入    参：iOptIndex：操作[0：放芯片--->进，1：放芯片--->出，2：取芯片--->进，3：取芯片--->出]
	* 出    参：无
	* 返    回：动作是否成功执行
	******************************************************************************/
	bool OptChips(int iPosIndex, int iOptIndex);

	/*****************************调用Python转移 start**************************/
	//从AnyWhere到Home
	bool Ctrl_AnyWhere2Home();
	//从AnyWhere到中转槽等待区
	bool Ctrl_AnyWhere2TransferTankWait();
	//从AnyWhere到风刀口等待区
	bool Ctrl_AnyWhere2WindBladeWait();
	//从AnyWhere到涂布机生化等待区
	bool Ctrl_AnyWhere2CoaterWait();
	//从AnyWhere到光机拍照等待区
	bool Ctrl_AnyWhere2ImagerWait();
	//从AnyWhere到沥液区
	bool Ctrl_AnyWhere2DryWait();

	//从home到中转槽等待区
	bool Ctrl_Home2TransferTankWait();
	//从Home到风刀口等待区
	bool Ctrl_Home2WindBladeWait();
	//从Home到涂布机生化等待区
	bool Ctrl_Home2CoaterWait();
	//从Home到光机拍照等待区
	bool Ctrl_Home2ImagerWait();
	//从Home到沥液区
	bool Ctrl_Home2DryWait();

	//从中转槽等待区到Home
	bool Ctrl_TransferTankWait2Home();
	//从中转槽等待区到风刀口等待区
	bool Ctrl_TransferTankWait2WindBladeWait();
	//从中转槽等待区到涂布机等待区
	bool Ctrl_TransferTankWait2CoaterWait();
	//从中转槽等待区到光机等待区
	bool Ctrl_TransferTankWait2ImagerWait();
	//从中转槽等待区到沥液区
	bool Ctrl_TransferTankWait2DryWait();

	//从风刀口到风刀口等待区
	bool Ctrl_WindBlade2WindBladeWait();

	//从风刀口等待区到Home
	bool Ctrl_WindBladeWait2Home();
	//从风刀口等待区到中转槽等待区
	bool Ctrl_WindBladeWait2TransferTankWait();
	//从风刀口等待区到风刀口
	bool Ctrl_WindBladeWait2WindBlade();
	//从风刀口等待区到涂布机生化等待区
	bool Ctrl_WindBladeWait2CoaterWait();
	//从风刀口等待区到光机拍照等待区
	bool Ctrl_WindBladeWait2ImagerWait();
	//从风刀口等待区到沥液区
	bool Ctrl_WindBladeWait2DryWait();

	//从涂布机生化等待区到Home
	bool Ctrl_CoaterWait2Home();
	//从涂布机生化等待区到中转槽等待区
	bool Ctrl_CoaterWait2TransferTankWait();
	//从涂布机生化等待区到风刀口等待区
	bool Ctrl_CoaterWait2WindBladeWait();
	//从涂布机生化等待区到光机拍照等待区
	bool Ctrl_CoaterWait2ImagerWait();
	//从涂布机生化等待区到沥液区
	bool Ctrl_CoaterWait2DryWait();
	//从涂布机生化等待区到生化沥液区
	bool Ctrl_CoaterWait2CoaterDry();
	//从涂布机生化等待区到生化沥液区
	bool Ctrl_CoaterDry2CoaterWait();

	//从光机拍照等待区到Home
	bool Ctrl_ImagerWait2Home();
	//从光机拍照等待区到中转槽等待区
	bool Ctrl_ImagerWait2TransferTankWait();
	//从光机拍照等待区到风刀口等待区
	bool Ctrl_ImagerWait2WindBladeWait();
	//从光机拍照等待区到涂布机生化等待区
	bool Ctrl_ImagerWait2CoaterWait();
	//从光机拍照等待区到沥液区
	bool Ctrl_ImagerWait2DryWait();
	//从光机等待区到光机沥液区
	bool Ctrl_ImagerWait2ImagerDry();
	//从光机沥液区到光机拍照等待区
	bool Ctrl_ImagerDry2ImagerWait();

	//从沥液区到Home
	bool Ctrl_DryWait2Home();
	//从沥液区到中转槽等待区
	bool Ctrl_DryWait2TransferTankWait();
	//从沥液区到风刀口等待区
	bool Ctrl_DryWait2WindBladeWait();
	//从沥液区到涂布机生化等待区
	bool Ctrl_DryWait2CoaterWait();
	//从沥液区到光机拍照等待区
	bool Ctrl_DryWait2ImagerWait();

	/*****************************调用Python转移 end**************************/
	
	/*****************************调用Python操作芯片 Start**************************/
	//中转槽放芯片-进
	bool Ctrl_TransferTankPutChips_In();
	//中转槽放芯片-出
	bool Ctrl_TransferTankPutChips_Out();
	//中转槽取芯片-进
	bool Ctrl_TransferTankGetChips_In();
	//中转槽取芯片-出
	bool Ctrl_TransferTankGetChips_Out();
	//涂布机放芯片-进
	bool Ctrl_CoaterPutChips_In();
	//涂布机放芯片-出
	bool Ctrl_CoaterPutChips_Out();
	//涂布机取芯片-进
	bool Ctrl_CoaterGetChips_In();
	//涂布机取芯片-出
	bool Ctrl_CoaterGetChips_Out();
	//光机放芯片-进
	bool Ctrl_ImagerPutChips_In();
	//光机放芯片-出
	bool Ctrl_ImagerPutChips_Out();
	//光机取芯片-进
	bool Ctrl_ImagerGetChips_In();
	//光机取芯片-出
	bool Ctrl_ImagerGetChips_Out();
	/*****************************调用Python操作芯片 end**************************/

	/*****************************主控主流程调用 start**************************/
	//把芯片从回收槽送到涂布机
	bool GoCoaterFromRecycle();
	//把芯片释放给涂布机，机械臂退到生化等待区
	bool ReleaseChipToCoater();
	//机械臂从生化等待区到涂布机夹取芯片
	bool HoldChipFromCoater();
	//把芯片从涂布机送到回收槽
	bool GoRecycleFromCoater();

	//把芯片从回收槽送到光机
	bool GoImagerFromRecycle();
	//把芯片释放给光机，机械臂退到拍照等待区
	bool ReleaseChipToImager();
	//机械臂从拍照等待区到光机夹取芯片
	bool HoldChipFromImager();
	//把芯片从光机送到回收槽
	bool GoRecycleFromImager();

	//急停
	bool EmergencyStop();
	//关夹手
	bool CloseGripper();
	//开夹手
	bool OpenGripper();
	//吹风
	bool BlowChip();
	//信号灯 iColorType 0:灯灭  1：红  2：绿
	bool OperateWarningLights(RobotParaType::RobotLightColor ColorType);
	//判断是否有芯片
	bool HaveChipInTransferTank(int iSlotID);
	//是否处于ReadyMove状态
	bool IsMotorReadyMove();
	/*****************************主控主流程调用 end**************************/

private:
	EpsonRobotFlowCtrl();
	bool ConductCommandByName(QString strFuncName,bool bHigh = true);
	void InitFlowCtrl();
	bool ChangeSpeed(bool bHigh);
private:
	PyObject* m_pModule;
	EpsonRobotDriver* m_EpsonRobotDriver;
	bool m_bConducting;
	//当前操作芯片槽
	int m_iCurSlotID;
};

