/*
* @author NanGe 20210929
*/
#ifndef _DEVSCANNER_DSDR_H
#define _DEVSCANNER_DSDR_H
#include "DevHXSSelectValveInterface.h"
#include "BgiSerialPort.h"
#include <qstring.h>
enum HXSRespErrType
{
	HXSRespErrType_OK,
	HXSRespErrType_CRCErr,
	HXSRespUnMatchDevice,
	HXSRespUnMatchFunc,
	HXSRespErrType_ExcuteErr,
	HXSRespErrType_Other
};

//0x10：无效参数
//0x11：无效指令
//0x12：未检测到零位
//0x13：未检测到码盘
//0x14：无效孔位
//0x04: 设备忙

struct HXRespData
{
	QByteArray respSrcData;
	QString respFunc;
	QString respData;
	QString highCRC;
	QString lowCRC;
	HXSRespErrType errType;
};


class DevHXS1606SelectValve : public DevHXSelectValveInterface,public BgiSerialPort
{
public:
	DevHXS1606SelectValve();
	DevHXS1606SelectValve(QString strPortName,
		QSerialPort::BaudRate portBaudRate,
		QSerialPort::DataBits portDataBits,
		QSerialPort::FlowControl portFlowControl,
		QSerialPort::Parity portParity,
		QSerialPort::StopBits portStopBits);
	virtual ~DevHXS1606SelectValve();

	int Open();
	int Close();

	void SetDevHXS1606SelectValvePort(QString strPortName,
		QSerialPort::BaudRate portBaudRate,
		QSerialPort::DataBits portDataBits,
		QSerialPort::FlowControl portFlowControl,
		QSerialPort::Parity portParity,
		QSerialPort::StopBits portStopBits);

	//设置目标地址的新地址，通过广播地址0xff设置
	void SetAddress(unsigned short addr);
	void TryPraseSetAddress(QByteArray buf, int& status);
	
	//获取地址,通过广播地址0xff读取
	void GetAddress();
	void TryPraseGetAddress(QByteArray buf, int& status, unsigned short& addr);

	//读取版本
	void ReadVersion(unsigned short addr);
	void TryPraseReadVersion(unsigned short addr, QByteArray buf, int& status, int& major, int& sub);

	//回home
	void Home(unsigned short addr);
	void TryPraseHome(unsigned short addr, QByteArray buf, int& status);

	//到位置
	void GotoPosition(unsigned short addr, unsigned short pos);
	void TryPraseGotoPosition(unsigned short addr, QByteArray buf, int& status);

	//获取位置
	void GetPosition(unsigned short addr);
	void TryPraseGetPosition(unsigned short addr, QByteArray buf, int& status, unsigned short& pos);

	//设置校验
	void SetCheck(unsigned short addr, bool check) ;
	void TryPraseSetCheck(unsigned short addr, QByteArray buf, int& status);

	//恢复出厂
	void ResetFactory(unsigned short addr);
	void TryPraseResetFactory(unsigned short addr, QByteArray buf, int& status);

	//设置波特率
	void SetBaudRate(unsigned short addr, int baudindex);
	void TryPraseSetBaudRate(unsigned short addr, QByteArray buf, int& status);

	//查正向补偿
	void QueryPositiveCompensition(unsigned short addr);
	void TryPraseQueryPositiveCompensition(unsigned short addr, QByteArray buf, int& status, int& compensate);

	//查反向补偿
	void QueryNegtiveCompensition(unsigned short addr);
	void TryPraseQueryNegtiveCompensitition(unsigned short addr, QByteArray buf, int& status, int& compensate);

	//查通信方式
	void QueryCommunicaitonType(unsigned short addr);
	void TryPraseQueryCommunicationType(unsigned short addr, QByteArray buf, int& status, int& type);

	//设置通信方式
	void SetCommunicationType(unsigned short addr,int type);
	void TryPraseSetCommunicationType(unsigned short addr, QByteArray buf, int& status);

	//查最小速度  
	void QueryMinSpeed(unsigned short addr);
	void TryPraseQueryMinSpeed(unsigned short addr, QByteArray buf, int& status, int& pulse);

	//设置最小速度
	void SetMinSpeed(unsigned short addr, unsigned short pulse);
	void TryPraseSetMinSpeed(unsigned short addr, QByteArray buf, int& status);

	//查最大速度
	void QueryMaxSpeed(unsigned short addr);
	void TryPraseQueryMaxSpeed(unsigned short addr, QByteArray buf, int& status, int& pulse);

	//设置最大速度
	void SetMaxSpeed(unsigned short addr, unsigned short pulse);
	void TryPraseSetMaxSpeed(unsigned short addr, QByteArray buf, int& status);
protected:
	bool MakeCmd(unsigned short addr,unsigned char funCode, unsigned char dataCode, unsigned char* cmd);
	HXRespData DeCmd(unsigned short addr,unsigned short func, QByteArray arry);
	int PraseRespToStatus(HXRespData data);
};

#endif // _DEVSCANNER_DSDR_H
