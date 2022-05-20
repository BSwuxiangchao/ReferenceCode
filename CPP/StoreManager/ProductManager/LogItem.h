#pragma once
class LogItem : public ui::ListContainerElement
{
public:
	LogItem();
	~LogItem();

	// 提供外部调用来初始化 item log text
	void InitSubControlWithLogText(const std::wstring& log);

private:
	ui::Label* m_pLabelLog;
};

