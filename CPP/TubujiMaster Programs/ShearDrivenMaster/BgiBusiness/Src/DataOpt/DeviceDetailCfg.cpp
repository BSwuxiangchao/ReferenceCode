#include "DeviceDetailCfg.h"
#include <qfile.h>
#include <qjsondocument.h>
#include <qjsonobject.h>
#include <qdebug.h>
#include <qsettings.h>
#include <qfileinfo.h>
#include "BgiLogger.h"
#include <qcoreapplication.h>
#include "BgiFileTools.h"


DeviceDetailCfg* DeviceDetailCfg::Instance()
{
	static DeviceDetailCfg instance;
	return &instance;
}

DeviceDetailCfg::DeviceDetailCfg()
	:m_bReadSuc(false)
{
	QString path;
	BgiFileTools::GetDataDirPath(path);

	m_QListPointVar.clear();
	path += "//Data//Input//DeviceDetailCfg.ini";
	ReadConfigIni(path);
}


DeviceDetailCfg::~DeviceDetailCfg()
{
}


bool DeviceDetailCfg::GetCfgValue(QString key, QString& Value)
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

bool DeviceDetailCfg::ReadConfigIni()
{
	QString path;
	BgiFileTools::GetDataDirPath(path);

	path += "//Data//Input//DeviceDetailCfg.ini";
	return ReadConfigIni(path);
}

bool DeviceDetailCfg::ReadConfigIni(QString path)
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
	QVariant qvar;
	
	foreach(QString key, p_setting->allKeys())
	{
		if (!m_DataMap.contains(key))
		{
			QString value = p_setting->value(key).toString();
			m_DataMap.insert(key, value);
		}
	}

	p_setting->beginGroup("RobotPosVar");
	m_QListPointVar.clear();
	foreach(QString key, p_setting->allKeys())
	{
		QString value = p_setting->value(key).toString();
		m_QListPointVar.append(value);
	}
	p_setting->endGroup();

	p_setting->beginGroup("RobotOutIO");
	m_QListOutOptIO.clear();
	foreach(QString key, p_setting->allKeys())
	{
		m_QListOutOptIO.append(key);
	}
	p_setting->endGroup();

	delete p_setting;
	p_setting = NULL;
	m_bReadSuc = true;
	return m_bReadSuc;

}

QStringList DeviceDetailCfg::GetListPointVar()
{
	return m_QListPointVar;
}

QStringList DeviceDetailCfg::GetListOutOptIO()
{
	return m_QListOutOptIO;
}
