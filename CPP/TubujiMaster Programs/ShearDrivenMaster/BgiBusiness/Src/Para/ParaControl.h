/*
* @Abrief 参数控件
* @note 用于GUI与参数的交互，参数值发生变化，就会通知到此参数控件
* @note 由于内部把多个参数组合在一起作为一个参数，因此在与界面传递时必须对外公开此数据结构的内容 
* @version 1.0
* @author mosongjin 2021-08-17
* @remarks 此控件参数类的主要用意是想不让外界知道内部情况，现在来看需要对参数设计做修改才能更好的隐藏内部情况(使用QVariant类型作为参数的基本类型)
*/

#ifndef _PARA_CONTROL_H_
#define _PARA_CONTROL_H_

#include "BusinessExternDef.h"
#include "ParaObserver.h"
#include "ParaBase.h"
#include <vector>


class BGIBUSINE_CLASS_EXPORT ParaControl : public ParaObserver
{
public:
    ParaControl();
    virtual ~ParaControl();

    void Update();

    void SetValue(const void* value);
    const void* GetValue();

    void AddObserver(ParaObserver* observer);
    void DelObserver(ParaObserver* observer);

protected:
    virtual int GetParaId() = 0;
    virtual BgiProc::ESystemId GetSystemId() = 0;
    virtual BgiProc::ESystemType GetSystemType() = 0;
    virtual BgiProc::EProcType GetProcType() = 0;
    virtual ParaBase* GetParaBase() = 0;
    


private:
    std::vector<ParaObserver*> m_ControlObserver;
};


#endif
