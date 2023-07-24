#include "DeviceErrorDescription.h"
#include "BgiFileTools.h"
#include <qsettings.h>
#include <qfileinfo.h>
#include "BgiLogger.h"

DeviceErrorDescription* DeviceErrorDescription::Instance()
{
	static DeviceErrorDescription instance;
	return &instance;
}

DeviceErrorDescription::DeviceErrorDescription()
	:m_bReadSuc(false)
{
	ReadConfigIni();
}

DeviceErrorDescription::~DeviceErrorDescription()
{}

bool DeviceErrorDescription::ReadConfigIni()
{
	QString path;
	BgiFileTools::GetDataDirPath(path);

	path += "//Data//Input//DeviceErrorDescription.ini";
	return ReadConfigIni(path);
}

bool DeviceErrorDescription::ReadConfigIni(QString path)
{
	m_bReadSuc = false;
	QSettings* p_setting = new QSettings(path, QSettings::IniFormat);

	QFileInfo fileInfo(path);
	if (!fileInfo.exists())
	{
		BGI_WARNING("could't open device config ini");
		return m_bReadSuc;
	}

	m_DataMap.clear();
	foreach(QString key, p_setting->allKeys())
	{
		if (!m_DataMap.contains(key))
		{
			QString value = p_setting->value(key).toString();
			m_DataMap.insert(key, value);
		}
	}

	delete p_setting;
	p_setting = NULL;
	m_bReadSuc = true;
	return m_bReadSuc;
}

bool DeviceErrorDescription::GetCfgValue(QString key, QString& Value)
{
	QMap<QString, QString>::iterator iter = m_DataMap.find(key);
	if (iter != m_DataMap.end())
	{
		Value = iter.value();
		return true;
	}
	else
	{
		BGI_WARNING(QString("未找到配置项 Key %1").arg(key).toStdString().c_str());
		return false;
	}
}
