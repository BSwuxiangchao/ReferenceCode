#pragma once

#include <qstring.h>
#include <qmap.h>
#include "BusinessExternDef.h"

class BGIBUSINE_CLASS_EXPORT CoaterStepDescription
{
public:
	static CoaterStepDescription* Instance();
	~CoaterStepDescription();

	bool GetCfgValue(QString key, QString& Value);
	bool ReadConfigIni();

private:
	CoaterStepDescription();
	bool ReadConfigIni(QString path);

private:
	QMap<QString, QString> m_DataMap;
	bool m_bReadSuc;
};
