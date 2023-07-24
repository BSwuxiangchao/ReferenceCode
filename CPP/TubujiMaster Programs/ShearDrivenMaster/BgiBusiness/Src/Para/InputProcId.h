/*
* @Abrief 定义输入类型的参数
* @note 定义各个业务系统的输入类型参数Id
* @version 1.0
* @author wuxiangchao 2023-02-24
*/

#ifndef _INPUT_PROC_ID_H_
#define _INPUT_PROC_ID_H_

#include "BgiProcId.h"
#include <vector>


//做Id定义时，规定要等于一个数字，这样就不会后添加的ID插在里面导致设置的数据失效
class InputProc : public BgiProc
{
public:
    class Gen
    {
    public:
        enum EParaId
        {
            start = 0,
            SequencingDesc,
        };

        static EParaId GetParaId(QString strParaId);
        static std::string GetToken(int para_enum);
        static std::vector<int> GetList();
    };

    class Robot
    {
    public:
        enum EParaId
        {
            Start = 0,
            ParaOpt = 1,
        };

        static EParaId GetParaId(QString strParaId);
        static std::string GetToken(int para_enum);
        static std::vector<int> GetList();
    };

    class Coater
    {
    public:
        enum EParaId
        {
            Start = 0,
           
        };

        static EParaId GetParaId(QString strParaId);
        static std::string GetToken(int para_enum);
        static std::vector<int> GetList();
    };

    class Imager
    {
    public:
        enum EParaId
        {
            Start = 0,
           
        };

        static EParaId GetParaId(QString strParaId);
        static std::string GetToken(int para_enum);
        static std::vector<int> GetList();
    };

};


#endif
