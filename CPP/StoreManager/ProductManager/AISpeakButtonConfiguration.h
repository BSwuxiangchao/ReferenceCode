#pragma once


class AISpeakButtonConfiguration
{
public:
	SINGLETON_DEFINE(AISpeakButtonConfiguration);

private:
	AISpeakButtonConfiguration();
	virtual ~AISpeakButtonConfiguration();

public:
	void GetTextByIndex(unsigned int index, std::wstring &outText);
	void GetRandomSpeakByText(const std::wstring &text, std::wstring& outSpeak);

private:
	void LoadConfiguration(std::wstring filePath);

	// ���[a, b)��Χ�ڵ��������
	int RandInt(int a, int b);
private:
	std::vector<std::tuple<std::wstring, std::vector<std::wstring> > >  m_textSpeakList;
};
 