/*
* @Abrief 参数管理集合
* @note 用于管理输入或输出的参数
* @version 1.0
* @author mosongjin 2021-07-30
*/

#ifndef _PROC_PARA_MANAGER_H_
#define _PROC_PARA_MANAGER_H_

#include "ParaManager.h"
#include <vector>

class ParaAgent;

class ProcParaManager : public ParaManager
{
public:
    ProcParaManager(std::vector<ParaAgent*> input_proc_agent,
        std::vector<ParaAgent*> output_proc_agent);
    ~ProcParaManager();

    void ResetParameter();

    const std::vector<ParaAgent*>& GetInputProcAgent() const;
    const std::vector<ParaAgent*>& GetOutputProcAgent() const;

private:
    void ResetAgent(std::vector<ParaAgent*>& agent);
    void DelAgent(std::vector<ParaAgent*>& agent);
    void AddAgent(std::vector<ParaAgent*>& agent);

    std::vector<ParaAgent*> m_InputProcAgent;
    std::vector<ParaAgent*> m_OutputProcAgent;

    friend class ParaEngine;
};


#endif
