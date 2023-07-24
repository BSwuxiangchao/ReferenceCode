/*
* @Abrief Ini配置文件
* @note 解释Ini文件
* @version 1.0
* @author mosongjin 2021-07-21
*/

#ifndef _BGI_INI_CONFIG_PARSER_H
#define _BGI_INI_CONFIG_PARSERF_H

#include "ConfigParser.h"
#include "ExternDef.h"


class BGIBASE_CLASS_EXPORT IniConfigParser : public ConfigParser
{
public:
	virtual bool Parser(const QString& conf_filepath, QMap<QString, QVariant >& config_map);

/************************************* start-XCW *****************************************
 * 作    者：XCW
 * 功能描述：读取ini文件的一组内容
 * 时    间：2022/11/9 15:55
 * 入    参：strPath：文件名；
 * 入    参：strArrayName：组名
 * 入    参：mapObj：键名；
 * 出    参：mapObj：读取后的值；
 * 返    回：0：读取全部内容成功，-1：文件不存在，-2：未能全部读取成功，有些键值不存在，其他值：读取异常本次读取不可用
 ************************************* end-XCW *****************************************/
	int ReadContents(const QString& strPath, const QString& strArrayName, QMap<QString, QVariant>& mapObj);

/************************************* start-XCW *****************************************
 * 作    者：XCW
 * 功能描述：写入ini文件内容
 * 时    间：2022/11/9 15:55
 * 入    参：strPath：文件路径
 * 入    参：strArrayName：组名名称
 * 入    参：mapObj：节点内容
 * 出    参：无
 * 返    回：成功-true，失败-false
 ************************************* end-XCW *****************************************/
	bool WriteContents(const QString& strPath, const QString& strArrayName, const QMap<QString, QVariant>& mapObj);
};


#endif