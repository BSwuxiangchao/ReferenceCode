#include "stdafx.h"
#include "UserConfig.h"


UserConfig::UserConfig()
{

}

UserConfig::~UserConfig()
{

}

UINT UserConfig::GetDefaultIcon()
{
	return m_iconId;
}

void UserConfig::SetDefaultIcon(UINT iconId)
{
	m_iconId = iconId;
}
