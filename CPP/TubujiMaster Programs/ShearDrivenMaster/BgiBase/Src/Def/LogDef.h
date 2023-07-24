/*
* @Abrief 日志定义
* @note 日志的输出类型宏定义
* @version 1.0
* @author mosongjin 2021-07-20
*/

#ifndef _BGI_LOG_DEF_H
#define _BGI_LOG_DEF_H


enum ELogType
{
	LOG_TYPE_diagnosis = 0,
	LOG_TYPE_statistics,
	LOG_TYPE_audit
};

enum ELogLevel
{
	LOG_LEVEL_DEBUG = 0,
	LOG_LEVEL_INFO,
	LOG_LEVEL_WARN,
	LOG_LEVEL_ERROR,
	LOG_LEVEL_FATAL,
	LOG_LEVEL_ASSERT
};



#endif