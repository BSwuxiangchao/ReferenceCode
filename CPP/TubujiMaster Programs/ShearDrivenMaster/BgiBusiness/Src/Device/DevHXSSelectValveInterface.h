/*
* @author NanGe 20210929
*/

#ifndef _HXSELECTVALVE_INTERFACE_H
#define _HXSELECTVALVE_INTERFACE_H

#include "DeviceComm.h"
#include "qbytearray.h"

class DevHXSelectValveInterface : public DeviceComm
{
public:
	virtual int Open() = 0;
	virtual int Close() = 0;
	virtual int Run(bool /*bWork*/) { return 1; }
	virtual void SetWorkPara(int /*iCardNo*/, int /*iPort*/){}

	//设置目标地址的新地址，通过广播地址0xff设置
	virtual void SetAddress(unsigned char addr) = 0;
	virtual void TryPraseSetAddress(unsigned char set_addr, QByteArray buf, int& status) = 0;

	//获取地址,通过广播地址0xff读取
	virtual void GetAddress() = 0;
	virtual void TryPraseGetAddress(QByteArray buf, int& status, unsigned char& addr) = 0;
	//读取版本
	virtual void ReadVersion(unsigned short addr) = 0;
	virtual void TryPraseReadVersion(unsigned short addr, QByteArray buf, int& status,int& major,int& sub) = 0;

	//回home
	virtual void Home( unsigned short addr ) = 0;
	virtual void TryPraseHome(unsigned short addr,QByteArray buf,int& status) = 0;

	//到位置
	virtual void GotoPosition(unsigned char addr,unsigned char pos) = 0;
	virtual void TryPraseGotoPosition(unsigned char addr, QByteArray buf, int& status) = 0;

	//获取位置
	virtual void GetPosition(unsigned char addr) = 0;
	virtual void TryPraseGetPosition(unsigned char addr, QByteArray buf, int& status,unsigned char& pos) = 0;

	//设置校验
	virtual void SetCheck(unsigned short addr, bool check) = 0;
	virtual void TryPraseSetCheck(unsigned short addr, QByteArray buf, int& status) = 0;

	//恢复出厂
	virtual void ResetFactory(unsigned short addr) = 0;
	virtual void TryPraseResetFactory(unsigned short addr, QByteArray buf, int& status) = 0;

	//设置波特率
	virtual void SetBaudRate(unsigned short addr,int baudindex) = 0;
	virtual void TryPraseSetBaudRate(unsigned short addr, QByteArray buf, int& status) = 0;

	//查正向补偿
	virtual void QueryPositiveCompensition(unsigned short addr) = 0;
	virtual void TryPraseQueryPositiveCompensition(unsigned short addr, QByteArray buf, int& status,int& compensate) = 0;

	//查反向补偿
	virtual void QueryNegtiveCompensition(unsigned short addr) = 0;
	virtual void TryPraseQueryNegtiveCompensitition(unsigned short addr, QByteArray buf, int& status, int& compensate) = 0;

	//查通信方式  0 立即响应   1完成后响应
	virtual void QueryCommunicaitonType(unsigned short addr) = 0;
	virtual void TryPraseQueryCommunicationType(unsigned short addr, QByteArray buf, int& status,int& type) = 0;

	//设置通信方式 0 立即响应   1完成后响应
	virtual void SetCommunicationType(unsigned short addr,int type) = 0;
	virtual void TryPraseSetCommunicationType(unsigned short addr, QByteArray buf, int& status) = 0;

	//查最小速度  每秒脉数 100的整数
	virtual void QueryMinSpeed(unsigned short addr) = 0;
	virtual void TryPraseQueryMinSpeed(unsigned short addr, QByteArray buf, int& status,int& pulse) = 0;

	//设置最小速度 每秒脉数 100的整数
	virtual void SetMinSpeed(unsigned short addr, unsigned short pulse) = 0;
	virtual void TryPraseSetMinSpeed(unsigned short addr, QByteArray buf, int& status) =0 ;

	//查最大速度 每秒脉数 100的整数
	virtual void QueryMaxSpeed(unsigned short addr) = 0;
	virtual void TryPraseQueryMaxSpeed(unsigned short addr, QByteArray buf, int& status,int& pulse) = 0;

	//设置最大速度 每秒脉数 100的整数
	virtual void SetMaxSpeed(unsigned short addr, unsigned short pulse) = 0;
	virtual void TryPraseSetMaxSpeed(unsigned short addr, QByteArray buf, int& status) = 0;

};
#endif//_HXSELECTVALVE_INTERFACE_H