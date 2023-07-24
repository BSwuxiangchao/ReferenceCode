/*
* @Abrief 参数处理引擎
* @note 负责参数的创建、调节、输出
* @version 1.0
* @author mosongjin 2021-07-30
*/

#ifndef _PARA_ENGINE_
#define _PARA_ENGINE_

#include <string>
#include <vector>
#include <qmutex.h>
#include <qsharedpointer.h>


struct ParaCmd;
class ProcParaManager;
class ParaBase;

class ParaEngine
{
public:

    static QSharedPointer<ParaEngine>& Instance()
    {

        if (m_pInstance.isNull())
        {
            QMutexLocker mutexLocker(&m_Mutex);
            if (m_pInstance.isNull())
                m_pInstance = QSharedPointer<ParaEngine>(new ParaEngine());
        }
        return m_pInstance;
    }

    
    ~ParaEngine();

    ProcParaManager* GetProcParaManager();

    bool ParaCommand(const ParaCmd& para_cmd);
    bool ParaCommand(const std::vector<ParaCmd>& para_cmds);

private:
    ParaEngine();

    bool ExecuteCommand(const ParaCmd& para_cmd);
    bool DoExecuteCommand(ParaBase* parameter, const ParaCmd& para_cmd);

private:
    void Initialize();
    void BeginChange();
    void EndChange();

    void Output();


private:
    static QMutex m_Mutex;
    static QSharedPointer<ParaEngine> m_pInstance;

    ProcParaManager* m_ProcParaManager;
};


#endif
