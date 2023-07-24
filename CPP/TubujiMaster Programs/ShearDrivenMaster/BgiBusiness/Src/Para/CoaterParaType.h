/*
* @Abrief 自定义参数
* @note 自定义参数，某些同类型参数可以作为一个参数处理
* @version 1.0
* @author wuxiangchao 2023-02-24
*/


#ifndef _SORTING_PARA_TYPE_H_
#define _SORTING_PARA_TYPE_H_

#include "BusinessExternDef.h"
#include <QMetaType>
#include <QList>
#include "GeneralTool.h"
#include "BgiProcId.h"


class BGIBUSINE_CLASS_EXPORT CoaterParaType
{
public:
    struct CoaterPara
    {
        int iConnect;

        bool operator ==(const CoaterPara& obj) const
        {
            if (obj.iConnect == this->iConnect)
                return true;
            return false;
        }
    };

    //涂布机PLC状态数据
    struct PLCStatusData
    {
        int Addr;
        int Len;
        int Value;

        PLCStatusData()
        {
            Addr = 0;
            Len = 0;
            Value = 0;
        }

        PLCStatusData(int addr, int len, int value = 0)
        {
            this->Addr = addr;
            this->Len = len;
            this->Value = value;
        }
    };
};

//Q_DECLARE_METATYPE(CoaterParaType::DeviceDistance);


#endif
