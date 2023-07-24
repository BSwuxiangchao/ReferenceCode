/*
* @Abrief 单例工具
* @note 单例工具，通过此工具获取共同的实例
* @version 1.0
* @author mosongjin 2021-10-08
*/

#ifndef _BGI_SINGLETON_TOOL_H
#define _BGI_SINGLETON_TOOL_H


template <typename T>
class BgiSingleTool
{
public:
    static T* GetInstance();
    static void ReleaseInstance();
protected:
    BgiSingleTool();
    virtual ~BgiSingleTool();
protected:
    static T* m_pInstance;
};

template <typename T>
T* BgiSingleTool<T>::m_pInstance = NULL;

template <typename T>
BgiSingleTool<T>::BgiSingleTool()
{

}

template <typename T>
BgiSingleTool<T>::~BgiSingleTool()
{

}


template <typename T>
T* BgiSingleTool<T>::GetInstance()
{
    if (NULL == m_pInstance)
    {
        m_pInstance = new T();
    }
    return m_pInstance;
}

template <typename T>
void BgiSingleTool<T>::ReleaseInstance()
{
    if (NULL != m_pInstance)
    {
        delete m_pInstance;
        m_pInstance = NULL;
    }
}



#endif