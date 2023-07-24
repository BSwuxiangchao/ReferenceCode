#include "OperateParameterConfig.h"
#include <QSettings>
#include <QFileInfo>
#include <QCoreApplication>
#include "BgiFileTools.h"
#include "IniConfigParser.h"

// 引片配置的组名称
const QString gc_strConfigChip = "ConfigChip";
// 最小id
const QString gc_strChipMinId = "ChipMinId";
// 最大id
const QString gc_strChipMaxId = "ChipMaxId";

/* 分拣超时参数的组名称 */
const QString gc_strConfigSortingTimeout = "ConfigSortingTimeout";
//是否执行分拣超时
const QString gc_strSortingTimeoutSwitch = "Switch";
//剩余芯片数量
const QString gc_strSortingTimeouRemainderNum = "RemainderNum";
//无有效芯片的时间,单位s
const QString gc_strSortingTimeouSpareTime = "SpareTime";

/* 回流触发条件参数 */
const QString gc_strConfigBackFlow = "ConfigBackFlow";
// 首端触发片数
const QString gc_strConfigBackFlowStartNum = "StartNum";
// 首端触发回流时间（S）
const QString gc_strConfigBackFlowStartTime = "StartTime";
// 末端触发片数
const QString gc_strConfigBackFlowEndNum = "EndNum";
// 末端触发回流时间（S）
const QString gc_strConfigBackFlowEndTime = "EndTime";

/* 试剂消耗表格相关路径参数 */
const QString gc_strConfigReagentConsume = "ConfigReagentConsume";
// 试剂剩余量
const QString gc_strConfigReagentRemain = "ReagentRemain";
// 芯片数量对应试剂容积
const QString gc_strConfigReagentVolume = "ReagentVolume";

// 转盘重量(kg)
const QString gc_strSortigDeviceParaTurnWeight = "TurnWeight";
// 转盘半径(mm)
const QString gc_strSortigDeviceParaTurnRadius = "TurnRadius";
// 转盘分辨率(ppr)
const QString gc_strSortigDeviceParaTurnPpr = "TurnPpr";
// 转盘转速(r/min)
const QString gc_strSortigDeviceParaTurnSpeed = "TurnSpeed";
// 芯片间距(mm)
const QString gc_strSortigDeviceParaChipDistance = "ChipDistance";
// 相机X轴(mm)
const QString gc_strSortigDeviceParaCameraX = "CameraX";
// 相机Y轴(mm)
const QString gc_strSortigDeviceParaCameraY = "CameraY";
// 拍照范围(pps)
const QString gc_strSortigDeviceParaCameraDistance = "CameraDistance";
// 吹气阀时长(ms)
const QString gc_strSortigDeviceParaBlowDistance = "BlowDistance";
// 最小早开阀
const QString gc_strSortigDeviceParaBlowEarlyMin = "BlowEarlyMin";
// 最大早开阀
const QString gc_strSortigDeviceParaBlowEarlyMax = "BlowEarlyMax";

/* 仅分拣参数 */
const QString gc_strSortingOnly = "SortingOnly";
// 无需转运
const QString gc_strSortingOnlyNeedTransport = "NeedTransport";

// 分拣相机软件
// 组名
QString gc_strSortingCameraSoftwareArray = "SortingCameraSoftware";
// 软件路径
QString gc_strSortingCameraPath = "SoftwarePath";
// 类名
QString gc_strSortingCameraClassName = "ClassName";
// 窗口标题
QString gc_strSortingCameraWindowTitle = "WindowTitle";
// 相机文件
QString gc_strSortingCameraCxnFile = "CxnFile";

OperateParameterConfig::OperateParameterConfig()
{
	Init();

}

OperateParameterConfig::~OperateParameterConfig()
{

}

// 初始化
void OperateParameterConfig::Init()
{
	// 获取ini文件名
	BgiFileTools::GetDataDirPath(m_strFileName);
	m_strFileName += "//Data//Input//";
	m_strFileName += gc_strParameterConfigName;

	// 测试
	/*ParameterConfig::InterfaceParamCfgChip chip;
	chip.m_iIdMin = 100;
	chip.m_iIdMax = 99999;
	WriteParamChip(chip);

	ParameterConfig::InterfaceParamCfgChip chipRead;
	ReadParamChip(chipRead);*/

}

// 路径
void OperateParameterConfig::SetFileName(const QString& strFileName)
{
	m_strFileName = strFileName;
}

// 读取ini文件
bool OperateParameterConfig::ReadIni(const QString& strPath, const QString& strArrayName, QMap<QString, QVariant>& mapObj)
{
	bool bReadSuc = false;

	QFileInfo fileInfo(strPath);
	if (!fileInfo.exists())
	{
		qDebug() << "could't open projects config ini";
		return bReadSuc;
	}

	QSettings* p_setting = new QSettings(strPath, QSettings::IniFormat);

	QMap < QString, QVariant>::iterator begin_it = mapObj.begin();
	QMap < QString, QVariant>::iterator end_it = mapObj.end();
	for (; begin_it != end_it; ++begin_it)
	{
		QVariant qvar = p_setting->value(QString("/%1/%2").arg(QString(strArrayName)).arg(begin_it.key()));
		begin_it.value() = qvar;
	}

	delete p_setting;
	p_setting = NULL;

	return true;
}

bool OperateParameterConfig::ReadIniContents(const QString& strPath, const QString& strArrayName, QMap<QString, QVariant>& mapObj)
{
	bool bReadSuc = false;

	QFileInfo fileInfo(strPath);
	if (!fileInfo.exists())
	{
		qDebug() << "could't open projects config ini";
		return bReadSuc;
	}

	QSettings* p_setting = new QSettings(strPath, QSettings::IniFormat);

	QMap < QString, QVariant>::iterator begin_it = mapObj.begin();
	QMap < QString, QVariant>::iterator end_it = mapObj.end();
	QString strKeyName = "";
	for (; begin_it != end_it; ++begin_it)
	{
		strKeyName = QString("/%1/%2").arg(QString(strArrayName)).arg(begin_it.key());
		// 判断键是否存在
		if (!p_setting->contains(strKeyName))
		{
			return false;
		}
		// 读取键值对
		QVariant qvar = p_setting->value(strKeyName);
		begin_it.value() = qvar;
	}

	delete p_setting;
	p_setting = NULL;

	return true;
}

// 写入ini文件
bool OperateParameterConfig::WriteIni(const QString& strPath, const QString& strArrayName, const QMap<QString, QVariant>& mapObj)
{
	QSettings settings(strPath, QSettings::IniFormat); // 当前目录的INI文件
	// 设置组名
	settings.beginGroup(strArrayName);

	QMap < QString, QVariant>::const_iterator begin_it = mapObj.begin();
	QMap < QString, QVariant>::const_iterator end_it = mapObj.end();
	for (; begin_it != end_it; ++begin_it)
	{
		settings.setValue(begin_it.key(), begin_it.value());
	}
	settings.endGroup();

	return true;
}

//// 读取引片参数
//bool OperateParameterConfig::ReadParamChip(ParameterConfig::InterfaceParamCfgChip& obj)
//{
//	QMap<QString, QVariant> map;
//	map.insert(gc_strChipMinId, 0);
//	map.insert(gc_strChipMaxId, 0);
//	
//	if (!ReadIni(m_strFileName, gc_strConfigChip, map))
//	{
//		return false;
//	}
//
//	QMap < QString, QVariant>::iterator end_it = map.end();
//	QMap < QString, QVariant>::iterator find_it;
//
//	find_it = map.find(gc_strChipMinId);
//	if (find_it != end_it)
//	{
//		obj.m_iIdMin = find_it.value().toInt();
//	}
//
//	find_it = map.find(gc_strChipMaxId);
//	if (find_it != end_it)
//	{
//		obj.m_iIdMax = find_it.value().toInt();
//	}
//
//	return true;
//}
//
//// 写入引片参数
//bool OperateParameterConfig::WriteParamChip(const ParameterConfig::InterfaceParamCfgChip& obj)
//{
//	QMap<QString, QVariant> map;
//	map.insert(gc_strChipMinId, obj.m_iIdMin);
//	map.insert(gc_strChipMaxId, obj.m_iIdMax);
//
//	WriteIni(m_strFileName, gc_strConfigChip, map);
//	return true;
//}



