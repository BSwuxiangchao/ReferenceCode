/*
* @Abrief 继电器控制模块
* @note 继电器控制模板提供了合成工装上的电子阀控制、流量计清零功能
* @version 1.0
* @author mosongjin 2022-01-20
*/

#include "DeviceIoControl.h"

#ifndef _ELECTRIC_RELAY_COMM_H_
#define _ELECTRIC_RELAY_COMM_H_

#include "ElectricRelaySerialPort.h"
#include <qstring.h>

class ElectricRelayComm : public DeviceIoControl
{
public:
	ElectricRelayComm();
	virtual ~ElectricRelayComm();

	bool CheckId(QByteArray arry);

	virtual int SwitchFluidPort(int card_id, int port_id, bool bOpen);
	virtual int ReadPortValue(int card_id, int port_id, unsigned short& value);
	virtual short GetElecPressure(int card_id, int channel, double* elev_value);
	
	void SetTakingBuffWay(bool bDirectRead);
	void TryPraseSetElectricIO(QByteArray buf, int& status);
private:
	ElectricRelaySerialPort* m_pSerialPort;
	bool m_bDirectRead;
	unsigned char m_cardId;
	unsigned char m_portId;
};

#endif // _ELECTRIC_RELAY_COMM_H_
