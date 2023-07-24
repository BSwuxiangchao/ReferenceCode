#include "BgiMpsEvent.h"


QMap<QString, QEvent::Type> BgiMpsEvent::m_BgiMpsEventIdMap;

BgiMpsEvent::BgiMpsEvent()
    : QEvent(Type(BGI_MPS_EVENT_TYPE)),
    HandledCount(0)
{

}

BgiMpsEvent::BgiMpsEvent(const QString& key)
    : QEvent(Type(BGI_MPS_EVENT_TYPE)),
    m_key(key),
    HandledCount(0)
{

}

BgiMpsEvent::BgiMpsEvent(const QString& key, const QVariantHash& params)
    : QEvent(Type(BGI_MPS_EVENT_TYPE)),
    m_key(key),
    m_params(params),
    HandledCount(0)
{

}

BgiMpsEvent::BgiMpsEvent(const QString& key, const QString& paramKey, const QVariant& var)
    : QEvent(Type(BGI_MPS_EVENT_TYPE)),
    m_key(key),
    HandledCount(0)
{
    m_params.insert(paramKey, var);
}


BgiMpsEvent::~BgiMpsEvent()
{

}

QString BgiMpsEvent::Key()
{
    return m_key;
}

void BgiMpsEvent::SetKey(const QString& key)
{
    m_key = key;
}

QString BgiMpsEvent::From()
{
    return m_from;
}
void BgiMpsEvent::SetFrom(const QString& from)
{
    m_from = from;
}

QString BgiMpsEvent::To()
{
    return m_to;
}

void BgiMpsEvent::SetTo(const QString& to)
{
    m_to = to;
}

QString BgiMpsEvent::Action()
{
    return m_action;
}

void BgiMpsEvent::SetAction(const QString& action)
{
    m_action = action;
}
const QVariantHash& BgiMpsEvent::Params()
{
    return m_params;
}

void BgiMpsEvent::SetParams(const QVariantHash& params)
{
    m_params = params;
}

void BgiMpsEvent::AddParams(const QString& key, const QVariant& var)
{
    m_params.insert(key, var);
}

bool BgiMpsEvent::BeHandled()
{
    return HandledCount > 0;
}

void BgiMpsEvent::AddHandled()
{
    ++HandledCount;
}

bool BgiMpsEvent::IsBgiMpsEvent(QEvent* event)
{
    if (!event)
    {
        return false;
    }

    BgiMpsEvent* uiEvent = dynamic_cast<BgiMpsEvent*>(event);
    if (!uiEvent)
    {
        return false;
    }

    return (BGI_MPS_EVENT_TYPE == event->type());
}
