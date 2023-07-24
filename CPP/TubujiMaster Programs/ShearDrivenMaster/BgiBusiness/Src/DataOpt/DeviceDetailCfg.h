/*
* @Abrief 设备具体配置数据
* @note 包含IO配置，初始化数据
* @version 1.0
* @author wuxiangchao 2023-04-11
*/

#ifndef _DEVICE_CONFIG_DOC_H
#define _DEVICE_CONFIG_DOC_H

#include <qstring.h>
#include <qmap.h>
#include "BusinessExternDef.h"


class BGIBUSINE_CLASS_EXPORT DeviceDetailCfg
{
public:	
	static DeviceDetailCfg* Instance();
	~DeviceDetailCfg();

	bool GetCfgValue(QString key, QString& Value);
	bool ReadConfigIni();
	//bool SaveConfigIni(QMap<QString, QString> PortMap);
	QStringList GetListPointVar();
	QStringList GetListOutOptIO();
private:
	DeviceDetailCfg();
	bool ReadConfigIni(QString path);


private:
	QMap<QString, QString> m_DataMap;
	bool m_bReadSuc;
	QStringList m_QListPointVar;
	QStringList m_QListOutOptIO;

};




#endif



