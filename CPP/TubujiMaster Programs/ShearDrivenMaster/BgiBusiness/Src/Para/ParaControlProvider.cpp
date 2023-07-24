#include "ParaControlProvider.h"
#include "BgiLogger.h"
#include "ParaControlLibrary.h"
#include "ParaControl.h"

ParaControlProvider* ParaControlProvider::Instance()
{
    static ParaControlProvider lib;
    return &lib;
}

ParaControlProvider::ParaControlProvider()
{
    m_pParaComtrolLib = new ParaControlLibrary();
}

ParaControlProvider::~ParaControlProvider()
{
    if (m_pParaComtrolLib != NULL)
    {
        delete m_pParaComtrolLib;
        m_pParaComtrolLib = NULL;
    }
    /*
    std::map<ParaControlKey, ParaControl *>::iterator iter = m_ParaControlMap.begin();
    std::map<ParaControlKey, ParaControl *>::iterator iter_end = m_ParaControlMap.end();
    for(; iter != iter_end; ++iter)
    {
        delete iter->second;
        iter->second = NULL;
    }
    */
}

ParaControl* ParaControlProvider::GetParameterControl(QString strSystemId, QString strSystemType, QString strProcType, QString strParaName)
{
    ParaControl* para_control = NULL;
    if(m_pParaComtrolLib->CheckParameterExist(strSystemId, strSystemType, strProcType, strParaName))
        BGI_ASSERT(false);

    ParaControlKey key;
    key.SystemId = strSystemId;
    key.SystemType = strSystemType;
    key.ProcType = strProcType;
    key.ParaName = strParaName;
    std::map<ParaControlKey, ParaControl *>::iterator it = m_ParaControlMap.find(key);
    if(it != m_ParaControlMap.end())
        para_control = it->second;
    else
    {
        para_control = m_pParaComtrolLib->GetParameterControl(strSystemId, strSystemType, strProcType,strParaName);
        m_ParaControlMap[key] = para_control;
    }
    return para_control;
}

bool ParaControlProvider::DisableParameterControl(int id, bool disable)
{
    bool is_suc = false;
    std::map<ParaControlKey, ParaControl *>::iterator iter = m_ParaControlMap.begin();
    std::map<ParaControlKey, ParaControl *>::iterator iter_end = m_ParaControlMap.end();
    for(; iter != iter_end; ++iter)
    {
        ParaControl* control = iter->second;
        BGI_ASSERT(control != NULL);
        /*
        if(control->GetId() == id)
        {
            control->Enable(!disable);
            is_suc = true;
            break;
        }
        */
    }
    return is_suc;
}
