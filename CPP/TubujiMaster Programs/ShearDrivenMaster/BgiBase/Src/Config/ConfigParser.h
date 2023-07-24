/*
* @Abrief 配置文件类
* @note 配置文件的基类
* @version 1.0
* @author mosongjin 2021-07-21
*/

#ifndef _BGI_CONFIG_PARSER_H
#define _BGI_CONFIG_PARSERF_H

#include <QString>
#include <QVariant> 

class  ConfigParser
{
public:
	virtual ~ConfigParser() {}
	virtual bool Parser(const QString& conf_filepath, QMap<QString, QVariant >& config_map) = 0;
};


#endif