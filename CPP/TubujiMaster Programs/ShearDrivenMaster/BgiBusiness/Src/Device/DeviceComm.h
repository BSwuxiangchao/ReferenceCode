/*
* @Abrief 设备通讯的基类
* @note 设备通讯的基类，转盘、光纤、振盘(物料输送)、相机、吹气阀为设备类
* @version 1.0
* @author mosongjin 2021-08-06
*/


#ifndef _DEVICE_COMM_H
#define _DEVICE_COMM_H


class DeviceComm
{
public:
	DeviceComm() {};
	virtual ~DeviceComm() {};

	virtual int Open() = 0;
	virtual int Close() = 0;
	virtual int Run(bool bWork) = 0;
	virtual void SetWorkPara(int iCardNo, int iPort) = 0;

};


#endif //_DEVICE_COMM_H

