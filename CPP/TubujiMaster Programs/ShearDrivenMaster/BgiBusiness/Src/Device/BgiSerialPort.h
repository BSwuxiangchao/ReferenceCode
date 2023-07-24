/*
* @Abrief 串口
* @note 串口实现，多线程串口通讯
* @version 1.0
* @author mosongjin 2021-08-31
*/

#ifndef _BGI_SERIAL_PORT_H_
#define _BGI_SERIAL_PORT_H_


#include <qobject.h>
#include <qserialport.h>
#include <qthread.h>
#include "BgiLock.h"

#include "OperateRecordInfo.h"
#include "BgiProcID.h"
#include "CommonColors.h"

class  BgiSerialPort : public QObject
{
	Q_OBJECT
public:
	BgiSerialPort();
	BgiSerialPort(QString strPortName,
	QSerialPort::BaudRate portBaudRate,
	QSerialPort::DataBits portDataBits,
	QSerialPort::FlowControl portFlowControl,
	QSerialPort::Parity portParity,
	QSerialPort::StopBits portStopBits);
	virtual ~BgiSerialPort();

	void SetSerialPortPara(QString strPortName,
		QSerialPort::BaudRate portBaudRate,
		QSerialPort::DataBits portDataBits,
		QSerialPort::FlowControl portFlowControl,
		QSerialPort::Parity portParity,
		QSerialPort::StopBits portStopBits);

	bool Open();
	bool Close();
	void Wirte(QByteArray buf);
	virtual void WirteEx(QByteArray buf);
	virtual void DelayWirte(QByteArray buf,int mSec = 0);
	virtual void GetBuf(QByteArray& buf);
	virtual void ReadBuf();

signals:
	void SendBuf(QByteArray buf);
	void WirteBuf(QByteArray buf);

protected slots:
	void ReadData();

private slots:
	void WriteData(QByteArray buf);

protected:
	QByteArray m_ReadBuf;
	BgiLock m_BgiLock;
	QSerialPort* m_pSerialPort;

private:
	void RecordToLogFile(QString strContext, QColor color=gc_colorRed);
private:
	QThread* m_pThread;
	QString m_strPortName;
	QSerialPort::BaudRate m_portBaudRate;
	QSerialPort::DataBits m_portDataBits;
	QSerialPort::FlowControl m_portFlowControl;
	QSerialPort::Parity m_portParity;
	QSerialPort::StopBits m_portStopBits;



};

#endif 


