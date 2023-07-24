/*
* @Abrief 设备IO控制类
* @note 设备IO控制类，子类有雷赛控制卡(高通量合成)，继电器控制模块(合成工装)；控制的设备：流体电子阀开关，气压传感器的电压值
* @version 1.0
* @author mosongjin 2022-01-20
*/


#ifndef _DEVICE_IO_CONTROL_H
#define _DEVICE_IO_CONTROL_H

class DeviceIoControl
{
public:
	DeviceIoControl() {};
	virtual ~DeviceIoControl() {};

	virtual int SwitchFluidPort(int card_id, int port_id, bool bOpen) = 0;
	virtual int ReadPortValue(int card_id, int port_id, unsigned short& value) = 0;
	virtual short GetElecPressure(int card_id, int channel, double* elev_value) = 0;
};

#endif //_DEVICE_IO_CONTROL_H

