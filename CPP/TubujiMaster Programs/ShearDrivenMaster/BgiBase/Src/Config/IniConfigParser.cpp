#include "IniConfigParser.h"
#include "BgiLogger.h"

#include <QFile>
#include <QString> 
#include <QCoreApplication>
#include <QDir>
#include <qsettings.h>

bool IniConfigParser::Parser(const QString& conf_filepath, QMap<QString, QVariant >& config_map)
{
	bool b_ret = false;
	QFile* file = new QFile(conf_filepath);
	if (file->open(QIODevice::ReadOnly | QIODevice::Text))
	{
		QSettings* p_setting = new QSettings(conf_filepath, QSettings::IniFormat);
		QStringList child_key_list = p_setting->childKeys();

		/*for each (QString var in child_key_list)
		{
			config_map.insert(var, p_setting->value(var));
		}*/
		foreach (QString var, child_key_list)
		{
			config_map.insert(var, p_setting->value(var));
		}

		delete p_setting;
		p_setting = NULL;
	}
	delete file;
	file = NULL;

	return b_ret;
}

/************************************* start-XCW *****************************************
 * 作    者：XCW
 * 功能描述：读取ini文件的一组内容
 * 时    间：2022/11/9 15:55
 * 入    参：strPath：文件名；
 * 入    参：strArrayName：组名
 * 入    参：mapObj：键名；
 * 出    参：mapObj：读取后的值；
 * 返    回：0：读取全部内容成功，-1：文件不存在，-2：未能全部读取成功，有些键值不存在，其他值：读取异常本次读取不可用
 ************************************* end-XCW *****************************************/
int IniConfigParser::ReadContents(const QString& strPath, const QString& strArrayName, QMap<QString, QVariant>& mapObj)
{
	int iReadSuc = 0;

	QFileInfo fileInfo(strPath);
	if (!fileInfo.exists())
	{
		iReadSuc = -1;
		return iReadSuc;
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
			iReadSuc = -2;
			continue;
		}
		// 读取键值对
		QVariant qvar = p_setting->value(strKeyName);
		begin_it.value() = qvar;
	}

	delete p_setting;
	p_setting = NULL;

	return iReadSuc;
}

/************************************* start-XCW *****************************************
 * 作    者：XCW
 * 功能描述：写入ini文件内容
 * 时    间：2022/11/9 15:55
 * 入    参：strPath：文件路径
 * 入    参：strArrayName：组名名称
 * 入    参：mapObj：节点内容
 * 出    参：无
 * 返    回：成功-true，失败-false
 ************************************* end-XCW *****************************************/
bool IniConfigParser::WriteContents(const QString& strPath, const QString& strArrayName, const QMap<QString, QVariant>& mapObj)
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
