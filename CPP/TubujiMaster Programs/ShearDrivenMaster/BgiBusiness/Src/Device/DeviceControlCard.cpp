#include "DeviceControlCard.h"
#include "LTDMC.h"
#include "BgiLogger.h"
#include <qstring.h>
#include "OperateRecordInfo.h"
#include "PortConfigDoc.h"

#define  BOARD_COUNT		2	// 使用的雷赛卡数量
#define  BOARD_HAVE_CAN		1	// 每个雷赛总线卡含有使用的扩展CAN模块数量
#define  BOARD_CAN_1_START	16  // CAN1起始IO
#define  BOARD_CAN_1_END	63  // CAN1结束IO
#define  BOARD_CAN_2_START	64  // CAN2起始IO
#define  BOARD_CAN_2_END	121  // CAN2结束IO


_tagDmcBoardState::_tagDmcBoardState()
{
	m_strInSystemName = "";
	m_iBoardId = 0;
	m_bBoardConnect = false;
	m_iBoardCardId = 0;
	m_iCanCount = BOARD_HAVE_CAN;
	m_bCanConnect = false;
	m_iCanStartIo = 0;
	m_iCanEndIo = 0;
	m_bDDMotor = false;
}
_tagDmcBoardState& _tagDmcBoardState::operator=(const _tagDmcBoardState& obj)
{
	if (this != &obj)
	{
		m_strInSystemName = obj.m_strInSystemName;
		m_iBoardId = obj.m_iBoardId;
		m_bBoardConnect = obj.m_bBoardConnect;
		m_iBoardCardId = obj.m_iBoardCardId;
		m_iCanCount = obj.m_iCanCount;
		m_bCanConnect = obj.m_bCanConnect;
		m_iCanStartIo = obj.m_iCanStartIo;
		m_iCanEndIo = obj.m_iCanEndIo;
		m_bDDMotor = obj.m_bDDMotor;
	}
	return *this;
}

DeviceControlCard::DeviceControlCard()
	:m_iBoardCount(0)
{
	m_mapBoard.clear();
	
	InitBoard();

	/*dmc_board_close();

	m_iBoardCount = dmc_board_init();
	if (m_iBoardCount > 0)
	{
		for (int i = 0; i < m_iBoardCount; i++)
		{
			short re_code = dmc_write_sevon_pin(i, 0, 0);
			re_code = nmc_set_connect_state(i, 2, 1, 0);
		}
	}
	else
	{
		BGI_MESSAGE("Have not baord!");
	}*/
}

DeviceControlCard::~DeviceControlCard()
{
	for (int i = 0; i < m_iBoardCount; i++)
	{
		nmc_set_connect_state(i, 2, 0, 0);
		dmc_write_sevon_pin(i, 0, 1);
	}
	
	dmc_board_close();
}

// 初始化雷赛控制卡
void DeviceControlCard::InitBoard()
{
	m_mapBoard.clear();
	QString strCanNUm = "";
	PortConfigDoc::Instance()->GetPortValue("LesaiBoardHaveCan", strCanNUm);

	// 初始化卡的列表
	for (short i = 0; i < BOARD_COUNT; ++i)
	{
		_tagDmcBoardState board;
		board.m_strInSystemName = QString("系统%1").arg(i+1);
		board.m_iBoardId = i;
		board.m_iCanCount = strCanNUm.toInt();
		m_mapBoard.insert(i, board);
	}

	dmc_board_close();
	m_iBoardCount = dmc_board_init();
	RecordData data;
	data.m_strModelName = "雷赛控制卡";
	data.m_strInfoType = "初始化";
	if (0 >= m_iBoardCount)
	{
		// 全部总线卡初始化失败
		data.m_strContent = "系统1、系统2总线卡初始化失败，请检查总线卡线路是否正常连接！";
		data.m_color = Qt::red;
		OperateRecordInfo::AddRecordData(data);

		return;
	}

	// 判断是否初始化成功，读取总线卡号，直接使用
	bool bInitFail = false;
	QMap<short, _tagDmcBoardState>::iterator begin = m_mapBoard.begin();
	for (; begin != m_mapBoard.end(); ++begin)
	{
		DWORD cardId = 0;
		short iRet = dmc_get_card_ID(begin->m_iBoardId, &cardId);
		if (0 == iRet)
		{
			// 初始化成功
			begin->m_bBoardConnect = true;
			begin->m_iBoardCardId = cardId;
			// 连接CAN扩展模块
			iRet = nmc_set_connect_state(begin->m_iBoardId, begin->m_iCanCount, 1, 0);
			if (0 == iRet)
			{
				// 连接CAN成功
				begin->m_bCanConnect = true;
				begin->m_iCanStartIo = BOARD_CAN_1_START;
				begin->m_iCanEndIo = BOARD_CAN_2_END;
			}
			else
			{
				// 连接CAN失败
				begin->m_bCanConnect = false;
				begin->m_iCanStartIo = 0;
				begin->m_iCanEndIo = 0;
				data.m_strContent += QString("(%1总线卡初始化成功，但是CAN模块连接失败)").arg(begin->m_strInSystemName);
				bInitFail = true;
			}
			// 使能DD马达
			iRet = dmc_write_sevon_pin(begin->m_iBoardId, 0, 0);
			if (0 == iRet)
			{
				// 使能DD马达成功
				begin->m_bDDMotor = true;
			}
			else
			{
				// 使能DD马达失败
				begin->m_bDDMotor = false;
				data.m_strContent += QString("(%1总线卡初始化成功，但是DD马达使能失败，请检查DD马达是否正常上电或者总线卡与DD马达驱动器线路连接是否正常)").arg(begin->m_strInSystemName);
				bInitFail = true;
			}
		}
		else
		{
			// 初始化失败
			begin->m_bBoardConnect = false;
			begin->m_iBoardCardId = 0;
			data.m_strContent += QString("(%1总线卡初始化失败)").arg(begin->m_strInSystemName);
			bInitFail = true;
		}
	}

	// 判断初始化和连接状态
	if (bInitFail)
	{
		data.m_strContent += "：请检查总线卡的线路，确认通路正常后再重新打开软件使用！";
		data.m_color = Qt::red;
	}
	else
	{
		data.m_strContent = QString("系统1、系统2总线卡，初始化、CAN模块连接、DD马达使能成功！");
		data.m_color = Qt::green;
	}

	OperateRecordInfo::AddRecordData(data);
}

int DeviceControlCard::SwitchFluidPort(int card_id, int port_id, bool bOpen)
{
	// 判断该IO口是否已经初始化
	int iFindInit = FindIoPortInit(card_id, port_id);
	if (0 != iFindInit)
	{
		// 未初始化
		RecordData data;
		data.m_bShow = false;
		data.m_strModelName = "控制总线卡";
		data.m_strInfoType = "写入IO口操作";
		data.m_strContent = QString("卡号：%1，IO号：%2，该IO口未初始化，错误码为：%3！").arg(card_id).arg(port_id).arg(iFindInit);
		OperateRecordInfo::AddRecordData(data);

		return iFindInit;
	}

	int on_off = 1;
	if (bOpen)
	{
		on_off = 0;
//		QString str =  QString("card:%1,port:%2,open:%3").arg(card_id).arg(port_id).arg(bOpen);
//		BGI_MESSAGE(str.toLocal8Bit());
	}
		
	int i_ret = -1;
	//常规模块处理16个IO端口
	//扩展模块0处理48个IO端口
	//扩展模块1处理48个端口
	if (port_id >= 0 && port_id <= 15)
	{
		/*QString str =  QString("card:%1,port:%2,open:%3").arg(card_id).arg(port_id).arg(bOpen);
		BGI_MESSAGE(str.toLocal8Bit());*/
		i_ret = dmc_write_outbit(card_id, port_id, on_off);
	}
	else if (port_id >= 16 && port_id <= 63)
	{
		i_ret = nmc_write_outbit(card_id, 1, port_id - 16, on_off);
		/*if (16 == port_id)
		{	
			BGI_MESSAGE(QString("Run photo: %1,%2,%3:").arg(port_id).arg(on_off).arg(card_id).toLocal8Bit());
		}*/
	}
	else if (port_id >= 64 && port_id <= 121)
	{
		i_ret = nmc_write_outbit(card_id, 2, port_id - 16 - 48, on_off);
	}
	//V101、V102等电磁阀无论打开还是关闭返回值都是 0
	if (i_ret != 0)
	{
		QString str = QString("Switch Fluid Port fail, card_id = %1,port_id:%2, switch:%d").arg(card_id).arg(port_id).arg(on_off);
		BGI_WARNING(str.toStdString().c_str());
	}

	return i_ret;
}


short DeviceControlCard::GetElecPressure(int card_id, int channel, double* elev_value)
{
	return dmc_get_ad_input(card_id, channel, elev_value);
}

int DeviceControlCard::ReadPortValue(int card_id, int port_id, unsigned short& value)
{
	// 判断该IO口是否已经初始化
	int iFindInit = FindIoPortInit(card_id, port_id);
	if (0 != iFindInit)
	{
		// 未初始化
		RecordData data;
		data.m_bShow = false;
		data.m_strModelName = "控制总线卡";
		data.m_strInfoType = "读取IO口操作";
		data.m_strContent = QString("卡号：%1，IO号：%2，该IO口未初始化，错误码为：%3！").arg(card_id).arg(port_id).arg(iFindInit);
		OperateRecordInfo::AddRecordData(data);

		return iFindInit;
	}

	//常规模块处理16个IO端口
	//扩展模块0处理48个IO端口
	//扩展模块1处理48个端口
	int recode = -1;
	if (port_id >= 0 && port_id <= 15)
	{
		value = dmc_read_inbit(card_id, port_id);
		recode = 1;
	}
	else if (port_id >= 16 && port_id <= 63)
	{
//		recode = nmc_write_outbit(card_id, 1, port_id - 16, value);
		recode = nmc_read_inbit(card_id, 1, port_id - 16, &value);
	}
	else if (port_id >= 64 && port_id <= 121)
	{
//		recode = nmc_write_outbit(card_id, 2, port_id - 16 - 48, value);
		recode = nmc_read_inbit(card_id, 2, port_id - 16 - 48, &value);
	}

	return recode;
}

short DeviceControlCard::MoveTargetDist(int card_id, int port_id, double dist, int mode)
{
	return dmc_pmove(card_id, port_id, dist, mode);
}

/************************************* start-XCW *****************************************
 * 作    者：XCW
 * 功能描述：控制IO口是判断该IO口是否已经初始化成功;
 * 时    间：2022/9/14 13:56
 * 入    参：card_id：总线卡ID
 * 入    参：port_id：IO口
 * 出    参：无
 * 返    回：0-已初始化，-1001-总线卡未初始化，-1002-总线卡不存在，-1003-总线卡已经初始化但是CAN模块未连接
 ************************************* end-XCW *****************************************/
short DeviceControlCard::FindIoPortInit(const int& card_id, const int& port_id)
{
	QMap<short, _tagDmcBoardState>::iterator find =	m_mapBoard.find(card_id);
	if (find == m_mapBoard.end())
	{
		// 总线卡不存在
		return -1002;
	}

	if (!find->m_bBoardConnect)
	{
		// 总线卡存在但未初始化
		return -1001;
	}

	if (find->m_iCanStartIo <= port_id && find->m_iCanEndIo >= port_id)
	{
		// CAN IO口
		if (!find->m_bCanConnect)
		{
			// 总线卡已经初始化但是CAN模块未连接
			return -1003;
		}
	}

	// 初始化成功
	return 0;
}