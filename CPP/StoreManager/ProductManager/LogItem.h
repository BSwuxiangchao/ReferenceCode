#pragma once
class LogItem : public ui::ListContainerElement
{
public:
	LogItem();
	~LogItem();

	// �ṩ�ⲿ��������ʼ�� item log text
	void InitSubControlWithLogText(const std::wstring& log);

private:
	ui::Label* m_pLabelLog;
};

