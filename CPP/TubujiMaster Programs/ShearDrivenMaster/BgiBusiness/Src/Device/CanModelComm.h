/*** CAN模块控制 ***/
#pragma once
#include "DeviceComm.h"

class CanModelComm : public DeviceComm
{
public:
	int Open();
	int Close();
	int Run(bool bWork);
	void SetWorkPara(int iCardNo, int iPort);

private:
	//运动卡的编号
	int m_iCardNo;
	//光纤的电平端口
	int m_iPort;
};

