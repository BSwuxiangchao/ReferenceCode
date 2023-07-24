/*
* @Abrief 参数控件
* @note 用于GUI与参数的交互，参数值发生变化，就会通知到此参数控件
* @version 1.0
* @author wuxiangchao 2023-2-27
*/
#pragma once

#include "ParaControl.h"
#include <qvariant.h>


class ImagerOutputParaControl : public ParaControl
{
public:
    ImagerOutputParaControl(QString systemId, QString ParaId);
    virtual ~ImagerOutputParaControl();

private:
    virtual int GetParaId();
    virtual BgiProc::ESystemId GetSystemId();
    virtual BgiProc::ESystemType GetSystemType();
    virtual BgiProc::EProcType GetProcType();
    virtual ParaBase* GetParaBase();

protected:
    ParaBase* m_pParaBase;
    QString m_SystemId;
    QString m_strParaId;
    int m_iParaId;
};
