/************************************* start-XCW *****************************************
 * 作    者：XCW
 * 创建时间：2022/9/14 21:48
 * 功能描述：分拣、合成、转运共同使用的结构数据
 ************************************* end-XCW *****************************************/
#pragma once
#include <QMetaType>
#include "BgiProcId.h"
#include <QVector>
#include <QTime>

class BgiUnionParaType
{
public:
	// 工作类型枚举
	enum EModuleWorkType
	{
		WorkTypeManual = 0, // 手动模式
		WorkTypeAuto	= 1, // 自动模式
	};
	// 当前工作状态枚举
	enum EModuleWorkState
	{
		WorkStateNotRun	= 0, // 未运行
		WorkStateStart	= 1, // 本次加工开始中
		WorkStatePause	= 2, // 本次加工暂停中
		WorkStateEnd	= 3, // 本次加工结束中
	};

	// 子轮描述
	enum ESubRound
	{
		SubA = 0,
		SubB = 1,
	};

	// 描述各个部分的工作状态结构体
	struct _tagModuleWorkState
	{
		// 系统名称
		BgiProc::ESystemId m_eSystemName;
		// 模块名称
		BgiProc::ESystemType m_eModuleType;
		// 工作类型：手动，自动
		EModuleWorkType m_eModuleWorkType;
		// 当前工作状态：NotRun,Start,Pause,End,
		EModuleWorkState m_eModuleWorkState;
		// 记录时间
		qint64 m_timeMs;

		_tagModuleWorkState()
		{
			m_eSystemName = BgiProc::SystemA;
			m_eModuleType = BgiProc::GeneralType;
			m_eModuleWorkType = WorkTypeManual;
			m_eModuleWorkState = WorkStateNotRun;
			m_timeMs = QDateTime::currentMSecsSinceEpoch();
		}
		_tagModuleWorkState(const _tagModuleWorkState& obj)
		{
			m_eSystemName = obj.m_eSystemName;
			m_eModuleType = obj.m_eModuleType;
			m_eModuleWorkType = obj.m_eModuleWorkType;
			m_eModuleWorkState = obj.m_eModuleWorkState;
			m_timeMs = obj.m_timeMs;
		}
		_tagModuleWorkState& operator=(const _tagModuleWorkState& obj)
		{
			if (this != &obj)
			{
				m_eSystemName = obj.m_eSystemName;
				m_eModuleType = obj.m_eModuleType;
				m_eModuleWorkType = obj.m_eModuleWorkType;
				m_eModuleWorkState = obj.m_eModuleWorkState;
				m_timeMs = obj.m_timeMs;
			}
			return *this;
		}
		bool operator==(const _tagModuleWorkState& obj) const
		{
			if (m_eSystemName == obj.m_eSystemName&&
				m_eModuleType == obj.m_eModuleType&&
				m_eModuleWorkType == obj.m_eModuleWorkType&&
				m_eModuleWorkState == obj.m_eModuleWorkState && 
				m_timeMs == obj.m_timeMs)
			{
				return true;
			}
			return false;
		}
	};

	// 料仓数据结构体
	struct _tagBinData
	{
		// 料仓ID
		int m_iBinId;
		// 存放加工的碱基名称
		char m_cBase;
		// 芯片数量
		int m_iBaseCount;

		_tagBinData()
		{
			m_iBinId = 0;
			m_cBase = ' ';
			m_iBaseCount = 0;
		}
		_tagBinData(const _tagBinData& obj)
		{
			m_iBinId = obj.m_iBinId;
			m_cBase = obj.m_cBase;
			m_iBaseCount = obj.m_iBaseCount;
		}
		_tagBinData& operator=(const _tagBinData& obj)
		{
			if (this != &obj)
			{
				m_iBinId = obj.m_iBinId;
				m_cBase = obj.m_cBase;
				m_iBaseCount = obj.m_iBaseCount;
			}
			return *this;
		}
		bool operator==(const _tagBinData& obj)const
		{
			if (m_iBinId == obj.m_iBinId && 
				m_cBase == obj.m_cBase&&
				m_iBaseCount == obj.m_iBaseCount)
			{
				return true;
			}
			return false;
		}

	};

	// 加工芯片数据元素结构体：轮次，子轮，料仓芯片数量
	struct _tagChipDataElement
	{
		// 当前轮次
		int m_iCurRound;
		// 当前子轮
		ESubRound m_eSubRound;
		// 料仓数据
		_tagBinData m_sBin1;
		_tagBinData m_sBin2;
		_tagBinData m_sBin3;
		_tagBinData m_sBin4;
		_tagBinData m_sBin5;
		_tagBinData m_sBin6;

		_tagChipDataElement()
		{
			m_iCurRound = 0;
			m_eSubRound = SubA;

			m_sBin1.m_iBinId = 1;
			m_sBin1.m_cBase = 'A';
			m_sBin2.m_iBinId = 2;
			m_sBin2.m_cBase = 'T';
			m_sBin3.m_iBinId = 3;
			m_sBin3.m_cBase = 'X';
			m_sBin4.m_iBinId = 4;
			m_sBin4.m_cBase = 'Y';
			m_sBin5.m_iBinId = 5;
			m_sBin5.m_cBase = 'C';
			m_sBin6.m_iBinId = 6;
			m_sBin6.m_cBase = 'G';
		}
		_tagChipDataElement(const _tagChipDataElement& obj)
		{
			m_iCurRound = obj.m_iCurRound;
			m_eSubRound = obj.m_eSubRound;
			m_sBin1 = obj.m_sBin1;
			m_sBin2 = obj.m_sBin2;
			m_sBin3 = obj.m_sBin3;
			m_sBin4 = obj.m_sBin4;
			m_sBin5 = obj.m_sBin5;
			m_sBin6 = obj.m_sBin6;
		}
		_tagChipDataElement& operator=(const _tagChipDataElement& obj)
		{
			if (this != &obj)
			{
				m_iCurRound = obj.m_iCurRound;
				m_eSubRound = obj.m_eSubRound;
				m_sBin1 = obj.m_sBin1;
				m_sBin2 = obj.m_sBin2;
				m_sBin3 = obj.m_sBin3;
				m_sBin4 = obj.m_sBin4;
				m_sBin5 = obj.m_sBin5;
				m_sBin6 = obj.m_sBin6;
			}
			return *this;
		}

		bool operator==(const _tagChipDataElement& obj) const
		{
			if (m_iCurRound == obj.m_iCurRound &&
				m_eSubRound == obj.m_eSubRound &&
				m_sBin1 == obj.m_sBin1&&
				m_sBin2 == obj.m_sBin2&&
				m_sBin3 == obj.m_sBin3&&
				m_sBin4 == obj.m_sBin4&&
				m_sBin5 == obj.m_sBin5&&
				m_sBin6 == obj.m_sBin6)
			{
				return true;
			}
			return false;
		}
	};

	// 本次需要加工的芯片数据内容结构体
	struct _tagProcessChipData
	{
	private:
		// 发出加工数据消息的模块列表（该数据消息在传递过程中只许添加不可删除，为了溯源数据消息经过的加工模块）
		QVector<_tagModuleWorkState> m_vecModuleWorkState;
		// 芯片数据信息元素
		_tagChipDataElement m_sChipDataElement;
		// 加工芯片数据元素可改变状态
		bool m_bChipDataElementChange;
		// 加工状态
		bool m_bFinished;
		// 加工芯片数据元素创建时间
		qint64 m_iCreateTimeStamp;
	public:
		// 消息去往的模块（即下一个加工模块）
		_tagModuleWorkState m_sToModuleWorkState;
		
		_tagProcessChipData()
		{
			m_vecModuleWorkState.clear();
			m_bChipDataElementChange = true;
			m_bFinished = false;
			m_iCreateTimeStamp = QDateTime::currentMSecsSinceEpoch();
		}
		// 拷贝构造
		_tagProcessChipData(const _tagProcessChipData& obj)
		{
			m_vecModuleWorkState = obj.m_vecModuleWorkState;
			m_sToModuleWorkState = obj.m_sToModuleWorkState;
			m_sChipDataElement = obj.m_sChipDataElement;
			m_bChipDataElementChange = obj.m_bChipDataElementChange;
			m_bFinished = obj.m_bFinished;
			m_iCreateTimeStamp = obj.m_iCreateTimeStamp;
		}

		_tagProcessChipData& operator=(const _tagProcessChipData& obj)
		{
			if (this != &obj)
			{
				m_vecModuleWorkState = obj.m_vecModuleWorkState;
				m_sToModuleWorkState = obj.m_sToModuleWorkState;
				m_sChipDataElement = obj.m_sChipDataElement;
				m_bChipDataElementChange = obj.m_bChipDataElementChange;
				m_bFinished = obj.m_bFinished;
				m_iCreateTimeStamp = obj.m_iCreateTimeStamp;
			}
			return *this;
		}

		// 添加加工消息模块的信息
		void AddModuleWorkState(const _tagModuleWorkState& obj)
		{
			// 添加加工的时间
			_tagModuleWorkState data = obj;
			data.m_timeMs = QDateTime::currentMSecsSinceEpoch();
			m_vecModuleWorkState.push_back(data);
		}
		// 获取数据消息来自的模块
		bool GetFromModuleWorkState(_tagModuleWorkState& obj)
		{
			// 尾部元素就是上加工模块的消息
			if (m_vecModuleWorkState.isEmpty())
			{
				return false;
			}
			obj = m_vecModuleWorkState.at(m_vecModuleWorkState.size()-1);
			return true;
		}
		// 获取加工消息模块的列表
		const QVector<_tagModuleWorkState>& GetModuleWorkStateList() const
		{
			return m_vecModuleWorkState;
		}
		
		// 设置芯片数据信息元素，只允许设置一次，避免在传输过程中误操作修改其值
		bool SetChipDataElement(const _tagChipDataElement& obj)
		{
			if (false == m_bChipDataElementChange)
			{
				return false;
			}
			m_bChipDataElementChange = false;
			m_sChipDataElement = obj;
			return true;
		}
		// 设置芯片数据信息元素
		const _tagChipDataElement& GetChipDataElement() const 
		{
			return m_sChipDataElement;
		}

		// 计算料仓里的芯片数量
		int CalculChipCount()
		{
			int iTotal = 0;
			iTotal += m_sChipDataElement.m_sBin1.m_iBaseCount;
			iTotal += m_sChipDataElement.m_sBin2.m_iBaseCount;
			iTotal += m_sChipDataElement.m_sBin3.m_iBaseCount;
			iTotal += m_sChipDataElement.m_sBin4.m_iBaseCount;
			iTotal += m_sChipDataElement.m_sBin5.m_iBaseCount;
			iTotal += m_sChipDataElement.m_sBin6.m_iBaseCount;
			
			return iTotal;
		}
		// 设置加工状态
		void SetFinished(const bool& bFinish)
		{
			m_bFinished = bFinish;
		}
		// 获取加工芯片数据元素创建时间
		qint64 GetCreateTimeStamp()const
		{
			return m_iCreateTimeStamp;
		}
	};

protected:

private:

};

Q_DECLARE_METATYPE(BgiUnionParaType::_tagModuleWorkState);
Q_DECLARE_METATYPE(BgiUnionParaType::_tagBinData);
Q_DECLARE_METATYPE(BgiUnionParaType::_tagProcessChipData);

