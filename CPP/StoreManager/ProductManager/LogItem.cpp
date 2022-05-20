#include "stdafx.h"
#include "LogItem.h"


LogItem::LogItem()
{
}


LogItem::~LogItem()
{
}

void LogItem::InitSubControlWithLogText(const std::wstring& log)
{
	m_pLabelLog = dynamic_cast<ui::Label*>(FindSubControl(L"label_log"));
	auto currentTime = nbase::Time::Now().ToTimeStruct(true);
	int hour = currentTime.hour();
	int minute = currentTime.minute();
	int second = currentTime.second();
	auto logWithTime = nbase::StringPrintf(L"%02d:%02d:%02d  %s", hour, minute, second, log.c_str());

	m_pLabelLog->SetText(logWithTime);
}
