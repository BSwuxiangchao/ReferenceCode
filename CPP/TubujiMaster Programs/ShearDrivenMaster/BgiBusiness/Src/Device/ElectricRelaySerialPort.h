/*
* @Abrief 用于继电器的串口
* @note 不同的设备在通讯格式上有区别，此继电器的格式为帧头0x22,共8个字符
* @version 1.0
* @author mosongjin 2021-08-31
*/

#ifndef _ELECTRIC_RELAY_SERIAL_PORT_H_
#define _ELECTRIC_RELAY_SERIAL_PORT_H_


#include <qobject.h>
#include <qserialport.h>
#include <qthread.h>
#include "BgiLock.h"
#include "BgiSerialPort.h"
#include <qmap.h>

class  ElectricRelaySerialPort : public BgiSerialPort
{
	Q_OBJECT
public:
	ElectricRelaySerialPort();
	ElectricRelaySerialPort(QString strPortName,
	QSerialPort::BaudRate portBaudRate,
	QSerialPort::DataBits portDataBits,
	QSerialPort::FlowControl portFlowControl,
	QSerialPort::Parity portParity,
	QSerialPort::StopBits portStopBits);
	virtual ~ElectricRelaySerialPort();

	virtual void GetBuf(QByteArray& buf);
	bool GetBuf(int addr, QByteArray& buf);
	virtual void ReadBuf();

private:
	QMap<int, QByteArray> m_ReadBufMap;

};

#endif 


