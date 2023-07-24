/*
* @Abrief 参数控件提供者
* @note 给应用层使用的参数控件提供者
* @version 1.0
* @author mosongjin 2021-09-16
*/

#ifndef _PARA_CONTROL_PROVIDER_H
#define _PARA_CONTROL_PROVIDER_H

#include "BusinessExternDef.h"
#include <qstring.h>
#include <map>

class ParaControl;
class ParaControlLibrary;

class BGIBUSINE_CLASS_EXPORT ParaControlProvider
{
public:
    static ParaControlProvider* Instance();

    ~ParaControlProvider();

    ParaControl* GetParameterControl(QString strSystemId, QString strSystemType, QString strProcType, QString strParaName);

    bool DisableParameterControl(int id, bool disable);

private:
    ParaControlProvider();
    struct ParaControlKey
    {
        QString SystemId;
        QString SystemType;
        QString ProcType;
        QString ParaName;

        bool operator< (const ParaControlKey& obj) const
        {
            if(SystemId != obj.SystemId)
                return SystemId < obj.SystemId;
            if(SystemType != obj.SystemType)
                return SystemType < obj.SystemType;
            if (ProcType != obj.ProcType)
                return ProcType < obj.ProcType;
            if (ParaName != obj.ParaName)
                return ParaName < obj.ParaName;
            return false;
        }
    };

    ParaControlLibrary* m_pParaComtrolLib;
    std::map<ParaControlKey, ParaControl *> m_ParaControlMap;
};

#endif // _PARA_CONTROL_PROVIDER_H
