/*
* @Abrief 定义Parameter
* @note 定义Parameter, LParameter类, ParameterAgent以及其派生类, 用该类设置和获取参数
* @version 1.0
* @author mosongjin 2021-07-27
*/

#ifndef _PARAMETER_H_
#define _PARAMETER_H_

#include <math.h>
#include <stdlib.h>
#include <vector>
#include <string>

#include "ParaManager.h"
#include "ParaAgent.h"
#include "BgiLogger.h"
#include "BgiProcId.h"
#include "ParaType.h"
#include "ParaPred.h"
#include "ParaArchive.h"
#include "ParaBase.h"


template<class T, class Arch_T = DefaultArchive<T>, class Pred_T = DefaultPred<T> > class Parameter : public ParaBase
{
public:
    Parameter(int id, ParaAgent& agent)
        : ParaBase(id, agent)
    {
        ParaManager* manager = m_Agent->GetParaManager();
        BGI_ASSERT(manager != NULL);

        //增加值参数
        manager->Add(m_ValueId, new Arch_T, new Pred_T);

        //增加状态参数
        manager->Add(m_StatusId, new DefaultArchive<unsigned int>, new DefaultPred<unsigned int>);
    }
    int Level() const{
        return 0;
    }
    void Level(int /*level*/){
    }
    int LevelCount() const{
        return 1;
    }
    void AddLevel(int /*level*/){
    }
    const T& Value() const{
        ParaManager* manager = m_Agent->GetParaManager();
        BGI_ASSERT(manager != NULL);

        const T* value_ptr;
        manager->GetValue(m_ValueId, value_ptr);
        return *value_ptr;
    }
    void Value(const T& value){
        ParaManager* manager = m_Agent->GetParaManager();
        BGI_ASSERT(manager != NULL);
        bool changed = manager->SetValue(m_ValueId, value);
        if (changed)
            manager->Trigger(m_ValueId);
    }

    const std::type_info& GetTypeInfo() const{
        return typeid(T);
    }
    const void* GetValue(const std::type_info& info, int level) const{
        BGI_ASSERT(level == 0);
        BGI_ASSERT(typeid(T) == info);
        const T& value = Value();
        return (void*)&value;
    }
    void SetValue(const void* value_ptr, const std::type_info& info){
        BGI_ASSERT(typeid(T) == info);
        const T* t_value_ptr = (const T*)value_ptr;
        Value(*t_value_ptr);
    }
};

template<class T, class Arch_T = DefaultArchive<T>, class Pred_T = DefaultPred<T> > class LParameter : public ParaBase
{
public:
    LParameter(int id, ParaAgent& agent, bool circular)
//        : ParameterBase(id, agent), m_Circular(circular)
    : m_Circular(circular)
    {
        ParaManager* manager = m_Agent->GetParaManager();
        BGI_ASSERT(manager != NULL);

        //增加值参数
        manager->Add(m_ValueId, new LevelValueArchive<T>(new Arch_T), new LevelValuePred<T>(new Pred_T));

        //增加状态参数
        manager->Add(m_StatusId, new DefaultArchive<unsigned int>, new DefaultPred<unsigned int>);
    }
    int Level() const{
        ParaManager* manager = m_Agent->GetParaManager();
        BGI_ASSERT(manager != NULL);

        const LevelValue<T>* value_ptr;
        manager->GetValue(m_ValueId, value_ptr);
        return value_ptr->Level;
    }
    void Level(int level){
        ParaManager* manager = m_Agent->GetParaManager();
        BGI_ASSERT(manager != NULL);

        const LevelValue<T>* value_ptr;
        manager->GetValue(m_ValueId, value_ptr, false);
        const std::vector<T>& list = value_ptr->List;
        int level_num = list.size();

        BGI_WARNING((level < 0) || (level >= level_num), "Parameter::SetLevel(%d): m_ValueId = %d, Level超出了范围[0, %d)", level, m_ValueId, level_num);

        if ((0 <= level) && (level < level_num))
        {
            LevelValue<T> new_value = *value_ptr;
            new_value.Level = level;
            bool changed = manager->SetValue(m_ValueId, new_value);
            if (changed)
                manager->Trigger(m_ValueId);
        }

    }
    int LevelCount() const{
        const std::vector<T>& list = List();
        return list.size();
    }
    void AddLevel(int delta_level){
        ParaManager* manager = m_Agent->GetParaManager();
        BGI_ASSERT(manager != NULL);

        const LevelValue<T>* value_ptr;
        manager->GetValue(m_ValueId, value_ptr, false);
        int level_num = value_ptr->List.size();
        int current_level = value_ptr->Level;
        BGI_WARNING((current_level < 0), "Parameter::AddLevel(%d): m_ValueId = %d, 当前Level无效", delta_level, m_ValueId);

        int target_level = -1;
        if (current_level >= 0)
        {
            target_level = (current_level + delta_level);
            if (m_Circular)
            {
                //循环调节
                target_level %= level_num;
                if (target_level < 0)
                    target_level += level_num;
            }
            else
            {
                //超出范围则限制在最值上
                if (target_level < 0)
                    target_level = 0;
                if (target_level >= level_num)
                    target_level = level_num - 1;
            }
            Level(target_level);
        }
    }
    const std::vector<T>& List() const{
        ParaManager* manager = m_Agent->GetParaManager();
        BGI_ASSERT(manager != NULL);

        const LevelValue<T>* value_ptr;
        manager->GetValue(m_ValueId, value_ptr);
        return value_ptr->List;
    }
    void List(const std::vector<T>& list){
        ParaManager* manager = m_Agent->GetParaManager();
        BGI_ASSERT(manager != NULL);
        BGI_ASSERT(list.size() > 0);

        int new_level = 0;
        if (manager->IsValueValid(m_ValueId))
        {
            const LevelValue<T>* value_ptr;
            manager->GetValue(m_ValueId, value_ptr, false);
            new_level = value_ptr->Level;

            int list_num = list.size();
            if (new_level < 0)
                new_level = 0;
            if (new_level >= list_num)
                new_level = list_num - 1;
        }

        LevelValue<T> new_value;
        new_value.Level = new_level;
        new_value.List = list;
        bool changed = manager->SetValue(m_ValueId, new_value);
        if (changed)
            manager->Trigger(m_ValueId);
    }
    const T& Value() const{
        ParaManager* manager = m_Agent->GetParaManager();
        BGI_ASSERT(manager != NULL);

        const LevelValue<T>* value_ptr;
        manager->GetValue(m_ValueId, value_ptr);
        return value_ptr->List[value_ptr->Level];
    }
    void Value(const T& value){
        ParaManager* manager = m_Agent->GetParaManager();
        BGI_ASSERT(manager != NULL);

        const LevelValue<T>* value_ptr;
        manager->GetValue(m_ValueId, value_ptr, false);

        int sel_level = 0;
        float min_diff = (float)fabsf((float)value - (float)value_ptr->List[0]);
        int size = value_ptr->List.size();
        for (int i = 1; i < size; i++)
        {
            float diff = (float)fabsf((float)value - (float)value_ptr->List[i]);
            if (min_diff > diff)
            {
                min_diff = diff;
                sel_level = i;
            }
        }

        LevelValue<T> new_value = *value_ptr;
        new_value.Level = sel_level;
        bool changed = manager->SetValue(m_ValueId, new_value);
        if (changed)
            manager->Trigger(m_ValueId);
    }

    const std::type_info& GetTypeInfo() const{
        return typeid(T);
    }
    const void* GetValue(const std::type_info& info, int level) const{
        ParaManager* manager = m_Agent->GetParaManager();
        BGI_ASSERT(manager != NULL);

        const LevelValue<T>* value_ptr;
        manager->GetValue(m_ValueId, value_ptr);
        BGI_ASSERT(level < (int)value_ptr->List.size());
        return (void*)&value_ptr->List[level];
    }
    void SetValue(const void* value_ptr, const std::type_info& info){
        BGI_ASSERT(typeid(T) == info);
        const T* t_value_ptr = (const T*)value_ptr;
        Value(*t_value_ptr);
    }
private:
    bool m_Circular;
};

typedef Parameter<float, DefaultArchive<float>, FloatPred> ParameterFloat;
typedef Parameter<int> ParameterInt;
typedef Parameter<std::string, StringArchive> ParameterString;

typedef LParameter<float, DefaultArchive<float>, FloatPred> LParameterFloat;
typedef LParameter<int> LParameterInt;



#endif