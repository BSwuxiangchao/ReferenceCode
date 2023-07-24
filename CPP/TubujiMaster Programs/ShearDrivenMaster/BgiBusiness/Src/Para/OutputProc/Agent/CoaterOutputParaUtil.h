/*
* @Abrief 输出参数处理类
* @note 设置、修改、查询参数的帮助函数
* @version 1.0
* @author wuxiangchao 2023-02-27
*/
#ifndef _COATER_OUTPUT_PARA_UTIL_H_
#define _COATER_OUTPUT_PARA_UTIL_H_

#include "ParaType.h"
#include <vector>
#include "BgiProcId.h"
#include "CoaterParaType.h"
#include <QMutex>

enum BGIBUSINE_CLASS_EXPORT FluidType
{
    V100,
    V101,
    V102,
    V103,
    ACN,
    Deblock,
    ETT,
    CapA,
    CapB,
    DCM,
    OX,
    A,
    T,
    X,
    Y,
    C,
    G,
};
class BGIBUSINE_CLASS_EXPORT SynthesisOutputParaUtil
{
public:
private:
    static QMutex m_mMutex;
    //static QVector<SynthesisParaType::FluidValveDesc> m_vecMainFluidStateSysA;
};


#endif
