#include "DeviceEmgMode.h"
#include "LTDMC.h"
#include "OperateRecordInfo.h"
#include "CommonColors.h"


DeviceEmgMode::DeviceEmgMode()
	: m_iCardNo(0)
	, m_iAxis(0)
	, m_iMapIoIndex(15)
	, m_iEmgLogic(1)
{
	// 急停信号
	m_iIoType = 3;
	//通用输入端
	m_iMapIoType = 6;
	// 滤波时间间隔:0.001s
	m_fFilter = 0.01f;

}

// 设置急停
int DeviceEmgMode::SetEmg()
{
	short iResult = -1;
	// 设置轴IO映射
	iResult = dmc_set_axis_io_map(m_iCardNo, m_iAxis, m_iIoType, m_iMapIoType, m_iMapIoIndex, m_fFilter);
	if (0 != iResult)
	{
		// 设置轴IO映射失败
		RecordData data;
		data.m_strModelName = "设置急停";
		data.m_strInfoType = "设置轴IO映射";
		data.m_strContent = QString("设置失败，雷赛卡控制卡返回值为：%1").arg(iResult);
		data.m_color = gc_colorRed;
		OperateRecordInfo::AddRecordData(data);
		return 1;
	}

	// 设置急停信号使能：1-使能，0-使能， m_iEmgLogic=0：低电平有效，m_iEmgLogic=1：高电平有效；
	iResult = dmc_set_emg_mode(m_iCardNo, m_iAxis, 1, m_iEmgLogic);
	if (0 != iResult)
	{
		// 设置轴IO映射失败
		RecordData data;
		data.m_strModelName = "设置急停";
		data.m_strInfoType = "设置急停信号使能";
		data.m_strContent = QString("设置失败，雷赛卡控制卡返回值为：%1").arg(iResult);
		data.m_color = gc_colorRed;
		OperateRecordInfo::AddRecordData(data);
		return 1;
	}

	// 设置成功读取其设置的状态
	unsigned short enable = 0;
	unsigned short logic = 0;
	if (false == GetEmg(enable, logic))
	{
		// 读取失败
		return 1;
	}
	else
	{
		// 读取成功
		// 更新到界面
		return 2;
	}
}
// 获取设置急停状态
bool DeviceEmgMode::GetEmg(unsigned short& enable, unsigned short& logic)
{
	short iResult = -1;

	unsigned short iEnable = 0;
	unsigned short iLogic = 0;
	iResult = dmc_get_emg_mode(m_iCardNo, m_iAxis, &iEnable, &iLogic);

	if (0 != iResult)
	{
		// 设置轴IO映射失败
		RecordData data;
		data.m_strModelName = "急停状态";
		data.m_strInfoType = "获取急停状态";
		data.m_strContent = QString("获取失败，雷赛卡控制卡返回值为：%1").arg(iResult);
		data.m_color = gc_colorRed;
		OperateRecordInfo::AddRecordData(data);
		return false;
	}
	// 获取值
	enable = iEnable;
	logic = iLogic;
	return true;
}
// 清除急停设置；
int DeviceEmgMode::ClearEmg()
{
	// 设置急停失能
	unsigned short iResult = dmc_set_emg_mode(m_iCardNo, m_iAxis, 0, m_iEmgLogic);
	if (0 != iResult)
	{
		// 设置轴IO映射失败
		RecordData data;
		data.m_strModelName = "清除急停";
		data.m_strInfoType = "急停信号失能";
		data.m_strContent = QString("清除失败，雷赛卡控制卡返回值为：%1").arg(iResult);
		data.m_color = gc_colorRed;
		OperateRecordInfo::AddRecordData(data);
		return 3;
	}

	// 清除成功读取其设置的状态
	unsigned short enable = 0;
	unsigned short logic = 0;
	if (false == GetEmg(enable, logic))
	{
		// 读取失败
		return 3;
	}
	else
	{
		// 读取成功
		// 更新到界面
		return 4;
	}
}

// 设置参数
void DeviceEmgMode::SetEmgParam(unsigned short iCarNo, unsigned short iMapIoIndex, unsigned short iAxis, unsigned short logic)
{
	m_iCardNo = iCarNo;
	m_iMapIoIndex = iMapIoIndex;
	m_iAxis = iAxis;
	m_iEmgLogic = logic;
}

//运动卡的编号
void DeviceEmgMode::SetCardNo(const unsigned short& cardNo)
{
	m_iCardNo = cardNo;
}
unsigned short DeviceEmgMode::GetCardNo() const
{
	return m_iCardNo;
}

// 轴号
void DeviceEmgMode::SetAxis(const unsigned short& axis)
{
	m_iAxis = axis;
}
unsigned short DeviceEmgMode::GetAxis() const
{
	return m_iAxis;
}

void DeviceEmgMode::SetMapIoIndex(const unsigned short& io)
{
	m_iMapIoIndex = io;
}
unsigned short DeviceEmgMode::GetMapIoIndex() const
{
	return m_iMapIoIndex;
}

// 急停电平
void DeviceEmgMode::SetEmgLogic(const unsigned short& logic)
{
	m_iEmgLogic = logic;
}
unsigned short DeviceEmgMode::GetEmgLogic() const
{
	return m_iEmgLogic;
}

