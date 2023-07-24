/*
* @Abrief 定义输出类型的参数
* @note 定义各个业务系统的输出类型参数Id
* @version 1.0
* @author wuxiangchao 2023-02-24
*/

#ifndef _OUTPUT_PROC_ID_H_
#define _OUTPUT_PROC_ID_H_

#include "BgiProcId.h"
#include <vector>
#include <qstring.h>


//做Id定义时，规定要等于一个数字，这样就不会后添加的ID插在里面导致设置的数据失效
class OutputProc : public BgiProc
{
public:
    class Gen
    {
    public:
        enum EParaId
        {
            start = 0,
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
            ParaOpt = 0,
            WorkStatus,
            RobotPoint,
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
            ParaOpt = 0,
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
            Start               = 0,
        };

        static EParaId GetParaId(QString strParaId);
        static std::string GetToken(int para_enum);
        static std::vector<int> GetList();
    };

};


#endif
