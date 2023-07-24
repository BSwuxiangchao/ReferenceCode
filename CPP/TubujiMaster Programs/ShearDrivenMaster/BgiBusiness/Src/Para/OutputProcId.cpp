
#include "OutputProcId.h"
#include "BgiLogger.h"
#include "SortingParaCommonName.h"

struct GTokenTable
{
    OutputProc::Gen::EParaId ParaEnum;
    QString Token;
};
static const GTokenTable g_token_table[] = {
    {OutputProc::Gen::start,         "Start"},
};
static int g_token_table_len = int(sizeof(g_token_table) / sizeof(g_token_table[0]));

OutputProc::Gen::EParaId OutputProc::Gen::GetParaId(QString strParaId)
{
    for (int i = 0; i < g_token_table_len; i++)
    {
        if (g_token_table[i].Token == strParaId)
        {
            return g_token_table[i].ParaEnum;
        }
    }

    BGI_ASSERT(false);
    return OutputProc::Gen::start;
}

std::string OutputProc::Gen::GetToken(int para_enum)
{
    std::string token;
    for (int i = 0; i < g_token_table_len; i++)
    {
        if (g_token_table[i].ParaEnum == para_enum)
            token = g_token_table[i].Token.toUtf8().constData();
    }

    BGI_ASSERT(!token.empty());
    return token;
}

std::vector<int> OutputProc::Gen::GetList()
{
    std::vector<int> list;
    for (int i = 0; i < g_token_table_len; i++)
        list.push_back(g_token_table[i].ParaEnum);
    return list;
}

struct RobotTokenTable
{
    OutputProc::Robot::EParaId ParaEnum;
    QString Token;
};
static const RobotTokenTable Robot_token_table[] = {
    {OutputProc::Robot::ParaOpt,                            "ParaOpt"},
    {OutputProc::Robot::WorkStatus,                         "WorkStatus"},
    {OutputProc::Robot::RobotPoint,                         "RobotPoint"},
};
static int Robot_token_table_len = int(sizeof(Robot_token_table) / sizeof(Robot_token_table[0]));

OutputProc::Robot::EParaId OutputProc::Robot::GetParaId(QString strParaId)
{
    for (int i = 0; i < Robot_token_table_len; i++)
    {
        if (Robot_token_table[i].Token == strParaId)
        {
            return Robot_token_table[i].ParaEnum;
        }
    }

    BGI_ASSERT(false);
    return OutputProc::Robot::ParaOpt;
}

std::string OutputProc::Robot::GetToken(int para_enum)
{
    std::string token;
    for (int i = 0; i < Robot_token_table_len; i++)
    {
        if (Robot_token_table[i].ParaEnum == para_enum)
            token = Robot_token_table[i].Token.toUtf8().constData();
    }

    BGI_ASSERT(!token.empty());
    return token;
}

std::vector<int> OutputProc::Robot::GetList()
{
    std::vector<int> list;
    for (int i = 0; i < Robot_token_table_len; i++)
        list.push_back(Robot_token_table[i].ParaEnum);
    return list;
}

struct CoaterTokenTable
{
    OutputProc::Coater::EParaId ParaEnum;
    QString Token;
};
static const CoaterTokenTable Coater_token_table[] = {
    {OutputProc::Coater::ParaOpt,       "ParaOpt"},
   
};
static int Coater_token_table_len = int(sizeof(Coater_token_table) / sizeof(Coater_token_table[0]));

OutputProc::Coater::EParaId OutputProc::Coater::GetParaId(QString strParaId)
{
    for (int i = 0; i < Coater_token_table_len; i++)
    {
        if (Coater_token_table[i].Token == strParaId)
        {
            return Coater_token_table[i].ParaEnum;
        }
    }

    BGI_ASSERT(false);
    return OutputProc::Coater::ParaOpt;
}

std::string OutputProc::Coater::GetToken(int para_enum)
{
    std::string token;
    for (int i = 0; i < Coater_token_table_len; i++)
    {
        if (Coater_token_table[i].ParaEnum == para_enum)
            token = Coater_token_table[i].Token.toUtf8().constData();
    }

    BGI_ASSERT(!token.empty());
    return token;
}

std::vector<int> OutputProc::Coater::GetList()
{
    std::vector<int> list;
    for (int i = 0; i < Coater_token_table_len; i++)
        list.push_back(Coater_token_table[i].ParaEnum);
    return list;
}

struct ImagerTokenTable
{
    OutputProc::Imager::EParaId ParaEnum;
    QString Token;
};
static const ImagerTokenTable Imager_token_table[] = {
    {OutputProc::Imager::Start,                     "Start"},
};
static int Imager_token_table_len = int(sizeof(Imager_token_table) / sizeof(Imager_token_table[0]));

OutputProc::Imager::EParaId OutputProc::Imager::GetParaId(QString strParaId)
{
    for (int i = 0; i < Imager_token_table_len; i++)
    {
        if (Imager_token_table[i].Token == strParaId)
        {
            return Imager_token_table[i].ParaEnum;
        }
    }

    BGI_ASSERT(false);
    return OutputProc::Imager::Start;
}

std::string OutputProc::Imager::GetToken(int para_enum)
{
    std::string token;
    for (int i = 0; i < Imager_token_table_len; i++)
    {
        if (Imager_token_table[i].ParaEnum == para_enum)
            token = Imager_token_table[i].Token.toUtf8().constData();
    }

    BGI_ASSERT(!token.empty());
    return token;
}

std::vector<int> OutputProc::Imager::GetList()
{
    std::vector<int> list;
    for (int i = 0; i < Imager_token_table_len; i++)
        list.push_back(Imager_token_table[i].ParaEnum);
    return list;
}

