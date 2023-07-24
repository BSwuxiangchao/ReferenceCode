#include "ParaRule.h"
#include "BgiLogger.h"

ParaRule::ParaRule(const char* name)
{
    BGI_ASSERT(name != NULL);
    m_Name = name;
}

const char* ParaRule::GetName()
{
    return m_Name.c_str();
}