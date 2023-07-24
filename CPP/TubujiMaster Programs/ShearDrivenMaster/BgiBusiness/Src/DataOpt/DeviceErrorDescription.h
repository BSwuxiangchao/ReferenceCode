#pragma once

#include <qstring.h>
#include <qmap.h>
#include "BusinessExternDef.h"

class BGIBUSINE_CLASS_EXPORT DeviceErrorDescription
{
public:
	static DeviceErrorDescription* Instance();
	~DeviceErrorDescription();

	bool GetCfgValue(QString key, QString& Value);
	bool ReadConfigIni();

private:
	DeviceErrorDescription();
	bool ReadConfigIni(QString path);

private:
	QMap<QString, QString> m_DataMap;
	bool m_bReadSuc;
};
