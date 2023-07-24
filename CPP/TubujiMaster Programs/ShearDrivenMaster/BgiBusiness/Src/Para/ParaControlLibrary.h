/*
* @Abrief 参数控件库
* @note 生成所有参数控件，提供控件生成接口
* @version 1.0
* @author mosongjin 2021-09-15
*/

#ifndef _PARA_CONTROL_LIBRARY_H
#define _PARA_CONTROL_LIBRARY_H

#include <qstring.h>
#include <map>
#include "BgiProcId.h"

class ParaControl;

class ParaControlLibrary
{
public:
    ParaControlLibrary();
    ~ParaControlLibrary();

    bool CheckParameterExist(QString strSystemId, QString strSystemType, QString strProcType, QString strParaName);
    ParaControl* GetParameterControl(QString strSystemId, QString strSystemType, QString strProcType, QString strParaName);
private:
    
    struct ProcParaInfo
    {
        int iParaEnumId;
        ParaControl* pParaControl;
    };

    int GetParaEnumId(QString strSystemId, QString strSystemType, QString strProcType, QString strParaName);

    void CreateProcParaInfo(BgiProc::ESystemId systemId,BgiProc::ESystemType systemType,
        BgiProc::EProcType procType, ProcParaInfo* para_desc, int para_desc_size);


private:
    std::map<int, ProcParaInfo> m_ParaInfoVec;
};

#endif // _PARA_CONTROL_LIBRARY_H
