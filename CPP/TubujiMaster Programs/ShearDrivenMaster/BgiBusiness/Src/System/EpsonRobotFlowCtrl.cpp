#include "EpsonRobotFlowCtrl.h"
#include <qstring.h>
#include <qcoreapplication.h>
#include "BgiLogger.h"
#include "BgiFileTools.h"
#include "EpsonRobotDriver.h"
#include "MasterDevices.h"
#include "ErrorCodeTranslate.h"
#include <QThread>

//脚本路径
const QString gc_strPyModuleName = "RobotTransferFlow";

EpsonRobotFlowCtrl::EpsonRobotFlowCtrl()
	:m_pModule(NULL)
	, m_bConducting(false)
	, m_iCurSlotID(1)
{
	InitFlowCtrl();
	m_EpsonRobotDriver = MasterDevices::Instance()->GetEpsonRobotDriver();
}

EpsonRobotFlowCtrl::~EpsonRobotFlowCtrl()
{
	Py_Finalize();
}

EpsonRobotFlowCtrl* EpsonRobotFlowCtrl::Instance()
{
	static EpsonRobotFlowCtrl instance;
	return &instance;
}

void EpsonRobotFlowCtrl::InitFlowCtrl()
{
	Py_Initialize();//使用python之前，要调用Py_Initialize();这个函数进行初始化

	if (!Py_IsInitialized())
	{
		BGI_WARNING("python脚本初始化失败！");
		return;
	}
	//2、初始化python系统文件路径，保证可以访问到 .py文件
	QString path;
	BgiFileTools::GetDataDirPath(path);

	path += "//Data//Input//";
	PyRun_SimpleString("import sys");
	PyRun_SimpleString(QString("sys.path.append('%1')").arg(path).toStdString().c_str());

	//BGI_WARNING(QString("当前目录：%1").arg(QCoreApplication::applicationDirPath()).toStdString().c_str());
	m_pModule = PyImport_ImportModule(gc_strPyModuleName.toStdString().c_str());//这里是要调用的文件名
	if (m_pModule == NULL)
	{
		PyErr_Print();
		BGI_WARNING(QString("未找到脚本文件：%1").arg(gc_strPyModuleName).toStdString().c_str());
	}
}

bool EpsonRobotFlowCtrl::ChangeSpeed(bool bHigh)
{
	bool bOk = false;
	if (!m_EpsonRobotDriver->IsMotorPowerHigh())
		m_EpsonRobotDriver->SetPower(true);
	int iTotal = 3;
	int iCount = 0;
	while (iCount <iTotal)
	{
		if (bHigh)
			bOk = m_EpsonRobotDriver->MotorSetHighSpeed();
		else
			bOk = m_EpsonRobotDriver->MotorSetLowSpeed();
		if (bOk)
			return true;
		iCount++;
	}
	if (bHigh)
		return true;
	return ErrorCodeTranslate::Instance()->FalseWithTip(__func__);
}

void EpsonRobotFlowCtrl::ApplicationSleep(int mSec)
{
	QThread::msleep(mSec);
	/*while (mSec)
	{
		QCoreApplication::processEvents();
		QThread::msleep(1);
		mSec--;
	}*/
}

bool EpsonRobotFlowCtrl::TestMsgBox()
{
	return ConductCommandByName("TestMsgBox");
}

//重新加载脚本
bool EpsonRobotFlowCtrl::Ctrl_RealoadScript()
{
	Py_Finalize();
	InitFlowCtrl();
	return true;
}

/******************************************************************************
	* 功能描述：根据起点Index和终点Index移动机械臂
	* 时    间：2022/2/22
	* 入    参：iStartIndex：起点[0：AnyWhere，1：Home，2：中转槽等待区，3：风刀口，4：风刀口等待区，5：涂布机等待区，6：光机等待区，7：沥液区]
	* 入    参：iEndStart：终点  [             0：Home，1：中转槽等待区，2：风刀口，3：风刀口等待区，4：涂布机等待区，5：光机等待区，6：沥液区]
	* 出    参：无
	* 返    回：动作是否成功执行
******************************************************************************/
bool EpsonRobotFlowCtrl::TransferPos(int iStartIndex, int iEndIndex)
{
	if (iStartIndex == iEndIndex+1)
		return true;
	if (iStartIndex == 0)	//AnyWhere->
	{
		if (iEndIndex == 0)
			return Ctrl_AnyWhere2Home();
		else if (iEndIndex == 1)
			return Ctrl_AnyWhere2TransferTankWait();
		else if (iEndIndex == 3)
			return Ctrl_AnyWhere2WindBladeWait();
		else if (iEndIndex == 4)
			return Ctrl_AnyWhere2CoaterWait();
		else if (iEndIndex == 5)
			return Ctrl_AnyWhere2ImagerWait();
		else if (iEndIndex == 6)
			return Ctrl_AnyWhere2DryWait();
	}
	else if (iStartIndex == 1)	//Home->
	{
		if (iEndIndex == 1)
			return Ctrl_Home2TransferTankWait();
		else if (iEndIndex == 3)
			return Ctrl_Home2WindBladeWait();
		else if (iEndIndex == 4)
			return Ctrl_Home2CoaterWait();
		else if (iEndIndex == 5)
			return Ctrl_Home2ImagerWait();
		else if (iEndIndex == 6)
			return Ctrl_Home2DryWait();
	}
	else if (iStartIndex == 2)	//中转槽等待区->
	{
		if (iEndIndex == 0)
			return Ctrl_TransferTankWait2Home();
		else if (iEndIndex == 3)
			return Ctrl_TransferTankWait2WindBladeWait();
		else if (iEndIndex == 4)
			return Ctrl_TransferTankWait2CoaterWait();
		else if (iEndIndex == 5)
			return Ctrl_TransferTankWait2ImagerWait();
		else if (iEndIndex == 6)
			return Ctrl_TransferTankWait2DryWait();
	}
	else if (iStartIndex == 3) //风刀口->
	{
		if (iEndIndex == 3)
			return Ctrl_WindBlade2WindBladeWait();
	}
	else if (iStartIndex == 4)	//风刀口等待区->
	{
		if (iEndIndex == 0)
			return Ctrl_WindBladeWait2Home();
		else if (iEndIndex == 1)
			return Ctrl_WindBladeWait2TransferTankWait();
		else if (iEndIndex == 2)
			return Ctrl_WindBladeWait2WindBlade();
		else if (iEndIndex == 4)
			return Ctrl_WindBladeWait2CoaterWait();
		else if (iEndIndex == 5)
			return Ctrl_WindBladeWait2ImagerWait();
		else if (iEndIndex == 6)
			return Ctrl_WindBladeWait2DryWait();
	}
	else if (iStartIndex == 5)	//涂布机等待区->
	{
		if (iEndIndex == 0)
			return Ctrl_CoaterWait2Home();
		else if (iEndIndex == 1)
			return Ctrl_CoaterWait2TransferTankWait();
		else if (iEndIndex == 3)
			return Ctrl_CoaterWait2WindBladeWait();
		else if (iEndIndex == 5)
			return Ctrl_CoaterWait2ImagerWait();
		else if (iEndIndex == 6)
			return Ctrl_CoaterWait2DryWait();
	}
	else if (iStartIndex == 6)	//光机等待区->
	{
		if (iEndIndex == 0)
			return Ctrl_ImagerWait2Home();
		else if (iEndIndex == 1)
			return Ctrl_ImagerWait2TransferTankWait();
		else if (iEndIndex == 3)
			return Ctrl_ImagerWait2WindBladeWait();
		else if (iEndIndex == 4)
			return Ctrl_ImagerWait2CoaterWait();
		else if (iEndIndex == 6)
			return Ctrl_ImagerWait2DryWait();
	}
	else if (iStartIndex == 7)	//沥液区
	{
		if (iEndIndex == 0)
			return Ctrl_DryWait2Home();
		else if (iEndIndex == 1)
			return Ctrl_DryWait2TransferTankWait();
		else if (iEndIndex == 3)
			return Ctrl_DryWait2WindBladeWait();
		else if (iEndIndex == 4)
			return Ctrl_DryWait2CoaterWait();
		else if (iEndIndex == 5)
			return Ctrl_DryWait2ImagerWait();
	}
	ErrorCodeTranslate::Instance()->PromptInfo("转移提示", "为避免撞机，当前转移指令不能执行！！！", gc_colorRed);
	return false;
}

/******************************************************************************
	* 功能描述：转运芯片
	* 时    间：2023/5/6
	* 入    参：iPosIndex：位置[0：中转槽，1：涂布机，2：光机]
	* 入    参：iOptIndex：操作[0：放芯片--->进，1：放芯片--->出，2：取芯片--->进，3：取芯片--->出]
	* 出    参：无
	* 返    回：动作是否成功执行
******************************************************************************/
bool EpsonRobotFlowCtrl::OptChips(int iPosIndex, int iOptIndex)
{
	if (0 == iPosIndex)
	{
		if (0 == iOptIndex)
			return Ctrl_TransferTankPutChips_In();
		else if (1 == iOptIndex)
			return Ctrl_TransferTankPutChips_Out();
		else if (2 == iOptIndex)
			return Ctrl_TransferTankGetChips_In();
		else if (3 == iOptIndex)
			return Ctrl_TransferTankGetChips_Out();
		else if (4 == iOptIndex)
			return Ctrl_TransferTankWait2DryWait();
		else if (5 == iOptIndex)
			return Ctrl_DryWait2TransferTankWait();
	}
	else if (1 == iPosIndex)
	{
		if (0 == iOptIndex)
			return Ctrl_CoaterPutChips_In();
		else if (1 == iOptIndex)
			return Ctrl_CoaterPutChips_Out();
		else if (2 == iOptIndex)
			return Ctrl_CoaterGetChips_In();
		else if (3 == iOptIndex)
			return Ctrl_CoaterGetChips_Out();
		else if (4 == iOptIndex)
			return Ctrl_CoaterWait2CoaterDry();
		else if (5 == iOptIndex)
			return Ctrl_CoaterDry2CoaterWait();
	}
	else if (2 == iPosIndex)
	{
		if (0 == iOptIndex)
			return Ctrl_ImagerPutChips_In();
		else if (1 == iOptIndex)
			return Ctrl_ImagerPutChips_Out();
		else if (2 == iOptIndex)
			return Ctrl_ImagerGetChips_In();
		else if (3 == iOptIndex)
			return Ctrl_ImagerGetChips_Out();
		else if (4 == iOptIndex)
			return Ctrl_ImagerWait2ImagerDry();
		else if (5 == iOptIndex)
			return Ctrl_ImagerDry2ImagerWait();
	}
	return false;
}

bool EpsonRobotFlowCtrl::ConductCommandByName(QString strFuncName, bool bHigh)
{
	if (m_bConducting)
	{
		ErrorCodeTranslate::Instance()->RecordInfo("函数调用", "正在执行脚本动作，不能立即执行下一窗口点击事件", gc_colorYellow, true);
		return false;
	}
	if (!ChangeSpeed(bHigh))
		return ErrorCodeTranslate::Instance()->FalseWithTip(strFuncName);
	if (m_pModule == NULL)
		return ErrorCodeTranslate::Instance()->FalseWithTip(strFuncName);
	PyObject* pFunc1 = NULL;
	pFunc1 = PyObject_GetAttrString(m_pModule, strFuncName.toStdString().c_str());
	try
	{
		m_bConducting = true;
		if (pFunc1 == NULL)
		{
			ErrorCodeTranslate::Instance()->RecordInfo("函数调用", QString("Python 脚本中未找到 %1 函数").arg(strFuncName), gc_colorYellow);
			m_bConducting = false;
			return false;
		}
		PyObject* pReturn = PyObject_CallObject(pFunc1, NULL);
		int nResult = 0;
		PyArg_Parse(pReturn, "i", &nResult);
		//BGI_MESSAGE(QString("Python返回值：%1").arg(nResult).toStdString().c_str());
		m_bConducting = false;
		if (1 == nResult)
			return true;
		else if (0 == nResult)
			return ErrorCodeTranslate::Instance()->FalseWithTip(strFuncName);
	}
	catch (const std::exception&)
	{
		m_bConducting = false;
		return ErrorCodeTranslate::Instance()->FalseWithTip(strFuncName);
	}
	
	return true;
}

//从AnyWhere到Home
bool EpsonRobotFlowCtrl::Ctrl_AnyWhere2Home()
{
	return ConductCommandByName("Ctrl_AnyWhere2Home");
}

//从AnyWhere到中转槽等待区
bool EpsonRobotFlowCtrl::Ctrl_AnyWhere2TransferTankWait()
{
	return ConductCommandByName("Ctrl_AnyWhere2TransferTankWait");
}

//从AnyWhere到风刀口等待区
bool EpsonRobotFlowCtrl::Ctrl_AnyWhere2WindBladeWait()
{
	return ConductCommandByName("Ctrl_AnyWhere2WindBladeWait");
}

//从AnyWhere到涂布机生化等待区
bool EpsonRobotFlowCtrl::Ctrl_AnyWhere2CoaterWait()
{
	return ConductCommandByName("Ctrl_AnyWhere2CoaterWait");
}

//从AnyWhere到光机拍照等待区
bool EpsonRobotFlowCtrl::Ctrl_AnyWhere2ImagerWait()
{
	return ConductCommandByName("Ctrl_AnyWhere2ImagerWait");
}

//从AnyWhere到沥液区
bool EpsonRobotFlowCtrl::Ctrl_AnyWhere2DryWait()
{
	return ConductCommandByName("Ctrl_AnyWhere2DryWait");
}

//从home到中转槽等待区
bool EpsonRobotFlowCtrl::Ctrl_Home2TransferTankWait()
{
	return ConductCommandByName("Ctrl_Home2TransferTankWait");
}

//从Home到风刀口等待区
bool EpsonRobotFlowCtrl::Ctrl_Home2WindBladeWait()
{
	return ConductCommandByName("Ctrl_Home2WindBladeWait");
}

//从Home到涂布机生化等待区
bool EpsonRobotFlowCtrl::Ctrl_Home2CoaterWait()
{
	return ConductCommandByName("Ctrl_Home2CoaterWait");
}

//从Home到光机拍照等待区
bool EpsonRobotFlowCtrl::Ctrl_Home2ImagerWait()
{
	return ConductCommandByName("Ctrl_Home2ImagerWait");
}

//从Home到沥液区
bool EpsonRobotFlowCtrl::Ctrl_Home2DryWait()
{
	return ConductCommandByName("Ctrl_Home2DryWait");
}

//从中转槽等待区到Home
bool EpsonRobotFlowCtrl::Ctrl_TransferTankWait2Home()
{
	return ConductCommandByName("Ctrl_TransferTankWait2Home");
}

//从中转槽等待区到风刀口等待区
bool EpsonRobotFlowCtrl::Ctrl_TransferTankWait2WindBladeWait()
{
	return ConductCommandByName("Ctrl_TransferTankWait2WindBladeWait");
}

//从中转槽等待区到涂布机等待区
bool EpsonRobotFlowCtrl::Ctrl_TransferTankWait2CoaterWait()
{
	return ConductCommandByName("Ctrl_TransferTankWait2CoaterWait");
}

//从中转槽等待区到光机等待区
bool EpsonRobotFlowCtrl::Ctrl_TransferTankWait2ImagerWait()
{
	return ConductCommandByName("Ctrl_TransferTankWait2ImagerWait");
}

//从中转槽等待区到沥液区
bool EpsonRobotFlowCtrl::Ctrl_TransferTankWait2DryWait()
{
	return ConductCommandByName("Ctrl_TransferTankWait2DryWait");
}

//从风刀口到风刀口等待区
bool EpsonRobotFlowCtrl::Ctrl_WindBlade2WindBladeWait()
{
	return ConductCommandByName("Ctrl_WindBlade2WindBladeWait",false);
}

//从风刀口等待区到Home
bool EpsonRobotFlowCtrl::Ctrl_WindBladeWait2Home()
{
	return ConductCommandByName("Ctrl_WindBladeWait2Home");
}

//从风刀口等待区到中转槽等待区
bool EpsonRobotFlowCtrl::Ctrl_WindBladeWait2TransferTankWait()
{
	return ConductCommandByName("Ctrl_WindBladeWait2TransferTankWait");
}

//从风刀口等待区到风刀口
bool EpsonRobotFlowCtrl::Ctrl_WindBladeWait2WindBlade()
{
	return ConductCommandByName("Ctrl_WindBladeWait2WindBlade",false);
}

//从风刀口等待区到涂布机生化等待区
bool EpsonRobotFlowCtrl::Ctrl_WindBladeWait2CoaterWait()
{
	return ConductCommandByName("Ctrl_WindBladeWait2CoaterWait");
}

//从风刀口等待区到光机拍照等待区
bool EpsonRobotFlowCtrl::Ctrl_WindBladeWait2ImagerWait()
{
	return ConductCommandByName("Ctrl_WindBladeWait2ImagerWait");
}

//从风刀口等待区到沥液区
bool EpsonRobotFlowCtrl::Ctrl_WindBladeWait2DryWait()
{
	return ConductCommandByName("Ctrl_WindBladeWait2DryWait");
}

//从涂布机生化等待区到Home
bool EpsonRobotFlowCtrl::Ctrl_CoaterWait2Home()
{
	return ConductCommandByName("Ctrl_CoaterWait2Home");
}

//从涂布机生化等待区到中转槽等待区
bool EpsonRobotFlowCtrl::Ctrl_CoaterWait2TransferTankWait()
{
	return ConductCommandByName("Ctrl_CoaterWait2TransferTankWait");
}

//从涂布机生化等待区到风刀口等待区
bool EpsonRobotFlowCtrl::Ctrl_CoaterWait2WindBladeWait()
{
	return ConductCommandByName("Ctrl_CoaterWait2WindBladeWait");
}

//从涂布机生化等待区到光机拍照等待区
bool EpsonRobotFlowCtrl::Ctrl_CoaterWait2ImagerWait()
{
	return ConductCommandByName("Ctrl_CoaterWait2ImagerWait");
}

//从涂布机生化等待区到沥液区
bool EpsonRobotFlowCtrl::Ctrl_CoaterWait2DryWait()
{
	return ConductCommandByName("Ctrl_CoaterWait2DryWait");
}

//从涂布机生化等待区到生化沥液区
bool EpsonRobotFlowCtrl::Ctrl_CoaterWait2CoaterDry()
{
	return ConductCommandByName("Ctrl_CoaterWait2CoaterDry",false);
}

//从涂布机生化等待区到生化沥液区
bool EpsonRobotFlowCtrl::Ctrl_CoaterDry2CoaterWait()
{
	return ConductCommandByName("Ctrl_CoaterDry2CoaterWait",false);
}

//从光机拍照等待区到Home
bool EpsonRobotFlowCtrl::Ctrl_ImagerWait2Home()
{
	return ConductCommandByName("Ctrl_ImagerWait2Home");
}

//从光机拍照等待区到中转槽等待区
bool EpsonRobotFlowCtrl::Ctrl_ImagerWait2TransferTankWait()
{
	return ConductCommandByName("Ctrl_ImagerWait2TransferTankWait");
}

//从光机拍照等待区到风刀口等待区
bool EpsonRobotFlowCtrl::Ctrl_ImagerWait2WindBladeWait()
{
	return ConductCommandByName("Ctrl_ImagerWait2WindBladeWait");
}

//从光机拍照等待区到涂布机生化等待区
bool EpsonRobotFlowCtrl::Ctrl_ImagerWait2CoaterWait()
{
	return ConductCommandByName("Ctrl_ImagerWait2CoaterWait");
}

//从光机拍照等待区到沥液区
bool EpsonRobotFlowCtrl::Ctrl_ImagerWait2DryWait()
{
	return ConductCommandByName("Ctrl_ImagerWait2DryWait");
}

//从光机等待区到光机沥液区
bool EpsonRobotFlowCtrl::Ctrl_ImagerWait2ImagerDry() 
{
	return ConductCommandByName("Ctrl_ImagerWait2ImagerDry",false);
}

//从光机沥液区到光机拍照等待区
bool EpsonRobotFlowCtrl::Ctrl_ImagerDry2ImagerWait()
{
	return ConductCommandByName("Ctrl_ImagerDry2ImagerWait",false);
}

//从沥液区到Home
bool EpsonRobotFlowCtrl::Ctrl_DryWait2Home()
{
	return ConductCommandByName("Ctrl_DryWait2Home");
}

//从沥液区到中转槽等待区
bool EpsonRobotFlowCtrl::Ctrl_DryWait2TransferTankWait()
{
	return ConductCommandByName("Ctrl_DryWait2TransferTankWait");
}

//从沥液区到风刀口等待区
bool EpsonRobotFlowCtrl::Ctrl_DryWait2WindBladeWait()
{
	return ConductCommandByName("Ctrl_DryWait2WindBladeWait");
}

//从沥液区到涂布机生化等待区
bool EpsonRobotFlowCtrl::Ctrl_DryWait2CoaterWait()
{
	return ConductCommandByName("Ctrl_DryWait2CoaterWait");
}

//从沥液区到光机拍照等待区
bool EpsonRobotFlowCtrl::Ctrl_DryWait2ImagerWait()
{
	return ConductCommandByName("Ctrl_DryWait2ImagerWait");
}

//中转槽放芯片-进
bool EpsonRobotFlowCtrl::Ctrl_TransferTankPutChips_In()
{
	if (HaveChipInTransferTank(m_iCurSlotID))
		return ErrorCodeTranslate::Instance()->FalseWithTip(__func__);
	return ConductCommandByName("Ctrl_TransferTankPutChips_In",false);
}

//中转槽放芯片-出
bool EpsonRobotFlowCtrl::Ctrl_TransferTankPutChips_Out()
{
	return ConductCommandByName("Ctrl_TransferTankPutChips_Out",false);
}

//中转槽取芯片-进
bool EpsonRobotFlowCtrl::Ctrl_TransferTankGetChips_In()
{
	return ConductCommandByName("Ctrl_TransferTankGetChips_In",false);
}

//中转槽取芯片-出
bool EpsonRobotFlowCtrl::Ctrl_TransferTankGetChips_Out()
{
	return ConductCommandByName("Ctrl_TransferTankGetChips_Out",false);
}

//涂布机放芯片-进
bool EpsonRobotFlowCtrl::Ctrl_CoaterPutChips_In()
{
	return ConductCommandByName("Ctrl_CoaterPutChips_In",false);
}

//涂布机放芯片-出
bool EpsonRobotFlowCtrl::Ctrl_CoaterPutChips_Out()
{
	return ConductCommandByName("Ctrl_CoaterPutChips_Out",false);
}

//涂布机取芯片-进
bool EpsonRobotFlowCtrl::Ctrl_CoaterGetChips_In()
{
	return ConductCommandByName("Ctrl_CoaterGetChips_In",false);
}

//涂布机取芯片-出
bool EpsonRobotFlowCtrl::Ctrl_CoaterGetChips_Out()
{
	return ConductCommandByName("Ctrl_CoaterGetChips_Out",false);
}

//光机放芯片-进
bool EpsonRobotFlowCtrl::Ctrl_ImagerPutChips_In()
{
	return ConductCommandByName("Ctrl_ImagerPutChips_In",false);
}

//光机放芯片-出
bool EpsonRobotFlowCtrl::Ctrl_ImagerPutChips_Out()
{
	return ConductCommandByName("Ctrl_ImagerPutChips_Out",false);
}

//光机取芯片-进
bool EpsonRobotFlowCtrl::Ctrl_ImagerGetChips_In()
{
	return ConductCommandByName("Ctrl_ImagerGetChips_In",false);
}

//光机取芯片-出
bool EpsonRobotFlowCtrl::Ctrl_ImagerGetChips_Out()
{
	return ConductCommandByName("Ctrl_ImagerGetChips_Out",false);
}

/*****************************主控主流程调用 start**************************/

//把芯片从回收槽送到涂布机
bool EpsonRobotFlowCtrl::GoCoaterFromRecycle()
{
	bool bOk = false;
	//从AnyWhere 到中转槽等待区
	bOk = Ctrl_AnyWhere2TransferTankWait();
	if (!bOk)
		return bOk;
	bOk = HaveChipInTransferTank(m_iCurSlotID);
	if(!bOk)
		return ErrorCodeTranslate::Instance()->FalseWithTip(__func__);
	bOk = Ctrl_TransferTankGetChips_In();
	if (!bOk)
		return bOk;
	bOk = CloseGripper();
	if (!bOk)
		return bOk;
	bOk = Ctrl_TransferTankGetChips_Out();
	if (!bOk)
		return bOk;
	//从中转槽到风刀口
	bOk = Ctrl_TransferTankWait2WindBladeWait();
	if (!bOk)
		return bOk;
	//吹风 
	bOk = BlowChip();
	if (!bOk)
		return bOk;
	//从风刀口等待区到涂布机生化等待区
	bOk = Ctrl_WindBladeWait2CoaterWait();
	if (!bOk)
		return bOk;
	bOk = Ctrl_CoaterPutChips_In();
	if (!bOk)
		return bOk;
	return bOk;
}

//把芯片释放给涂布机，机械臂退到生化等待区
bool EpsonRobotFlowCtrl::ReleaseChipToCoater()
{
	bool bOk = false;
	bOk = OpenGripper();
	if (!bOk)
		return bOk;
	bOk = Ctrl_CoaterPutChips_Out();
	if (!bOk)
		return bOk;
	return bOk;
}

//机械臂从生化等待区到涂布机夹取芯片
bool EpsonRobotFlowCtrl::HoldChipFromCoater()
{
	bool bOk = false;
	bOk = Ctrl_CoaterGetChips_In();
	if (!bOk)
		return bOk;
	bOk = CloseGripper();
	if (!bOk)
		return bOk;
	return bOk;
}

//把芯片从涂布机送到回收槽
bool EpsonRobotFlowCtrl::GoRecycleFromCoater()
{
	bool bOk = false;
	bOk = Ctrl_CoaterGetChips_Out();
	if (!bOk)
		return bOk;
	//从涂布机生化等待区到生化沥液区,停 1.5 S
	bOk = Ctrl_CoaterWait2CoaterDry();
	if (!bOk)
		return bOk;
	ApplicationSleep(1500);
	bOk = Ctrl_CoaterDry2CoaterWait();
	if (!bOk)
		return bOk;
	bOk = Ctrl_CoaterWait2TransferTankWait();
	if (!bOk)
		return bOk;
	bOk = Ctrl_TransferTankPutChips_In();
	if (!bOk)
		return bOk;
	bOk = OpenGripper();
	if (!bOk)
		return bOk;
	bOk = Ctrl_TransferTankPutChips_Out();
	if (!bOk)
		return bOk;
	return bOk;
}

//把芯片从回收槽送到光机
bool EpsonRobotFlowCtrl::GoImagerFromRecycle()
{
	bool bOk = false;
	//从AnyWhere 到中转槽等待区
	bOk = Ctrl_AnyWhere2TransferTankWait();
	if (!bOk)
		return bOk;
	bOk = HaveChipInTransferTank(m_iCurSlotID);
	if (!bOk)
		return ErrorCodeTranslate::Instance()->FalseWithTip(__func__);
	bOk = Ctrl_TransferTankGetChips_In();
	if (!bOk)
		return bOk;
	bOk = CloseGripper();
	if (!bOk)
		return bOk;
	bOk = Ctrl_TransferTankGetChips_Out();
	if (!bOk)
		return bOk;
	//从中转槽到风刀口
	bOk = Ctrl_TransferTankWait2WindBladeWait();
	if (!bOk)
		return bOk;
	//吹风 
	bOk = BlowChip();
	if (!bOk)
		return bOk;
	//从风刀口等待区到光机机生化等待区
	bOk = Ctrl_WindBladeWait2ImagerWait();
	if (!bOk)
		return bOk;
	bOk = Ctrl_ImagerPutChips_In();
	if (!bOk)
		return bOk;
	return bOk;
}

//把芯片释放给光机，机械臂退到拍照等待区
bool EpsonRobotFlowCtrl::ReleaseChipToImager()
{
	bool bOk = false;
	bOk = OpenGripper();
	if (!bOk)
		return bOk;
	bOk = Ctrl_ImagerPutChips_Out();
	if (!bOk)
		return bOk;
	return bOk;
}

//机械臂从拍照等待区到光机夹取芯片
bool EpsonRobotFlowCtrl::HoldChipFromImager()
{
	bool bOk = false;
	bOk = Ctrl_ImagerGetChips_In();
	if (!bOk)
		return bOk;
	bOk = CloseGripper();
	if (!bOk)
		return bOk;
	return bOk;
}

//把芯片从光机送到回收槽
bool EpsonRobotFlowCtrl::GoRecycleFromImager()
{
	bool bOk = false;
	bOk = Ctrl_ImagerGetChips_Out();
	if (!bOk)
		return bOk;
	//从光机拍照等待区到光机沥液区,停 1.5 S
	bOk = Ctrl_ImagerWait2ImagerDry();
	if (!bOk)
		return bOk;
	ApplicationSleep(1500);
	bOk = Ctrl_ImagerDry2ImagerWait();
	if (!bOk)
		return bOk;
	bOk = Ctrl_ImagerWait2TransferTankWait();
	if (!bOk)
		return bOk;
	bOk = Ctrl_TransferTankPutChips_In();
	if (!bOk)
		return bOk;
	bOk = OpenGripper();
	if (!bOk)
		return bOk;
	bOk = Ctrl_TransferTankPutChips_Out();
	if (!bOk)
		return bOk;
	return bOk;
}

//急停
bool EpsonRobotFlowCtrl::EmergencyStop()
{
	bool bOk = false;
	bOk = m_EpsonRobotDriver->Abort();
	if(!bOk)
		return ErrorCodeTranslate::Instance()->FalseWithTip(__func__);
	return true;
}

//关夹手
bool EpsonRobotFlowCtrl::CloseGripper()
{
	bool bOk = false;
	bOk = m_EpsonRobotDriver->CloseGripper();
	if (!bOk)
		return ErrorCodeTranslate::Instance()->FalseWithTip(__func__);
	bOk = m_EpsonRobotDriver->IsGripChipSuccess();
	if (!bOk)
		return ErrorCodeTranslate::Instance()->FalseWithTip(__func__);
	return true;
}

//开夹手
bool EpsonRobotFlowCtrl::OpenGripper()
{
	bool bOk = false;
	bOk = m_EpsonRobotDriver->OpenGripper();
	if (!bOk)
		return ErrorCodeTranslate::Instance()->FalseWithTip(__func__);
	return true;
}

//吹风
bool EpsonRobotFlowCtrl::BlowChip()
{
	bool bOk = false;
	bOk = m_EpsonRobotDriver->BlowChip(true);
	if (!bOk)
		return ErrorCodeTranslate::Instance()->FalseWithTip(__func__);
	bOk = Ctrl_WindBladeWait2WindBlade();
	if (!bOk)
		return bOk;
	bOk = Ctrl_WindBlade2WindBladeWait();
	if (!bOk)
		return bOk;
	bOk = m_EpsonRobotDriver->BlowChip(false);
	if (!bOk)
		return ErrorCodeTranslate::Instance()->FalseWithTip(__func__);
	return true;
}

//信号灯
bool EpsonRobotFlowCtrl::OperateWarningLights(RobotParaType::RobotLightColor ColorType)
{
	bool bOk = false;
	bOk = m_EpsonRobotDriver->OperateWaringLights(ColorType);
	if (!bOk)
		return false;
	return true;
}

bool EpsonRobotFlowCtrl::HaveChipInTransferTank(int iSlotID)
{
	return m_EpsonRobotDriver->HaveChipInTransferTank(iSlotID);
}

bool EpsonRobotFlowCtrl::IsMotorReadyMove()
{
	return m_EpsonRobotDriver->IsMotorReadyMove();
}