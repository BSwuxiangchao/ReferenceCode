/*
* @Abrief 光机输入参数处理类
* @note 设置、修改、查询参数的帮助函数
* @version 1.0
* @author wuxiangchao 2023-02-24
*/
#ifndef _COATER_INPUT_PARA_UTIL_H_
#define _COATER_INPUT_PARA_UTIL_H_

#include "ParaType.h"
#include <vector>
#include "BgiProcId.h"
#include "CoaterParaType.h"
#include "BusinessExternDef.h"

class BGIBUSINE_CLASS_EXPORT CoaterInputParaUtil
{
public:
    static void SetSortingStatus(GeneralParaType::WorkStatus value);
    static void GetSortingStatus(GeneralParaType::WorkStatus& status);
};


#endif
