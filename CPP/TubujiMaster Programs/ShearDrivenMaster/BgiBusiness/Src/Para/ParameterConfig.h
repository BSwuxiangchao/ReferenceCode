/************************************* start-XCW *****************************************
 * 作    者：XCW
 * 创建时间：2022/6/14 20:36
 * 功能描述：
 ************************************* end-XCW *****************************************/
#pragma once
#include <QMetaType>
#include "BusinessExternDef.h"

class BGIBUSINE_CLASS_EXPORT ParameterConfig
{
public:

	// 引片参数结构体
	struct  InterfaceParamCfgChip
	{
	public:
		// 最小id
		int m_iIdMin;
		// 最大id
		int m_iIdMax;

		InterfaceParamCfgChip()
		{
			m_iIdMin = 0;
			m_iIdMax = 0;
		}

		InterfaceParamCfgChip& operator=(const InterfaceParamCfgChip& obj)
		{
			if (this != &obj)
			{
				m_iIdMin = obj.m_iIdMin;
				m_iIdMax = obj.m_iIdMax;
			}
			return *this;
		}

		bool operator==(const InterfaceParamCfgChip& obj)
		{
			if (m_iIdMin == obj.m_iIdMin && m_iIdMax == obj.m_iIdMax)
			{
				return true;
			}
			return false;
		}
	};

};


Q_DECLARE_METATYPE(ParameterConfig::InterfaceParamCfgChip);