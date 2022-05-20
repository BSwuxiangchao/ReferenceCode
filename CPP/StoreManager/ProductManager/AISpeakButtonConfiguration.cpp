#include "stdafx.h"
#include "AISpeakButtonConfiguration.h"
#include <cstdlib>
#include <ctime>

const std::wstring SpeakButtonIniFile = TEXT(".\\data\\speak.ini");

AISpeakButtonConfiguration::AISpeakButtonConfiguration()
{
	LoadConfiguration(SpeakButtonIniFile);
}

AISpeakButtonConfiguration::~AISpeakButtonConfiguration()
{

}

void AISpeakButtonConfiguration::GetTextByIndex(unsigned int index, std::wstring &outText)
{
	if (index < m_textSpeakList.size())
	{
		outText = std::get<0>(m_textSpeakList[index]);
	}
}

void AISpeakButtonConfiguration::GetRandomSpeakByText(const std::wstring &text, std::wstring& outSpeak)
{
	for (auto& item : m_textSpeakList)
	{
		if (text.compare(std::get<0>(item)) == 0)
		{
			auto size = std::get<1>(item).size();
			auto index = RandInt(0, size);
			outSpeak = std::get<1>(item)[index];

			break;
		}
	}
}

void AISpeakButtonConfiguration::LoadConfiguration(std::wstring filePath)
{
	CSimpleIni configController;
	auto fullPath = nbase::win32::GetCurrentModuleDirectory() + filePath;
	auto error = configController.LoadFile(fullPath.c_str());
	if (error != SI_OK)
	{
		LogError("Http", "º”‘ÿ≈‰÷√Œƒº˛ %s ¥ÌŒÛ: %d", fullPath.c_str(), error);
		return;
	}

	for (int i = 1; ; ++i)
	{
		auto section = nbase::StringPrintf(TEXT("btn%d"), i);
		std::wstring buttonText = configController.GetValue(section.c_str(), TEXT("caption"), TEXT(""));
		if (buttonText.empty())
			break;

		std::vector<std::wstring> speakList;
		for (int j = 1; ; ++j)
		{
			auto key = nbase::StringPrintf(TEXT("utter%d"), j);
			std::wstring speak = configController.GetValue(section.c_str(), key.c_str(), TEXT(""));
			if (speak.empty())
				break;
			
			speakList.push_back(speak);
		}

		m_textSpeakList.push_back(std::make_tuple(buttonText, speakList));
	}
}

int AISpeakButtonConfiguration::RandInt(int a, int b)
{
	srand((int)time(0));

	return (rand() % (b - a) + a);
}
