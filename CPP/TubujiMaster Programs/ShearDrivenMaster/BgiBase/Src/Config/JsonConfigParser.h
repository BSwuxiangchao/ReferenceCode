/*
* @Abrief Json配置文件
* @note 解释Json文件
* @version 1.0
* @author mosongjin 2021-07-21
*/

#ifndef _BGI_JSON_CONFIG_PARSER_H
#define _BGI_JSON_CONFIG_PARSERF_H

#include "ConfigParser.h"
#include "ExternDef.h"


class BGIBASE_CLASS_EXPORT JsonConfigParser : public ConfigParser
{
public:
	virtual bool Parser(const QString& conf_filepath, QMap<QString, QVariant >& config_map);
private:
	void ParserJosn(QJsonObject json_object, QMap<QString, QVariant >& config_map);
};


#endif