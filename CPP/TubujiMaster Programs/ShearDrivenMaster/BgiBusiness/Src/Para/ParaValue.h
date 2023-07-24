/*
* @Abrief 参数值
* @note 用于ParaManager保存参数值
* @version 1.0
* @author mosongjin 2021-07-27
*/


#ifndef _PARA_VALUE_H_
#define _PARA_VALUE_H_

#include <typeinfo>


class ParaValueBase
{
public:
    ParaValueBase(int id)
        :m_Id(id)
    {}

    virtual ~ParaValueBase() {}
    virtual const std::type_info & GetType() const = 0;
    virtual const void* GetValue() const = 0;
    virtual void SetValue(const void* data, const std::type_info& info) = 0;

protected:
    int m_Id;
};

template<class T> class ParaValue : public ParaValueBase
{
public:
    ParaValue(int id, const T& value)
        :ParaValueBase(id)
    {
        m_Data = new T(value);
    }

    ~ParaValue()
    {
        delete m_Data;
        m_Data = NULL;
    }

    const std::type_info& GetType() const {
        return typeid(T);
    }

    const void* GetValue() const {
        return m_Data;
    }

    void SetValue(const void* data, const std::type_info& info)
    {
        BGI_ASSERT(typeid(T) == info);
        *m_Data = *(T*)data;
    }

private:
    T* m_Data;
};



#endif
