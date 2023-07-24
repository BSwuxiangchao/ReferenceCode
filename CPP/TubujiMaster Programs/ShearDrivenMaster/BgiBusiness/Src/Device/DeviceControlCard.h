/*
* @Abrief 设备控制卡
* @note 设备控制卡控制的设备：流体电子阀开关，气压传感器的电压值
* @version 1.0
* @author mosongjin 2021-11-17
*/

#include "DeviceIoControl.h"
#include <QMap>

#ifndef _DEVICE_CONTROL_CARD_H
#define _DEVICE_CONTROL_CARD_H

// 雷赛总线卡连接状态信息
struct _tagDmcBoardState
{
	// 总线卡所在系统名称
	QString m_strInSystemName;
	// 总线卡地址ID
	short m_iBoardId;
	// 连接状态
	bool m_bBoardConnect;
	// 总线卡卡号
	unsigned long m_iBoardCardId;

	// 扩展CAN模块数量
	short m_iCanCount;
	// CAN模块连接状态（全成功或全失败）
	bool m_bCanConnect;
	// 扩展模块的起始IO
	short m_iCanStartIo;
	// 扩展模块的结束IO
	short m_iCanEndIo;

	// DD马达连接状态
	bool m_bDDMotor;
	_tagDmcBoardState();
	_tagDmcBoardState& operator=(const _tagDmcBoardState& obj);
};

class DeviceControlCard : public DeviceIoControl
{
public:
	DeviceControlCard();
	virtual ~DeviceControlCard();

	virtual int SwitchFluidPort(int card_id, int port_id, bool bOpen);
	virtual int ReadPortValue(int card_id, int port_id, unsigned short& value);
	virtual short GetElecPressure(int card_id, int channel, double* elev_value);
	//以绝对运动(1)或相对运动(0)的方式进行移动
	short MoveTargetDist(int card_id, int port_id, double dist, int mode);

	// 初始化雷赛控制卡
	void InitBoard();

protected:
	
/************************************* start-XCW *****************************************
 * 作    者：XCW
 * 功能描述：控制IO口是判断该IO口是否已经初始化成功;
 * 时    间：2022/9/14 13:56
 * 入    参：card_id：总线卡ID
 * 入    参：port_id：IO口
 * 出    参：无
 * 返    回：0-已初始化，-1001-总线卡未初始化，-1002-总线卡不存在，-1003-总线卡已经初始化但是CAN模块未连接
 ************************************* end-XCW *****************************************/
	short FindIoPortInit(const int& card_id, const int& port_id);

private:
	int m_iBoardCount;

	// 使用总线卡ID号作为标识符
	QMap<short, _tagDmcBoardState> m_mapBoard;
};

#endif //_DEVICE_CONTROL_CARD_H

