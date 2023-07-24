/************************************* start-XCW *****************************************
 * 作    者：XCW
 * 创建时间：2022/6/14 14:37
 * 功能描述：操作读写界面参数配置文件；
 ************************************* end-XCW *****************************************/
#pragma once
#include <QString>
#include <QMap>
#include "ParameterConfig.h"

// 配置文件名称
const QString gc_strParameterConfigName = "InterfaceParameterConfig.ini";


class BGIBUSINE_CLASS_EXPORT OperateParameterConfig
{
public:

	OperateParameterConfig();
	~OperateParameterConfig();

	// 初始化
	void Init();

	// 路径
	void SetFileName(const QString& strFileName);

	// 读取ini文件
	bool ReadIni(const QString& strPath, const QString& strArrayName, QMap<QString, QVariant>& mapObj);
/************************************* start-XCW *****************************************
 * 作    者：XCW
 * 功能描述：读取ini文件的一组内容，如果传入的组名或者键名不存在，则读取失败；
 * 时    间：2022/6/27 16:18
 * 入    参：strPath：文件名；
 * 入    参：strArrayName：组名
 * 入    参：mapObj：键名；
 * 出    参：mapObj：读取后的值；
 * 返    回：读取成功-true，读取失败-false；
 ************************************* end-XCW *****************************************/
	bool ReadIniContents(const QString& strPath, const QString& strArrayName, QMap<QString, QVariant>& mapObj);

	// 写入ini文件
	bool WriteIni(const QString& strPath, const QString& strArrayName, const QMap<QString, QVariant>& mapObj);

	
protected:

	// 路径
	QString m_strFileName;


};

