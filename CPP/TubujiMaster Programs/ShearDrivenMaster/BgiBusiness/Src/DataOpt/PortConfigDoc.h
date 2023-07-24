/*
* @Abrief 端口配置文档
* @note 端口配置文件，系统1、2牵涉到的串口、网络地址的配置
* @version 1.0
* @author mosongjin 2022-06-02
*/

#ifndef _PORT_CONFIG_DOC_H
#define _PORT_CONFIG_DOC_H

#include <qstring.h>
#include <qmap.h>
#include "BusinessExternDef.h"


class BGIBUSINE_CLASS_EXPORT PortConfigDoc
{
public:	
	static PortConfigDoc* Instance();
	~PortConfigDoc();

	bool GetPortValue(QString port_name, QString& port_value);
	bool ReadConfigIni();
	//bool SaveConfigIni(QMap<QString, QString> PortMap);
private:
	PortConfigDoc();
	bool ReadConfigIni(QString path);


private:
	QMap<QString, QString> m_PortMap;
	bool m_bReadSuc;
};




#endif



