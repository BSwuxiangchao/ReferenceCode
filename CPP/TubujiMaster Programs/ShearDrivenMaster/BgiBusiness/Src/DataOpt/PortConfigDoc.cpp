#include "PortConfigDoc.h"
#include <qfile.h>
#include <qjsondocument.h>
#include <qjsonobject.h>
#include <qdebug.h>
#include <qsettings.h>
#include <qfileinfo.h>
#include "BgiLogger.h"
#include <qcoreapplication.h>
#include "BgiFileTools.h"


PortConfigDoc* PortConfigDoc::Instance()
{
	static PortConfigDoc instance;
	return &instance;
}

PortConfigDoc::PortConfigDoc()
	:m_bReadSuc(false)
{
	QString path;
	BgiFileTools::GetDataDirPath(path);

	path += "//Data//Input//PortConfig.ini";
	ReadConfigIni(path);
}


PortConfigDoc::~PortConfigDoc()
{
}


bool PortConfigDoc::GetPortValue(QString port_name, QString& port_value)
{
	QMap<QString, QString>::iterator iter = m_PortMap.find(port_name);
	if (iter != m_PortMap.end())
	{
		port_value = iter.value();
		return true;
	}
	else
	{
		BGI_WARNING(QString("未找到配置项 port_name %1").arg(port_name).toStdString().c_str());
		return false;
	}
}

bool PortConfigDoc::ReadConfigIni()
{
	QString path;
	BgiFileTools::GetDataDirPath(path);

	path += "//Data//Input//PortConfig.ini";
	return ReadConfigIni(path);
}

bool PortConfigDoc::ReadConfigIni(QString path)
{
	m_bReadSuc = false;
	QSettings* p_setting = new QSettings(path, QSettings::IniFormat);

	QFileInfo fileInfo(path);
	if (!fileInfo.exists())
	{
		BGI_WARNING("could't open port config ini");
		return m_bReadSuc;
	}

	m_PortMap.clear();
	QVariant qvar;
	qvar = p_setting->value(QString("/%1/%2").arg(QString("RobotCfg")).arg(QString("RobotIP")));
	m_PortMap.insert("RobotIP", qvar.toString());
	qvar = p_setting->value(QString("/%1/%2").arg(QString("RobotCfg")).arg(QString("RobotPort")));
	m_PortMap.insert("RobotPort", qvar.toString());

	qvar = p_setting->value(QString("/%1/%2").arg(QString("CoaterCfg")).arg(QString("CoaterIP")));
	m_PortMap.insert("CoaterIP", qvar.toString());
	qvar = p_setting->value(QString("/%1/%2").arg(QString("CoaterCfg")).arg(QString("CoaterPort")));
	m_PortMap.insert("CoaterPort", qvar.toString());

	qvar = p_setting->value(QString("/%1/%2").arg(QString("ImagerCfg")).arg(QString("ImagerIP")));
	m_PortMap.insert("ImagerIP", qvar.toString());
	qvar = p_setting->value(QString("/%1/%2").arg(QString("ImagerCfg")).arg(QString("ImagerPort")));
	m_PortMap.insert("ImagerPort", qvar.toString());

	qvar = p_setting->value(QString("/%1/%2").arg(QString("ScannerCfg")).arg(QString("ScannerIP")));
	m_PortMap.insert("ScannerIP", qvar.toString());
	qvar = p_setting->value(QString("/%1/%2").arg(QString("ScannerCfg")).arg(QString("ScannerPort")));
	m_PortMap.insert("ScannerPort", qvar.toString());

	foreach(QString key, p_setting->allKeys())
	{
		if (!m_PortMap.contains(key))
		{
			QString value = p_setting->value(key).toString();
			m_PortMap.insert(key, value);
		}
	}
	delete p_setting;
	p_setting = NULL;

	m_bReadSuc = true;
	return m_bReadSuc;

}



