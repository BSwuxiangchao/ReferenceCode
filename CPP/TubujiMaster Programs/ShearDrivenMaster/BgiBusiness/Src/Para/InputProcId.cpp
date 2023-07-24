
#include "InputProcId.h"
#include "BgiLogger.h"
#include "SortingParaCommonName.h"

struct GTokenTable
{
    InputProc::Gen::EParaId ParaEnum;
    QString Token;
};
static const GTokenTable g_token_table[] = {
    {InputProc::Gen::start,         "Start"},
    {InputProc::Gen::SequencingDesc, "SequencingDesc"},
};
static int g_token_table_len = int(sizeof(g_token_table) / sizeof(g_token_table[0]));


InputProc::Gen::EParaId InputProc::Gen::GetParaId(QString strParaId)
{
    for (int i = 0; i < g_token_table_len; i++)
    {
        if (g_token_table[i].Token == strParaId)
        {
            return g_token_table[i].ParaEnum;
        }
    }

    BGI_ASSERT(false);
    return InputProc::Gen::start;
}

std::string InputProc::Gen::GetToken(int para_enum)
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

std::vector<int> InputProc::Gen::GetList()
{
    std::vector<int> list;
    for (int i = 0; i < g_token_table_len; i++)
        list.push_back(g_token_table[i].ParaEnum);
    return list;
}

struct RobotTokenTable
{
    InputProc::Robot::EParaId ParaEnum;
    QString Token;
};
static const RobotTokenTable rorot_token_table[] = {
    {InputProc::Robot::Start,                 "Start"},
    {InputProc::Robot::ParaOpt,                 "ParaOpt"},
};
static int robot_token_table_len = int(sizeof(rorot_token_table) / sizeof(rorot_token_table[0]));


InputProc::Robot::EParaId InputProc::Robot::GetParaId(QString strParaId)
{
    for (int i = 0; i < robot_token_table_len; i++)
    {
        if (rorot_token_table[i].Token == strParaId)
        {
            return rorot_token_table[i].ParaEnum;
        }
    }

    BGI_ASSERT(false);
    return InputProc::Robot::Start;
}

std::string InputProc::Robot::GetToken(int para_enum)
{
    std::string token;
    for (int i = 0; i < robot_token_table_len; i++)
    {
        if (rorot_token_table[i].ParaEnum == para_enum)
            token = rorot_token_table[i].Token.toUtf8().constData();
    }

    BGI_ASSERT(!token.empty());
    return token;
}

std::vector<int> InputProc::Robot::GetList()
{
    std::vector<int> list;
    for (int i = 0; i < robot_token_table_len; i++)
        list.push_back(rorot_token_table[i].ParaEnum);
    return list;
}

struct CoaterTokenTable
{
    InputProc::Coater::EParaId ParaEnum;
    QString Token;
};
static const CoaterTokenTable Coater_token_table[] = {
    {InputProc::Coater::Start,               "Start"},
};
static int Coater_token_table_len = int(sizeof(Coater_token_table) / sizeof(Coater_token_table[0]));


InputProc::Coater::EParaId InputProc::Coater::GetParaId(QString strParaId)
{
    for (int i = 0; i < Coater_token_table_len; i++)
    {
        if (Coater_token_table[i].Token == strParaId)
        {
            return Coater_token_table[i].ParaEnum;
        }
    }

    BGI_ASSERT(false);
    return InputProc::Coater::Start;
}

std::string InputProc::Coater::GetToken(int para_enum)
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

std::vector<int> InputProc::Coater::GetList()
{
    std::vector<int> list;
    for (int i = 0; i < Coater_token_table_len; i++)
        list.push_back(Coater_token_table[i].ParaEnum);
    return list;
}

struct ImagerTokenTable
{
    InputProc::Imager::EParaId ParaEnum;
    QString Token;
};
static const ImagerTokenTable Imager_token_table[] = {
    {InputProc::Imager::Start,               "Start"},
};
static int Imager_token_table_len = int(sizeof(Imager_token_table) / sizeof(Imager_token_table[0]));

InputProc::Imager::EParaId InputProc::Imager::GetParaId(QString strParaId)
{
    for (int i = 0; i < Imager_token_table_len; i++)
    {
        if (Imager_token_table[i].Token == strParaId)
        {
            return Imager_token_table[i].ParaEnum;
        }
    }

    BGI_ASSERT(false);
    return InputProc::Imager::Start;
}

std::string InputProc::Imager::GetToken(int para_enum)
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

std::vector<int> InputProc::Imager::GetList()
{
    std::vector<int> list;
    for (int i = 0; i < Imager_token_table_len; i++)
        list.push_back(Imager_token_table[i].ParaEnum);
    return list;
}

