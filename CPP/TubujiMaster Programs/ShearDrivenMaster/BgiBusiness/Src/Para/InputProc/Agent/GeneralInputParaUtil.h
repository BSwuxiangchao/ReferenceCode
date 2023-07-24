/*
* @Abrief 通用输入参数处理类
* @note 设置、修改、查询参数的帮助函数
* @version 1.0
* @author wuxiangchao 2023-02-14
*/
#ifndef _GENERAL_INPUT_PARA_UTIL_H_
#define _GENERAL_INPUT_PARA_UTIL_H_

#include "ParaType.h"
#include <vector>
#include "BgiProcId.h"
#include "BusinessExternDef.h"
#include "qvector.h"

class BGIBUSINE_CLASS_EXPORT GeneralInputParaUtil
{
public:
    static void SetSequencingTableDesc(QVector<GeneralParaType::SequencingTableDesc> value);
    static void GetSequencingTableDesc(QVector < GeneralParaType::SequencingTableDesc>& value);
};


#endif
