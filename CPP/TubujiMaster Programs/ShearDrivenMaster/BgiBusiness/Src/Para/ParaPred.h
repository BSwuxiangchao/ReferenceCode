/*
* @Abrief 参数比较
* @note 比较参数是否相等
* @version 1.0
* @author mosongjin 2021-07-27
*/


#ifndef _PARA_PRED_H_
#define _PARA_PRED_H_

#include "EqualPred.h"
#include "BgiLogger.h"
//#include "ParaType.h"


template<class T> class DefaultPred : public EqualPred
{
public:
    bool IsEqual(const void* p1, const void* p2, const std::type_info& info) const {
        BGI_ASSERT(typeid(T) == info);
        return (*(const T*)p1 == *(const T*)p2);
    }
};

template<class T> class LevelValuePred : public EqualPred
{
public:
    LevelValuePred(EqualPred* pred)
        :m_Pred(pred)
    {}
    ~LevelValuePred() {
        delete m_Pred;
    }
    bool IsEqual(const void* p1, const void* p2, const std::type_info& info) const {
        BGI_ASSERT(typeid(LevelValue<T>) == info);
        const LevelValue<T>* lv1 = (const LevelValue<T>*)p1;
        const LevelValue<T>* lv2 = (const LevelValue<T>*)p2;

        if (lv1->Level != lv2->Level)
            return false;

        const std::vector<T>& vf1 = lv1->List;
        const std::vector<T>& vf2 = lv2->List;

        if (vf1.size() != vf2.size())
            return false;

        bool equal = true;
        int size = vf1.size();
        for (int i = 0; i < size; i++)
        {
            if (!m_Pred->IsEqual(&vf1[i], &vf2[i], typeid(T)))
            {
                equal = false;
                break;
            }
        }
        return equal;
    }
private:
    EqualPred* m_Pred;
};

class FloatPred : public EqualPred
{
public:
    bool IsEqual(const void* p1, const void* p2, const std::type_info& info) const {
        BGI_ASSERT(typeid(float) == info);
        return (fabsf(*(const float*)p1 - *(const float*)p2) < 0.000001f);
    }
};



#endif
