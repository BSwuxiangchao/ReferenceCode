#include "CoaterStepDescription.h"
#include "BgiFileTools.h"
#include <qsettings.h>
#include <qfileinfo.h>
#include "BgiLogger.h"

CoaterStepDescription* CoaterStepDescription::Instance()
{
	static CoaterStepDescription instance;
	return &instance;
}

CoaterStepDescription::CoaterStepDescription()
	:m_bReadSuc(false)
{
	ReadConfigIni();
}

CoaterStepDescription::~CoaterStepDescription()
{}

bool CoaterStepDescription::ReadConfigIni()
{
	QString path;
	BgiFileTools::GetDataDirPath(path);

	path += "//Data//Input//CoaterStepDescription.ini";
	return ReadConfigIni(path);
}

bool CoaterStepDescription::ReadConfigIni(QString path)
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

bool CoaterStepDescription::GetCfgValue(QString key, QString& Value)
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
