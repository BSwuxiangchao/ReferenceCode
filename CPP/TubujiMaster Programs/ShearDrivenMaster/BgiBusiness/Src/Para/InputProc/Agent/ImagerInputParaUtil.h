/*
* @Abrief 光机输入参数处理类
* @note 设置、修改、查询参数的帮助函数
* @version 1.0
* @author wuxiangchao 2023-02-24
*/
#ifndef _IMAGER_INPUT_PARA_UTIL_H_
#define _IMAGER_INPUT_PARA_UTIL_H_

#include "ParaType.h"
#include <vector>
#include "BgiProcId.h"

class ImagerInputParaUtil
{
public:
    static void SetTransportStatus(GeneralParaType::WorkStatus value);


    static void GetTransportStatus(GeneralParaType::WorkStatus& status);
};


#endif
