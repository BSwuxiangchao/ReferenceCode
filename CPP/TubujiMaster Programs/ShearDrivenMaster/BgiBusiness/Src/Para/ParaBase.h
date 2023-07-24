/*
* @Abrief 定义参数基本类
* @note 参数的基类
* @version 1.0
* @author mosongjin 2021-07-27
*/

#ifndef _PARA_BASE_H_
#define _PARA_BASE_H_


#include <stdlib.h>
#include <vector>
#include <string>
#include <typeinfo>
#include "ParaAgent.h"

using namespace std; 

class ParaManager;
class ParaObserver;
class ParaRule;

class ParaBase
{
public:
    ParaBase(int para_enum, ParaAgent& agent)
        : m_ParaEnum(para_enum), m_Agent(&agent)
    {
        m_ValueId = m_Agent->GetValueId(m_ParaEnum);
        m_StatusId = m_Agent->GetStatusId(m_ParaEnum);
    }
    virtual ~ParaBase() {}

    virtual int Level() const = 0;
    virtual void Level(int level) = 0;
    virtual int LevelCount() const = 0;
    virtual void AddLevel(int level) = 0;

    virtual const std::type_info& GetTypeInfo() const = 0;
    virtual const void* GetValue(const std::type_info& info, int level) const = 0;
    virtual void SetValue(const void* value_ptr, const std::type_info& info) = 0;

    std::string Token() const;
    bool Status() const;
    void Status(bool status);
    int Version() const;
    int ValueId() const;
    int StatusId() const;
    int Enum() const;

    void AddObserver(ParaObserver* observer, int mask);
    void DelObserver(ParaObserver* observer);
    void AddReader(ParaRule* rule, int mask);
    void AddWriter(ParaRule* rule, int mask);

protected:
    int m_ParaEnum;
    int m_ValueId;
    int m_StatusId;
    ParaAgent* m_Agent;
};


#endif
