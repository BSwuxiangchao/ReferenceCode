#include "DevHXS1606SelectValve.h"
#include "CRC.h"

DevHXS1606SelectValve::DevHXS1606SelectValve()
{

}
DevHXS1606SelectValve::DevHXS1606SelectValve(QString strPortName,
	QSerialPort::BaudRate portBaudRate,
	QSerialPort::DataBits portDataBits,
	QSerialPort::FlowControl portFlowControl,
	QSerialPort::Parity portParity,
	QSerialPort::StopBits portStopBits)
	:BgiSerialPort(strPortName, portBaudRate, portDataBits, portFlowControl, portParity, portStopBits)
{

}
DevHXS1606SelectValve::~DevHXS1606SelectValve()
{

}

void DevHXS1606SelectValve::SetDevHXS1606SelectValvePort(QString strPortName,
	QSerialPort::BaudRate portBaudRate,
	QSerialPort::DataBits portDataBits,
	QSerialPort::FlowControl portFlowControl,
	QSerialPort::Parity portParity,
	QSerialPort::StopBits portStopBits)
{
	SetSerialPortPara(strPortName, portBaudRate, portDataBits, portFlowControl, portParity, portStopBits);
}

int DevHXS1606SelectValve::Open()
{
	bool bOpen = BgiSerialPort::Open();

	return bOpen ? 1 : 0;
}

int DevHXS1606SelectValve::Close()
{
	bool bOpen = BgiSerialPort::Close();

	return bOpen ? 1 : 0;
}

bool DevHXS1606SelectValve::MakeCmd(unsigned short addr,unsigned char funCode, unsigned char dataCode, unsigned char* cmd)
{
	cmd[0] = 0xa5;
	cmd[1] = addr;//地址
	cmd[2] = funCode;
	cmd[3] = dataCode;
	cmd[4] = 0x5a;
	unsigned short cval = CRC::calculateCRC16(cmd, 5);
	cmd[5] = (cval >> 8) & 0xFF;
	cmd[6] = cval & 0xFF;
	return true;
}
HXRespData DevHXS1606SelectValve::DeCmd(unsigned short addr, unsigned short /*func*/,QByteArray arry)
{
	HXRespData data;
	data.errType = HXSRespErrType_OK;
	data.respSrcData = arry;
	QString resp(arry);
	if (resp.length() >= 7)
	{
		if (arry[0] == 0xa5 && arry[4] == 0x5a )
		{
			if (resp[1] == addr)
			{
				unsigned char* tempData = (unsigned char*)resp.data();
				unsigned short tempCRC = CRC::calculateCRC16(tempData, 5);
				if ((arry[5] << 8) + arry[6] == tempCRC)
				{
					/*data.highCRC = resp[5];
					data.lowCRC = resp[6];
					data.respData = resp[3];
					data.respFunc = resp[2];*/
					data.highCRC = resp.mid(5, 1);
					data.lowCRC = resp.mid(6, 1);
					data.respData = resp.mid(3, 1);
					data.respFunc = resp.mid(2, 1);
					bool ok;
					int val = data.respData.toInt(&ok, 16);
					if (ok)
					{
						if (0x80 == val || 0x81 == val || 0x82 == val || 0x83 == val)
						{
							data.errType = HXSRespErrType_ExcuteErr;

						}
					}
				}
				else {
					data.errType = HXSRespErrType_CRCErr;
				}
			}
			else {
				data.errType = HXSRespUnMatchDevice;
			}
		}
		else
		{
			data.errType = HXSRespErrType_Other;

		}
	}
	else {
		data.errType = HXSRespErrType_Other;
	}

	return data;
}

int DevHXS1606SelectValve::PraseRespToStatus(HXRespData data)
{
	int status = -1;
	if ( HXSRespErrType_OK == data.errType) 
	{
		status = 0;
	}
	else if (HXSRespErrType_CRCErr == data.errType) {
		status = 1;
	}
	else if (HXSRespUnMatchFunc == data.errType) {
		status = 2;
	}
	else if (HXSRespUnMatchDevice == data.errType) {
		status = 3;
	}
	else if (HXSRespErrType_ExcuteErr == data.errType)
	{
		status = 4;
	}
	else {
		status = 10;
	}
	return status;
}

//设置本机地址
void DevHXS1606SelectValve::SetAddress(unsigned short addr)
{
	unsigned char cmd[8];
	unsigned short val = addr;
	unsigned short func = 0x02;
	if (MakeCmd(0xff, func, val, cmd))
	{
		Wirte((char*)cmd);
	}
}
void DevHXS1606SelectValve::TryPraseSetAddress(QByteArray buf, int& status)
{
	unsigned short func = 0x02;
	HXRespData data = DeCmd(0xff, func, buf);
	status = PraseRespToStatus(data);
}

//获取本机地址
void DevHXS1606SelectValve::GetAddress()
{
	unsigned char cmd[8];
	int val = 0x00;
	unsigned short func = 0x03;
	if (MakeCmd(0xff, func, val, cmd))
	{
		Wirte((char*)cmd);
	}
}
void DevHXS1606SelectValve::TryPraseGetAddress(QByteArray buf, int& status, unsigned short& addr)
{
	unsigned short func = 0x03;
	HXRespData data = DeCmd(0xff, func, buf);
	status = PraseRespToStatus(data);
	if (HXSRespErrType_OK == data.errType)
	{
		bool ok;
		int val = data.respData.toInt(&ok, 16);
		if (ok)
		{
			addr = val;
		}
		else {
			status = 5;
		}
	}
}

//读取版本
void DevHXS1606SelectValve::ReadVersion(unsigned short addr)
{
	unsigned char cmd[8];
	int val = 0x00;
	unsigned short func = 0x08;
	if (MakeCmd(addr, func, val, cmd))
	{
		Wirte((char*)cmd);
	}
}
void DevHXS1606SelectValve::TryPraseReadVersion(unsigned short addr, QByteArray buf, int& status, int& major, int& sub)
{
	unsigned short func = 0x08;
	HXRespData data = DeCmd(addr, func, buf);
	status = PraseRespToStatus(data);
	if (HXSRespErrType_OK == data.errType)
	{
		bool ok;
		int val = data.respData.toInt(&ok, 16);
		if (ok) 
		{
			major = (val >> 4) & 0xF;
			sub = val & 0xF;
		}
		else {
			status = 5;
		}
	}
}

//回home
void DevHXS1606SelectValve::Home(unsigned short addr)
{
	unsigned char cmd[8];
	int val = 0x00;
	unsigned short func = 0x05;
	if (MakeCmd(addr, func, val, cmd))
	{
		Wirte((char*)cmd);
	}
}
void DevHXS1606SelectValve::TryPraseHome(unsigned short addr, QByteArray buf, int& status)
{
	unsigned short func = 0x05;
	HXRespData data = DeCmd(addr, func, buf);
	status = PraseRespToStatus(data);
}

//到位置
void DevHXS1606SelectValve::GotoPosition(unsigned short addr, unsigned short pos)
{
	unsigned char cmd[8];
	unsigned short val = pos;
	unsigned short func = 0x06;
	if (MakeCmd(addr, func, val, cmd))
	{
		Wirte((char*)cmd);
	}
}
void DevHXS1606SelectValve::TryPraseGotoPosition(unsigned short addr, QByteArray buf, int& status)
{
	unsigned short func = 0x06;
	HXRespData data = DeCmd(addr, func, buf);
	status = PraseRespToStatus(data);
}

//获取位置
void DevHXS1606SelectValve::GetPosition(unsigned short addr)
{
	unsigned char cmd[8];
	int val = 0x00;
	unsigned short func = 0x07;
	if (MakeCmd(addr, func, val, cmd))
	{
		Wirte((char*)cmd);
	}
}
void DevHXS1606SelectValve::TryPraseGetPosition(unsigned short addr, QByteArray buf, int& status, unsigned short& pos)
{
	unsigned short func = 0x07;
	HXRespData data = DeCmd(addr, func, buf);
	status = PraseRespToStatus(data);
	if (HXSRespErrType_OK == data.errType)
	{
		bool ok;
		int val = data.respData.toInt(&ok, 16);
		if (ok) {
			pos = val;
		}
		else {
			status = 5;
		}
	}
}

//设置校验
void DevHXS1606SelectValve::SetCheck(unsigned short addr, bool check)
{
	unsigned char cmd[8];
	int val = 0x01;
	val = check ? 0x01 : 0x00;
	unsigned short func = 0x09;
	if (MakeCmd(addr, func, val, cmd)) 
	{
		Wirte((char*)cmd);
	}
}

void DevHXS1606SelectValve::TryPraseSetCheck(unsigned short addr, QByteArray buf, int& status)
{
	unsigned short func = 0x09;
	HXRespData data = DeCmd(addr, func, buf);
	status = PraseRespToStatus(data);
}

//恢复出厂
void DevHXS1606SelectValve::ResetFactory(unsigned short addr)
{
	unsigned char cmd[8];
	int val = 0x00;
	unsigned short func = 0x01;
	if (MakeCmd(addr, func, val, cmd))
	{
		Wirte((char*)cmd);
	}
}
void DevHXS1606SelectValve::TryPraseResetFactory(unsigned short addr, QByteArray buf, int& status)
{
	unsigned short func = 0x01;
	HXRespData data = DeCmd(addr, func, buf);
	status = PraseRespToStatus(data);
}

//设置波特率
void DevHXS1606SelectValve::SetBaudRate(unsigned short addr, int baudindex)
{
	unsigned char cmd[8];
	int val = baudindex;
	unsigned short func = 0x04;
	if (MakeCmd(addr, func, val, cmd))
	{
		Wirte((char*)cmd);
	}
}
void DevHXS1606SelectValve::TryPraseSetBaudRate(unsigned short addr, QByteArray buf, int& status)
{
	unsigned short func = 0x04;
	HXRespData data = DeCmd(addr, func, buf);
	status = PraseRespToStatus(data);
}

//查正向补偿
void DevHXS1606SelectValve::QueryPositiveCompensition(unsigned short addr)
{
	unsigned char cmd[8];
	int val = 0x00;
	unsigned short func = 0x0C;
	if (MakeCmd(addr, func, val, cmd))
	{
		Wirte((char*)cmd);
	}
}
void DevHXS1606SelectValve::TryPraseQueryPositiveCompensition(unsigned short addr, QByteArray buf, int& status, int& compensate)
{
	unsigned short func = 0x0C;
	HXRespData data = DeCmd(addr, func, buf);
	status = PraseRespToStatus(data);
	if (HXSRespErrType_OK == data.errType)
	{
		bool ok;
		int val = data.respData.toInt(&ok, 16);
		if (ok) {
			compensate = val;
		}
		else {
			status = 5;
		}
	}
}

//查反向补偿
void DevHXS1606SelectValve::QueryNegtiveCompensition(unsigned short addr)
{
	unsigned char cmd[8];
	int val = 0x00;
	unsigned short func = 0x0E;
	if (MakeCmd(addr, func, val, cmd))
	{
		Wirte((char*)cmd);
	}
}
void DevHXS1606SelectValve::TryPraseQueryNegtiveCompensitition(unsigned short addr, QByteArray buf, int& status, int& compensate)
{
	unsigned short func = 0x0E;
	HXRespData data = DeCmd(addr, func, buf);
	status = PraseRespToStatus(data);
	if (HXSRespErrType_OK == data.errType)
	{
		bool ok;
		int val = data.respData.toInt(&ok, 16);
		if (ok) {
			compensate = val;
		}
		else {
			status = 5;
		}
	}
}

//查通信方式
void DevHXS1606SelectValve::QueryCommunicaitonType(unsigned short addr)
{
	unsigned char cmd[8];
	int val = 0x00;
	unsigned short func = 0x10;
	if (MakeCmd(addr, func, val, cmd))
	{
		Wirte((char*)cmd);
	}
}
void DevHXS1606SelectValve::TryPraseQueryCommunicationType(unsigned short addr, QByteArray buf, int& status, int& type)
{
	unsigned short func = 0x10;
	HXRespData data = DeCmd(addr, func, buf);
	status = PraseRespToStatus(data);
	if (HXSRespErrType_OK == data.errType)
	{
		bool ok;
		int val = data.respData.toInt(&ok, 16);
		if (ok) {
			type = val;
		}
		else {
			status = 5;
		}
	}
}

//设置通信方式
void DevHXS1606SelectValve::SetCommunicationType(unsigned short addr,int type)
{
	unsigned char cmd[8];
	int val = type;
	unsigned short func = 0x11;
	if (MakeCmd(addr, func, val, cmd))
	{
		Wirte((char*)cmd);
	}
}
void DevHXS1606SelectValve::TryPraseSetCommunicationType(unsigned short addr, QByteArray buf, int& status)
{
	unsigned short func = 0x11;
	HXRespData data = DeCmd(addr, func, buf);
	status = PraseRespToStatus(data);
}

//查最小速度  
void DevHXS1606SelectValve::QueryMinSpeed(unsigned short addr)
{
	unsigned char cmd[8];
	int val = 0x00;
	unsigned short func = 0x12;
	if (MakeCmd(addr, func, val, cmd))
	{
		Wirte((char*)cmd);
	}
}
void DevHXS1606SelectValve::TryPraseQueryMinSpeed(unsigned short addr, QByteArray buf, int& status, int& pulse)
{
	unsigned short func = 0x12;
	HXRespData data = DeCmd(addr, func, buf);
	status = PraseRespToStatus(data);
	if (HXSRespErrType_OK == data.errType)
	{
		bool ok;
		int val = data.respData.toInt(&ok, 16);
		if (ok) {
			pulse = val * 100;
		}
		else {
			status = 5;
		}
	}
}

//设置最小速度
void DevHXS1606SelectValve::SetMinSpeed(unsigned short addr, unsigned short pulse)
{
	unsigned char cmd[8];
	int val = pulse / 100;
	unsigned short func = 0x13;
	if (MakeCmd(addr, func, val, cmd))
	{
		Wirte((char*)cmd);
	}
}
void DevHXS1606SelectValve::TryPraseSetMinSpeed(unsigned short addr, QByteArray buf, int& status)
{
	unsigned short func = 0x13;
	HXRespData data = DeCmd(addr, func, buf);
	status = PraseRespToStatus(data);
}

//查最大速度
void DevHXS1606SelectValve::QueryMaxSpeed(unsigned short addr)
{
	unsigned char cmd[8];
	int val = 0x00;
	unsigned short func = 0x14;
	if (MakeCmd(addr, func, val, cmd))
	{
		Wirte((char*)cmd);
	}
}
void DevHXS1606SelectValve::TryPraseQueryMaxSpeed(unsigned short addr, QByteArray buf, int& status, int& pulse)
{
	unsigned short func = 0x14;
	HXRespData data = DeCmd(addr, func, buf);
	status = PraseRespToStatus(data);
	if (HXSRespErrType_OK == data.errType)
	{
		bool ok;
		int val = data.respData.toInt(&ok, 16);
		if (ok) {
			pulse = val * 100;
		}
		else {
			status = 5;
		}
	}
}

//设置最大速度
void DevHXS1606SelectValve::SetMaxSpeed(unsigned short addr, unsigned short pulse)
{
	unsigned char cmd[8];
	int val = pulse / 100;
	unsigned short func = 0x15;
	if (MakeCmd(addr, func, val, cmd))
	{
		Wirte((char*)cmd);
	}
}
void DevHXS1606SelectValve::TryPraseSetMaxSpeed(unsigned short addr, QByteArray buf, int& status)
{
	unsigned short func = 0x15;
	HXRespData data = DeCmd(addr, func, buf);
	status = PraseRespToStatus(data);
}