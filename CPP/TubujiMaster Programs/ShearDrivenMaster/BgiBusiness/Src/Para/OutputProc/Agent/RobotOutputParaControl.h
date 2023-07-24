/*
* @Abrief 参数控件
* @note 用于GUI与参数的交互，参数值发生变化，就会通知到此参数控件
* @version 1.0
* @author wuxiangchao 2023-02-27
*/

#ifndef _ROBOT_OUTPUT_PARA_CONTROL_H_
#define _ROBOT_OUTPUT_PARA_CONTROL_H_

#include "ParaControl.h"
#include <qvariant.h>


class RobotOutputParaControl : public ParaControl
{
public:
    RobotOutputParaControl(QString systemId, QString ParaId);
    virtual ~RobotOutputParaControl();

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


#endif
