#include "ParaBase.h"
#include "ParaManager.h"
#include "ParaAgent.h"
#include "BgiLogger.h"
#include "BgiProcId.h"
#include "ParaObserver.h"
#include <math.h>

std::string ParaBase::Token() const
{
    return BgiProc::GetToken(m_ValueId);
}

bool ParaBase::Status() const
{
    ParaManager* manager = m_Agent->GetParaManager();
    BGI_ASSERT(manager != NULL);
    const unsigned int* status_ptr;
    manager->GetValue(m_StatusId, status_ptr);
    return (*status_ptr == 0xffffffff);
}

void ParaBase::Status(bool status)
{
    ParaManager* manager = m_Agent->GetParaManager();
    BGI_ASSERT(manager != NULL);
    unsigned int status_value = status ? 0xffffffff : 0x00;
    bool changed = manager->SetValue(m_StatusId, status_value);
    if (changed)
        manager->Trigger(m_StatusId);
}


int ParaBase::Version() const
{
    ParaManager* manager = m_Agent->GetParaManager();
    BGI_ASSERT(manager != NULL);

    int ver = manager->GetVersion(m_ValueId);
    return ver;
}

int ParaBase::ValueId() const
{
    return m_ValueId;
}

int ParaBase::StatusId() const
{
    return m_StatusId;
}
int ParaBase::Enum() const
{
    return m_ParaEnum;
}


void ParaBase::AddObserver(ParaObserver* observer, int mask)
{
    m_Agent->AddParaObserver(Enum(), mask, observer);
}

void ParaBase::DelObserver(ParaObserver* observer)
{
    m_Agent->DelParaObserver(Enum(), observer);
}

void ParaBase::AddReader(ParaRule* rule, int mask)
{
    m_Agent->AddReader(Enum(), mask, rule);
}

void ParaBase::AddWriter(ParaRule* rule, int mask)
{
    m_Agent->AddWriter(Enum(), mask, rule);
}